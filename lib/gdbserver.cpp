/* gdbserver.cpp
 * Copyright (C) 2023 John Jekel and Nick Chan
 * See the LICENSE file at the root of the project for licensing info.
 *
 * Minimal GDB server implementation to ease debugging
 *
 * Thanks https://beej.us/guide/bgnet !!!
 * Also https://medium.com/@tatsuo.nomura/implement-gdb-remote-debug-protocol-stub-from-scratch ...
*/

#ifdef __unix__

/* Constants And Defines */

#define RECIEVE_BUFFER_SIZE 1024

/* Includes */

#include "common.h"
#include "emulator.h"
#include "memory.h"
#include "gdbserver.h"

#define INST_COUNT 0
#include "logging.h"

#include <cassert>
#include <cstdint>
#include <cstring>
#include <string>
#include <variant>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

using namespace irve::internal;

/* Types */

enum class special_packet_t {
    ACK,
    NACK,
    CORRUPT,
    MALFORMED,
    DISCONNECTED
};

typedef std::variant<std::string, special_packet_t> packet_t;//You can tell I'm missing Rust

/* Variables */

//TODO

/* Static Function Declarations */

static bool client_loop(
    emulator::emulator_t& emulator,
    cpu_state::cpu_state_t& cpu_state,
    memory::memory_t& memory,
    int connection_file_descriptor
);//Returns false if it's time to accept a new connection

static bool send_packet(int connection_file_descriptor, const packet_t& packet);//An empty string means the client disconnected
static packet_t recieve_packet(int connection_file_descriptor);//An empty string means the client disconnected
static std::string compute_checksum(const std::string& packet);
static std::string byte_2_string(uint8_t byte);

static bool nicesend(int connection_file_descriptor, const std::string& message);
static std::string nicerecv(int connection_file_descriptor);//If the string is empty, this means the client disconnected
static int setup_server_socket(uint16_t port);
static struct addrinfo* get_addrinfo_ll_ptr(uint16_t port);

/* Function Implementations */

void gdbserver::start(emulator::emulator_t& emulator, uint16_t port) {
    int socket_file_descriptor = setup_server_socket(port);

    irvelog_always(0, "Alrighty, you can connect to port %d now! (Waiting for a connection...)", port);

    //Loop trying to accept connections
    while (true) {
        struct sockaddr_storage client_addr;
        socklen_t client_addr_size = sizeof(sockaddr_storage);
        int connection_file_descriptor = accept(socket_file_descriptor, (struct sockaddr*)(&client_addr), &client_addr_size);
        if (connection_file_descriptor == -1) {
            irvelog(0, "Failed to accept a connection, retrying...");
            continue;
        }

        irvelog_always(0, "Accepted a connection! Hello there! :)");

        //Loop communicating with the client
        while (client_loop(emulator, emulator.m_cpu_state, emulator.m_memory, connection_file_descriptor));

        close(connection_file_descriptor);
        irvelog_always(0, "Client disconnected. Cya later! (Waiting for another connection...)");
    }
}

/* Static Function Implementations */

