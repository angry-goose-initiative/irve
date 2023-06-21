/* gdbserver.cpp
 * Copyright (C) 2023 John Jekel and Nick Chan
 * See the LICENSE file at the root of the project for licensing info.
 *
 * Minimal GDB server implementation to ease debugging
 *
 * Thanks https://beej.us/guide/bgnet !!!
 *
*/

#ifdef __unix__

/* Constants And Defines */

//TODO

/* Includes */

#include "emulator.h"
#include "gdbserver.h"

#define INST_COUNT 0
#include "logging.h"

#include <cassert>
#include <cstring>
#include <string>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

using namespace irve::internal;

/* Types */

//TODO

/* Variables */

//TODO

/* Static Function Declarations */

static int setup_socket(uint16_t port);
static struct addrinfo* get_addrinfo_ll_ptr(uint16_t port);

/* Function Implementations */

void gdbserver::start(emulator::emulator_t& /*emulator*/, uint16_t port) {
    /*int socket_file_descriptor = */setup_socket(port);

    //TODO accept connections
    assert(false && "TODO");

    irvelog_always(0, "Alrighty, you can connect to port %d now! (Waiting for connections...)", port);
}

/* Static Function Implementations */

static int setup_socket(uint16_t port) {
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
        irvelog(0, "Opened a socket!");

        //If the port was already in use, try to reuse the address to avoid errors
        int we_indeed_want_to_reuse_the_address = 1;
        int setsockopt_result = setsockopt(socket_file_descriptor, SOL_SOCKET, SO_REUSEADDR, &we_indeed_want_to_reuse_the_address, sizeof(int));
        if (setsockopt_result == -1) {
            irvelog(0, "Failed to set the SO_REUSEADDR socket option for addrinfo struct #%d, trying the next one...", attempt);
            close(socket_file_descriptor);
            ++attempt;
            continue;
        }

        //Bind the socket to the IP address in the addrinfo struct
        int bind_result = bind(socket_file_descriptor, addrinfo_node_ptr->ai_addr, addrinfo_node_ptr->ai_addrlen);
        if (bind_result == -1) {
            irvelog(0, "Failed to bind to the socket for addrinfo struct #%d, trying the next one...", attempt);
            close(socket_file_descriptor);
            ++attempt;
            continue;
        } else {
            irvelog(0, "Bound to a socket succesfully, we can move on! :)");
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
