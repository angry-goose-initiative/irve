/**
 * @brief   Minimal GDB server implementation to ease debugging
 * 
 * @copyright
 *  Copyright (C) 2023-2024 John Jekel\n
 *  See the LICENSE file at the root of the project for licensing info.
 * 
 * Thanks https://beej.us/guide/bgnet !!!
 * Also https://medium.com/@tatsuo.nomura/implement-gdb-remote-debug-protocol-stub-from-scratch ...
 *
*/

#if defined (__unix__) || (defined (__APPLE__) && defined (__MACH__))
#else
#error "The IRVE emulator only has GDB support on Unix-like systems"
#endif

/* ------------------------------------------------------------------------------------------------
 * Includes
 * --------------------------------------------------------------------------------------------- */

#include "common.h"
#include "emulator.h"
#include "memory.h"
#include "gdbserver.h"

#define INST_COUNT 0
#include "logging.h"

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#include <cassert>
#include <cstdint>
#include <cstring>
#include <string>
#include <variant>
#include <optional>

#if  __has_include(<expected>)
#include <expected>
#else

#warning "Your compiler doesn't support std::expected, GDB support may be limited"

namespace std {
    //Terrible minimal implementation until more compilers have C++23 support
    template <typename T, typename E>
    using expected = std::variant<T, E>;

    template <typename T, typename E>
    bool operator!(const expected<T, E>& expected) {
        return std::holds_alternative<E>(expected);
    }

    template <typename T, typename E>
    T& operator*(expected<T, E>& expected) {
        assert(std::holds_alternative<T>(expected));
        return std::get<T>(expected);
    }

    template <typename E>
    E unexpected(E error) {
        return error;
    }
}

#endif

using namespace irve::internal;

/* ------------------------------------------------------------------------------------------------
 * Constants/Defines
 * --------------------------------------------------------------------------------------------- */

#define RECIEVE_BUFFER_SIZE 4096

/* ------------------------------------------------------------------------------------------------
 * Type/Class Declarations
 * --------------------------------------------------------------------------------------------- */

enum class special_packet_t {
    ACK,
    NACK,
    CTRLC,
    CORRUPT,
    MALFORMED,
    DISCONNECTED//This is not actually a packet (indicates the client disconnected)
};

typedef std::variant<std::string, special_packet_t> packet_t;//You can tell I'm missing Rust

class unused_t {};

/* ------------------------------------------------------------------------------------------------
 * Static Function Declarations
 * --------------------------------------------------------------------------------------------- */

static bool handle_recieved_packet(
    const packet_t&         packet,
    emulator::emulator_t&   emulator,
    CpuState& cpu_state,
    Memory&       memory,
    int                     connection_fd
);//Returns false if it's time to accept a new connection

static bool send_packet(int connection_fd, const packet_t& packet);
static packet_t recieve_packet(int connection_fd);
static std::optional<packet_t> recieve_packet_nonblocking(int connection_fd);
static packet_t raw_string_2_packet(const std::string& raw_string);
static std::string compute_checksum(const std::string& packet);
static std::string byte_2_string(uint8_t byte);

static std::expected<unused_t, int> nicesend(int connection_fd, const std::string& message, int flags = 0);
static std::expected<std::string, std::optional<int>> nicerecv(int connection_fd, int flags = 0);
static int setup_server_socket(uint16_t port);
static struct addrinfo* get_addrinfo_ll_ptr(uint16_t port);

/* ------------------------------------------------------------------------------------------------
 * Function Implementations
 * --------------------------------------------------------------------------------------------- */

void gdbserver::start(
    emulator::emulator_t& emulator,
    CpuState& cpu_state,
    Memory& memory,
    uint16_t port
) {
    int socket_file_descriptor = setup_server_socket(port);

    irvelog_always(0, "Alrighty, you can connect to port %d now! (Waiting for a connection...)", port);

    //Loop trying to accept connections
    while (true) {
        struct sockaddr_storage client_addr;
        socklen_t client_addr_size = sizeof(sockaddr_storage);
        int connection_fd = accept(socket_file_descriptor, (struct sockaddr*)(&client_addr), &client_addr_size);
        if (connection_fd == -1) {
            irvelog(0, "Failed to accept a connection, retrying...");
            continue;
        }

        irvelog_always(0, "Accepted a connection! Hello there! :)");

        //Loop communicating with the client
        bool keep_going = true;
        while (keep_going) {
            //Wait for a packet from the GDB client
            packet_t packet = recieve_packet(connection_fd);

            //Handle it
            keep_going = handle_recieved_packet(
                packet,
                emulator,
                cpu_state,
                memory,
                connection_fd
            );
        }

        close(connection_fd);
        irvelog_always(0, "Client disconnected. Cya later! (Waiting for another connection...)");
    }
}

