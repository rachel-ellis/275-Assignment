#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

#include <arpa/inet.h>      // inet_aton, htonl, htons
#include <cstring>          // strlen, strcmp
#include <sys/socket.h>     // socket, connect
using namespace std;

#define BUFFER_SIZE 1024
#define SERVER_PORT 50000
#define SERVER_IP "127.0.0.1"

int create_and_open_fifo(const char * pname, int mode) {
    // creating a fifo special file in the current working directory
    // with read-write permissions for communication with the plotter
    // both proecsses must open the fifo before they can perform
    // read and write operations on it
    if (mkfifo(pname, 0666) == -1) {
        cout << "Unable to make a fifo. Ensure that this pipe does not exist already!" << endl;
        exit(-1);
    }

    // opening the fifo for read-only or write-only access
    // a file descriptor that refers to the open file description is
    // returned
    int fd = open(pname, mode);

    if (fd == -1) {
        cout << "Error: failed on opening named pipe." << endl;
        exit(-1);
    }

    return fd;
}

int main(int argc, char const *argv[]) {
    const char *inpipe = "inpipe";
    const char *outpipe = "outpipe";

    int in = create_and_open_fifo(inpipe, O_RDONLY);
    cout << "inpipe opened..." << endl;
    int out = create_and_open_fifo(outpipe, O_WRONLY);
    cout << "outpipe opened..." << endl;

    // Here is what you need to do:
    // 1. Establish a connection with the server
    // 2. Read coordinates of start and end points from inpipe (blocks until they are selected)
    //    If 'Q' is read instead of the coordinates then go to Step 7
    // 3. Write to the socket
    // 4. Read coordinates of waypoints one at a time (blocks until server writes them)
    // 5. Write these coordinates to outpipe

    // Declare structure variables that store local and peer socket addresses
    // sockaddr_in is the address sturcture used for IPv4 
    // sockaddr is the protocol independent address structure
    struct sockaddr_in my_addr, peer_addr;

    // zero out the structor variable because it has an unused part
    memset(&my_addr, '\0', sizeof my_addr);

    // Declare socket descriptor
    int socket_desc;

    char outbound[BUFFER_SIZE] = {};
    char inbound[BUFFER_SIZE] = {};

    /*
        socket() input arguments are:
        socket domain (AF_INET):    IPv4 Internet protocols
        socket type (SOCK_STREAM):  sequenced, reliable, two-way, connection-based
                                    byte streams
        socket protocol (0):        OS selects a protocol that supports the requested
                                    socket type (in this case: IPPROTO_TCP)
        socket() returns a socket descriptor
    */
    socket_desc = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_desc == -1) {
        std::cerr << "Listening socket creation failed!\n";
        return 1;
    }

    // Prepare sockaddr_in structure variable
    peer_addr.sin_family = AF_INET;  // address family (2 bytes)
    peer_addr.sin_port = htons(SERVER_PORT);   
    // port in network byte order (2 bytes)
    // htons takes care of host-order to short network-order conversion

    peer_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);     
    // internet address (4 bytes). INADDR_LOOPBACK is localhost address

    // connecting to the server socket
    if (connect(socket_desc, (struct sockaddr *) &peer_addr, sizeof peer_addr) == -1) {
        std::cerr << "Cannot connect to the host!\n";
        close(socket_desc);
        return 1;
    }
    std::cout << "Connection established with " << inet_ntoa(peer_addr.sin_addr) << ":" << ntohs(peer_addr.sin_port) << "\n";

    // I THINK THIS IS THE ONLY PART WE HAVE TO CHANGE
    // 1. IT'S NOT STD INPUT
    // 2. FIX MEANING OF INPUT (NOT JUST ONE INT)
    // 3. WRITE THE COORDINATES TO OUTPIPE
    while (true) {
        std::cout << "Enter an integer or \"Q\" to quit:\n";
        std::cin.getline(outbound, BUFFER_SIZE);

        send(socket_desc, outbound, strlen(outbound) + 1, 0);
        if (strcmp("Q", outbound) == 0) {
            break;
        }

        // blocking call
        int rec_size = recv(socket_desc, inbound, BUFFER_SIZE, 0);
        std::cout << "Received: " << inbound << std::endl;
    }

    // close socket
    close(socket_desc);

    close(in);
    close(out);
    unlink(inpipe);
    unlink(outpipe);
    return 0;
}