static bool client_loop(
    emulator::emulator_t& /*emulator*/,
    cpu_state::cpu_state_t& cpu_state,
    memory::memory_t& /*memory*/,
    int connection_file_descriptor
) {//Returns false if it's time to accept a new connection
    //Get a packet from the GDB client
    packet_t packet = recieve_packet(connection_file_descriptor);
    std::string packet_string;
    if (std::holds_alternative<std::string>(packet)) {
        packet_string = std::get<std::string>(packet);
    } else {//Is a special packet
        switch (std::get<special_packet_t>(packet)) {
            case special_packet_t::ACK:             return true;//We don't care about ACKs
            case special_packet_t::NACK:            assert(false && "NACKs should never be recieved"); break;//TODO proper error handling
            case special_packet_t::CORRUPT:         return false;//Assume a disconnect//TODO should we actually retry in this case?
            case special_packet_t::MALFORMED:       return false;//Assume a disconnect//TODO should we actually retry in this case?
            case special_packet_t::DISCONNECTED:    return false;//The client disconnected
            default:                                assert(false && "Invalid special packet type"); break;
        }
    }

    //We got a packet, now we need to handle it
    if (packet_string == "?") {
        //Make up a reason why we stopped
        send_packet(connection_file_descriptor, "S03");//SIGQUIT
    } else if (packet_string == "g") {//Read all registers
        //This shows the order we must provide:
        //https://android.googlesource.com/toolchain/gdb.git/+/76f55a3e2a750d666fbe2e296125b31b4e792461/gdb-9.1/gdb/riscv-tdep.c
        
        std::string contents_of_registers;
        
        //General purpose registers
        for (std::size_t i = 0; i < 32; i++) {
            //Little endian
            word_t reg = cpu_state.get_r(i);
            contents_of_registers += byte_2_string(reg.bits( 7,  0).u);
            contents_of_registers += byte_2_string(reg.bits(15,  8).u);
            contents_of_registers += byte_2_string(reg.bits(23, 16).u);
            contents_of_registers += byte_2_string(reg.bits(31, 24).u);
        }

        //Then the PC (also little endian)
        word_t pc = cpu_state.get_pc();
        contents_of_registers += byte_2_string(pc.bits( 7,  0).u);
        contents_of_registers += byte_2_string(pc.bits(15,  8).u);
        contents_of_registers += byte_2_string(pc.bits(23, 16).u);
        contents_of_registers += byte_2_string(pc.bits(31, 24).u);

        send_packet(connection_file_descriptor, contents_of_registers);
    } else if (!packet_string.empty() && (packet_string.at(0) == 'G')) {//Write all registers
        packet_string.erase(0, 1);//Remove the 'G' at the beginning

        //This shows the order we must accept:
        //https://android.googlesource.com/toolchain/gdb.git/+/76f55a3e2a750d666fbe2e296125b31b4e792461/gdb-9.1/gdb/riscv-tdep.c
        
        //General purpose registers
        for (std::size_t i = 0; i < 32; i++) {
            //Little endian
            word_t reg = 0;
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
        word_t pc = 0;
        pc |= (uint32_t)(std::strtol(packet_string.substr(0, 2).c_str(), nullptr, 16));
        packet_string.erase(0, 2);
        pc |= (uint32_t)(std::strtol(packet_string.substr(0, 2).c_str(), nullptr, 16) << 8);
        packet_string.erase(0, 2);
        pc |= (uint32_t)(std::strtol(packet_string.substr(0, 2).c_str(), nullptr, 16) << 16);
        packet_string.erase(0, 2);
        pc |= (uint32_t)(std::strtol(packet_string.substr(0, 2).c_str(), nullptr, 16) << 24);
        packet_string.erase(0, 2);
        cpu_state.set_pc(pc);

        send_packet(connection_file_descriptor, "OK");
    } else if (!packet_string.empty() && (packet_string.at(0) == 'm')) {//Read memory
        assert(false && "TODO");//TODO
    } else if (!packet_string.empty() && (packet_string.at(0) == 'M')) {//Write memory
        assert(false && "TODO");//TODO
    } else if (!packet_string.empty() && (packet_string.at(0) == 'c')) {//Continue
        assert(false && "TODO");//TODO
    } else if (!packet_string.empty() && (packet_string.at(0) == 's')) {//Single step
        assert(false && "TODO");//TODO
    } else {//Unknown/unimplemented command
        send_packet(connection_file_descriptor, "");
    }

    return true;//Continue with this connection
}

static bool send_packet(int connection_file_descriptor, const packet_t& packet) {
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
    return nicesend(connection_file_descriptor, raw_message);
}

static packet_t recieve_packet(int connection_file_descriptor) {//An empty string means the client disconnected
    std::string raw_message = nicerecv(connection_file_descriptor);
    irvelog(0, "\x1b[95mGDB Says\x1b[0m:     \"\x1b[1m%s\x1b[0m\"", raw_message.c_str());

    if (raw_message.empty()) {//The client disconnected
        return special_packet_t::DISCONNECTED;
    }

    try {
        //TODO in the future actually validate the format/checksum of the packet
        raw_message = raw_message.substr(raw_message.find('$') + 1);//Remove the '$' at the beginning
        raw_message.erase(raw_message.size() - 3);//Remove the checksum and '#' at the end
        return raw_message;
    } catch (std::out_of_range& e) {//Not a regular packet
        if (raw_message == "+") {
            return special_packet_t::ACK;
        } else if (raw_message == "-") {
            return special_packet_t::NACK;
        } else {
            return special_packet_t::MALFORMED;
        }
    }
}

static std::string compute_checksum(const std::string& packet) {
    char accumulator = 0;

    for (size_t i = 0; i < packet.size(); i++) {
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

static bool nicesend(int connection_file_descriptor, const std::string& message) {
    return send(connection_file_descriptor, message.c_str(), message.size(), 0) != -1;//TODO handle the case where only part of the message was sent
}

static std::string nicerecv(int connection_file_descriptor) {//If the string is empty, this means the client disconnected
    char buffer[RECIEVE_BUFFER_SIZE + 1];
    //TODO handle the case where the message is larger than the buffer (we have to recieve, push to the string, and repeat)

    int recv_result = recv(connection_file_descriptor, buffer, RECIEVE_BUFFER_SIZE, 0);
    if ((!recv_result) || (recv_result == -1)) {
        //The client disconnected or an error occured
        return std::string();//An empty string means the client disconnected
    }

    buffer[recv_result] = '\0';//Null terminate the string

    return std::string(buffer);
}

static int setup_server_socket(uint16_t port) {
    //Get the linked list of addrinfo structs to attempt to setup a socket for
    struct addrinfo* addrinfo_ll_ptr = get_addrinfo_ll_ptr(port);

    //Loop through the list and setup a socket for the first one we can
    int attempt = 1;
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

#else//Not on Unix

/* Includes */

#include "emulator.h"
#include "gdbserver.h"

#define INST_COUNT 0
#include "logging.h"

/* Function Implementations */

void gdbserver::start(emulator::emulator_t& /*emulator*/, uint16_t /*port*/) {
    irvelog_always(0, "The IRVE GDB server is not supported on this platform");
}

#endif