/* ------------------------------------------------------------------------------------------------
 * Static Function Implementations
 * --------------------------------------------------------------------------------------------- */

static bool handle_recieved_packet(
    const packet_t&         packet,
    emulator::emulator_t&   emulator,
    CpuState& cpu_state,
    Memory&       memory,
    int                     connection_fd
) {//Returns false if it's time to accept a new connection
    //Extract the packet string from the packet (unless it's a special packet)
    std::string packet_string;
    if (std::holds_alternative<std::string>(packet)) {
        packet_string = std::get<std::string>(packet);
    } else {//Is a special packet
        switch (std::get<special_packet_t>(packet)) {
            case special_packet_t::ACK:             return true; //We don't care about ACKs
            case special_packet_t::NACK:            return false;//Disconnect since we didn't save the previous packet so we can't retry
            case special_packet_t::CTRLC:           return false;//Disconnect since we shouldn't really be getting this normally
            case special_packet_t::CORRUPT:         return false;//Again, disconnect if we get gibberish
            case special_packet_t::MALFORMED:       return false;//Same here
            case special_packet_t::DISCONNECTED:    return false;//The client disconnected gracefully (this is not actually a packet)
            default:                                assert(false && "Invalid special packet type"); break;
        }
    }

    if (packet_string.empty()) {
        //Disconnect if we get an empty packet, which should never occur/should be filtered out into a special packet
        return false;
    }

    auto send_sigint = [connection_fd] { send_packet(connection_fd, "S02"); };//SIGINT is 02

    char first_char = packet_string.at(0);
    packet_string.erase(0, 1);//Remove the first character
    switch (first_char) {
        case '?': {//Stop reason query
            send_sigint();//Just make up a reason
            break;
        }
        case 'g': {//Read all registers
            //This shows the order we must provide:
            //https://android.googlesource.com/toolchain/gdb.git/+/76f55a3e2a750d666fbe2e296125b31b4e792461/gdb-9.1/gdb/riscv-tdep.c
            
            std::string contents_of_registers;
            
            //General purpose registers
            for (std::size_t i = 0; i < 32; ++i) {
                //Little endian
                Word reg = cpu_state.get_r(i);
                contents_of_registers += byte_2_string(reg.bits( 7,  0).u);
                contents_of_registers += byte_2_string(reg.bits(15,  8).u);
                contents_of_registers += byte_2_string(reg.bits(23, 16).u);
                contents_of_registers += byte_2_string(reg.bits(31, 24).u);
            }

        //Then the PC (also little endian)
        Word pc = cpu_state.get_pc();
        contents_of_registers += byte_2_string(pc.bits( 7,  0).u);
        contents_of_registers += byte_2_string(pc.bits(15,  8).u);
        contents_of_registers += byte_2_string(pc.bits(23, 16).u);
        contents_of_registers += byte_2_string(pc.bits(31, 24).u);

            send_packet(connection_fd, contents_of_registers);
            break;
        }
        case 'G': {//Write all registers
            //This shows the order we must accept:
            //https://android.googlesource.com/toolchain/gdb.git/+/76f55a3e2a750d666fbe2e296125b31b4e792461/gdb-9.1/gdb/riscv-tdep.c
            
            //General purpose registers
            for (std::size_t i = 0; i < 32; i++) {
                //Little endian
                Word reg = 0;
                reg |= (uint32_t)(std::strtol(packet_string.substr(0, 2).c_str(), nullptr, 16));
                packet_string.erase(0, 2);
                reg |= (uint32_t)(std::strtol(packet_string.substr(0, 2).c_str(), nullptr, 16) << 8);
                packet_string.erase(0, 2);
                reg |= (uint32_t)(std::strtol(packet_string.substr(0, 2).c_str(), nullptr, 16) << 16);
                packet_string.erase(0, 2);
                reg |= (uint32_t)(std::strtol(packet_string.substr(0, 2).c_str(), nullptr, 16) << 24);
                packet_string.erase(0, 2);
                cpu_state.set_r(i, reg);
            }
            
            //Then the PC (also little endian)
            Word pc = 0;
            pc |= (uint32_t)(std::strtol(packet_string.substr(0, 2).c_str(), nullptr, 16));
            packet_string.erase(0, 2);
            pc |= (uint32_t)(std::strtol(packet_string.substr(0, 2).c_str(), nullptr, 16) << 8);
            packet_string.erase(0, 2);
            pc |= (uint32_t)(std::strtol(packet_string.substr(0, 2).c_str(), nullptr, 16) << 16);
            packet_string.erase(0, 2);
            pc |= (uint32_t)(std::strtol(packet_string.substr(0, 2).c_str(), nullptr, 16) << 24);
            packet_string.erase(0, 2);
            cpu_state.set_pc(pc);

            send_packet(connection_fd, "OK");
            
            break;
        }
        case 'm': {//Read memory
            packet_string.erase(0, packet_string.find_first_not_of(" "));//Remove leading whitespace//TODO other whitespace characters?
            Word address = (uint32_t)std::strtol(packet_string.substr(0, packet_string.find_first_of(",")).c_str(), nullptr, 16);//TODO is this correct if the address is > 8 bits?
            packet_string.erase(0, packet_string.find_first_of(",") + 1);//Remove the address
            Word length = (uint32_t)std::strtol(packet_string.c_str(), nullptr, 16);

            try {
                std::string memory_contents;

                for (Word i = 0; i.u < length.u; ++i) {
                    Word byte = memory.load(address + i, 0b000);
                    memory_contents += byte_2_string(byte.bits( 7,  0).u);
                }

                send_packet(connection_fd, memory_contents);
            } catch (const rv_trap::rvexception_t& e) {
                //Let GDB know we failed to read memory
                send_packet(connection_fd, "E00");
            }

            break;
        }
        case 'M': {//Write memory
            packet_string.erase(0, packet_string.find_first_not_of(" "));//Remove leading whitespace//TODO other whitespace characters?
            Word address = (uint32_t)std::strtol(packet_string.substr(0, packet_string.find_first_of(",")).c_str(), nullptr, 16);//TODO is this correct if the address is > 8 bits?
            packet_string.erase(0, packet_string.find_first_of(",") + 1);//Remove the address
            Word length = (uint32_t)std::strtol(packet_string.substr(0, packet_string.find_first_of(":")).c_str(), nullptr, 16);//TODO is this correct if the length is > 8 bits?
            packet_string.erase(0, packet_string.find_first_of(":") + 1);//Remove the length

            try {
                for (Word i = 0; i.u < length.u; ++i) {
                    Word byte = (uint32_t)(std::strtol(packet_string.substr(0, 2).c_str(), nullptr, 16));
                    packet_string.erase(0, 2);
                    memory.store(address + i, 0b000, byte);
                }

                send_packet(connection_fd, "OK");
            } catch (const rv_trap::rvexception_t& e) {
                //Let GDB know we failed to read memory
                send_packet(connection_fd, "E00");
            }

            emulator.flush_icache();//In case GDB added a breakpoint

            break;
        }
        case 'c':
        case 'C': {
            //TODO handle arguments properly (also note c and C aren't exactly the same)
            //TODO handle interrupts from GDB, not just breakpoints
            send_packet(connection_fd, "OK");

            while (!emulator.test_and_clear_breakpoint_encountered_flag()) {
                if (!emulator.tick()) {//TODO what if it wants to exit? Is this what we should do?
                    break;
                }

                auto mid_continue_packet = recieve_packet_nonblocking(connection_fd);

                if (mid_continue_packet) {
                    packet_t the_packet = *mid_continue_packet;
                    if (std::holds_alternative<special_packet_t>(the_packet)) {
                        if (std::get<special_packet_t>(the_packet) == special_packet_t::CTRLC) {
                            break;//GDB has requested us to stop the loop
                        } else if (std::get<special_packet_t>(the_packet) == special_packet_t::ACK) {
                            continue;//Ignore ACKs
                        }
                    }

                    //Otherwise, it's a normal packet, so exit the continue, then handle the packet
                    send_sigint();//TODO is this the right thing to do?
                    return handle_recieved_packet(
                        the_packet,
                        emulator,
                        cpu_state,
                        memory,
                        connection_fd
                    );
                }
            }

            send_sigint();//TODO is this the right thing to do?
            break;
        }
        case 's':
        case 'S': {//Single step
            //TODO also set address if specified
            emulator.tick();//TODO what if it wants to exit?
            send_sigint();//TODO is this the right thing to do?
            break;
        }
        default: {//Unknown / unimplemented command
            send_packet(connection_fd, "");
            break;
        }
    }

    return true;//Continue with this connection
}

static bool send_packet(int connection_fd, const packet_t& packet) {
    std::string raw_message;

    if (std::holds_alternative<std::string>(packet)) {
        std::string packet_string = std::get<std::string>(packet);
        raw_message = std::string("+$") + packet_string + "#" + compute_checksum(packet_string);
    } else {//Is a special packet
        switch (std::get<special_packet_t>(packet)) {
            case special_packet_t::ACK:     raw_message = "+"; break;
            case special_packet_t::NACK:    raw_message = "-"; break;
            default:                        assert(false && "Invalid special packet type to send"); break;
        }
    }

    irvelog(0, "\x1b[96mIRVE Replies\x1b[0m: \"\x1b[1m%s\x1b[0m\"", raw_message.c_str());

#if  __has_include(<expected>)
    return nicesend(connection_fd, raw_message).has_value();
#else
    return !!nicesend(connection_fd, raw_message);//Temporary hack until more compilers have C++23 support
#endif
}

static packet_t recieve_packet(int connection_fd) {
    auto recv_result = nicerecv(connection_fd);

    if (!recv_result) {
        //Treat both errors (int error) and disconnects (nullopt error) as disconnects
        return special_packet_t::DISCONNECTED;
    }

    std::string raw_message = *recv_result;
    irvelog(0, "\x1b[95mGDB Says\x1b[0m:     \"\x1b[1m%s\x1b[0m\"", raw_message.c_str());

    return raw_string_2_packet(raw_message);
}

//[[maybe_unused]] is needed when <expected> is not available
static std::optional<packet_t> recieve_packet_nonblocking([[maybe_unused]] int connection_fd) {
#if  __has_include(<expected>)
    auto recv_result = nicerecv(connection_fd, MSG_DONTWAIT);//FIXME MSG_DONTWAIT is not portable

    if (!recv_result) {
        std::optional<int> error = recv_result.error();
        if (error) {
            switch (*error) {
                case EAGAIN:
                //case EWOULDBLOCK://FIXME duplicate case value
                    return std::nullopt;//No packet available yet
                default://Treat other errors as disconnects
                    return special_packet_t::DISCONNECTED;//FIXME handle this
            }
        } else {//Graceful client disconnect
            return special_packet_t::DISCONNECTED;
        }
    }

    std::string raw_message = *recv_result;
    irvelog(0, "\x1b[95mGDB Says\x1b[0m:     \"\x1b[1m%s\x1b[0m\"", raw_message.c_str());

    return raw_string_2_packet(raw_message);
#else
    return std::nullopt;//Unable to recieve packets in a nonblocking manner
#endif
}

static packet_t raw_string_2_packet(const std::string& raw_string) {
    std::string raw_message;
    try {
        //TODO in the future actually validate the format/checksum of the packet
        raw_message = raw_string.substr(raw_string.find('$') + 1);//Remove the '$' at the beginning
        raw_message.erase(raw_message.size() - 3);//Remove the checksum and '#' at the end
        return raw_message;
    } catch (std::out_of_range& e) {//Not a regular packet
        if (raw_message == "+") {
            return special_packet_t::ACK;
        } else if (raw_message == "-") {
            return special_packet_t::NACK;
        } else if (raw_message == "\x03") {//Ctrl+C
            return special_packet_t::CTRLC;
        } else {
            return special_packet_t::MALFORMED;
        }
    }
}

static std::string compute_checksum(const std::string& packet) {
    char accumulator = 0;

    for (size_t i = 0; i < packet.size(); ++i) {
        accumulator += packet[i];
    }

    return byte_2_string(accumulator);
}

static std::string byte_2_string(uint8_t byte) {
    uint8_t lower_nibble = byte & 0x0F;
    char lower_char = (lower_nibble < 10) ? ('0' + lower_nibble) : ('a' + (lower_nibble - 10));
    uint8_t upper_nibble = (byte >> 4) & 0x0F;
    char upper_char = (upper_nibble < 10) ? ('0' + upper_nibble) : ('a' + (upper_nibble - 10));

    return std::string{upper_char, lower_char};
}

static std::expected<unused_t, int> nicesend(int connection_fd, const std::string& message, int flags) {
    int send_result = send(connection_fd, message.c_str(), message.size(), flags);

    //TODO handle the case where only part of the message was sent

    if (send_result == -1) {
        return std::unexpected(errno);
    } else {
        return unused_t{};
    }
}

static std::expected<std::string, std::optional<int>> nicerecv(int connection_fd, int flags) {
    char buffer[RECIEVE_BUFFER_SIZE];
    //TODO handle the case where the message is larger than the buffer (we have to recieve, push to the string, and repeat)

    int recv_result = recv(connection_fd, buffer, RECIEVE_BUFFER_SIZE - 1, flags);
    
    if (recv_result == -1) {//An error occured
        return std::unexpected(errno);
    } else if (recv_result == 0) {//The client disconnected gracefully
        return std::unexpected(std::nullopt);
    } else {
        buffer[recv_result] = '\0';//Null terminate the string
        return std::string(buffer);
    }
}

static int setup_server_socket(uint16_t port) {
    //Get the linked list of addrinfo structs to attempt to setup a socket for
    struct addrinfo* addrinfo_ll_ptr = get_addrinfo_ll_ptr(port);

    //Loop through the list and setup a socket for the first one we can
    [[maybe_unused]] int attempt = 1;//FIXME avoid warning about being unused when logging is disabled
    int socket_file_descriptor = -1;
    for (struct addrinfo* addrinfo_node_ptr = addrinfo_ll_ptr; addrinfo_node_ptr; addrinfo_node_ptr = addrinfo_node_ptr->ai_next) {
        //Attempt to create a socket based on this addrinfo struct
        socket_file_descriptor = socket(addrinfo_node_ptr->ai_family, addrinfo_node_ptr->ai_socktype, addrinfo_node_ptr->ai_protocol);
        if (socket_file_descriptor == -1) {
            irvelog(0, "Failed to create a socket for addrinfo struct #%d, trying the next one...", attempt);
            ++attempt;
            continue;
        }

        //If the port was already in use, try to reuse the address to avoid errors
        int we_indeed_want_to_reuse_the_address = 1;
        int setsockopt_result = setsockopt(socket_file_descriptor, SOL_SOCKET, SO_REUSEADDR, &we_indeed_want_to_reuse_the_address, sizeof(int));
        if (setsockopt_result == -1) {
            irvelog(0, "Failed to set the SO_REUSEADDR socket option for addrinfo struct #%d, trying the next one...", attempt);
            close(socket_file_descriptor);
            socket_file_descriptor = -1;
            ++attempt;
            continue;
        }

        //Bind the socket to the IP address in the addrinfo struct
        int bind_result = bind(socket_file_descriptor, addrinfo_node_ptr->ai_addr, addrinfo_node_ptr->ai_addrlen);
        if (bind_result == -1) {
            irvelog(0, "Failed to bind to the socket for addrinfo struct #%d, trying the next one...", attempt);
            close(socket_file_descriptor);
            socket_file_descriptor = -1;
            ++attempt;
            continue;
        } else {
            irvelog(0, "Created and bound to a socket succesfully, we can move on! :)");
            break;
        }
    }
    
    //Free the linked list
    freeaddrinfo(addrinfo_ll_ptr);

    //See if we were successful
    if (socket_file_descriptor == -1) {
        irvelog(0, "Failed to create a socket!");
        exit(1);
    }

    //Listen on the socket (We only support a single client)
    int listen_result = listen(socket_file_descriptor, 1);
    if (listen_result == -1) {
        irvelog(0, "Failed to listen on the socket!");
        exit(1);
    }

    //Return the socket file descriptor that we set up
    return socket_file_descriptor;
}

static struct addrinfo* get_addrinfo_ll_ptr(uint16_t port) {
    irvelog(0, "Calling getaddrinfo()...");

    //We need the port as a string
    std::string port_string = std::to_string(port);
    const char* port_cstr = port_string.c_str();

    //Options/"Hints" to getaddrinfo
    struct addrinfo getaddrinfo_hints;
    std::memset(&getaddrinfo_hints, 0, sizeof(addrinfo));//Zero the struct
    getaddrinfo_hints.ai_family     = AF_UNSPEC;//TODO is this correct?
    getaddrinfo_hints.ai_socktype   = SOCK_STREAM;//We want TCP
    getaddrinfo_hints.ai_flags      = AI_PASSIVE;//Have getaddrinfo get treat nullptr as localhost

    //The actual getaddrinfo call
    struct addrinfo* getaddrinfo_ll_ptr;
    int getaddrinfo_result = getaddrinfo(nullptr, port_cstr, &getaddrinfo_hints, &getaddrinfo_ll_ptr);

    //Error handling and returning results
    if (getaddrinfo_result) {
        irvelog(0, "Boom! Call to getaddrinfo failed: %s", gai_strerror(getaddrinfo_result));
        exit(1);
    } else {
        return getaddrinfo_ll_ptr;
    }
}
