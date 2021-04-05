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

// NOTE, client takes the port and IP address in as input!!!

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
    // 2. Read coordinates of start and end points from inpipe
    // 3. Write to the socket
    // 4. Read coordinates of waypoints one at a time
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
        cerr << "Listening socket creation failed!\n";
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
        cerr << "Cannot connect to the host!\n";
        close(socket_desc);
        return 1;
    }
    cout << "Connection established with " << inet_ntoa(peer_addr.sin_addr) << ":" << ntohs(peer_addr.sin_port) << "\n";

    while (true) {
        // read in the coordinates of the start and end point
        int bytes_read;
        // ensure that outbound is empty
        outbound[0] = '\0';
        bytes_read = read(in, outbound, 22);

        if (strcmp("Q\n", outbound) == 0) {
            send(socket_desc, outbound, strlen(outbound) + 1, 0);
            break;
        }

        string pt1 = string(outbound);
        // empty outbound
        outbound[0] = '\0';
        bytes_read = read(in, outbound, 22);
        string pt2 = string(outbound);
        cout << pt1 << " " << pt2 << endl;

        // remove the newline character
        pt1 = pt1.substr(0,pt1.length()-1);
        pt2 = pt2.substr(0,pt2.length()-1);

        // parse the strings to do the necessary conversions
        size_t space = pt1.find(" ");
        auto lat1 = static_cast<long long>(stod(pt1.substr(0, space-1))*100000);
        auto lon1 = static_cast<long long>(stod(pt1.substr(space+1))*100000);

        space = pt2.find(" ");
        auto lat2 = static_cast<long long>(stod(pt2.substr(0, space-1))*100000);
        auto lon2 = static_cast<long long>(stod(pt2.substr(space+1))*100000);
        
        // add info back to outbound to write to the socket
        string ans = "R " + to_string(lat1) + " " + to_string(lon1);
        ans += + " " + to_string(lat2) + " " + to_string(lon2);
        strcpy(outbound, ans.c_str());
        send(socket_desc, outbound, strlen(outbound) + 1, 0);

        int write_bytes;
        int num;
        while (true) {
            // blocking call
            int rec_size = recv(socket_desc, inbound, BUFFER_SIZE, 0);
            cout << "Received: " << inbound << endl;
            if (strcmp(inbound, "N 0") == 0) {
                char array1[] = "E";
                strcpy(inbound,array1);
                num = 0;
                break;
            } else {
                string input = string(inbound);
                if (input.at(0) = 'N') {
                    num = stoi(input.substr(2));
                    // DO I HAVE TO MAKE SURE THE STRING IS A NUMBER???
                    break;
                } else {
                    // received input is invalid
                    // resend the request with the start and end point
                    // and loop until inbound is valid
                    send(socket_desc, outbound, strlen(outbound) + 1, 0);
                }
            }
        }
        
        // WRONG INPUT WILL CAUSE THIS WHOLE PROCESS TO RESTART??
        // I FEEL LIKE I NEED A FUNCTION TO IMPLEMENT THAT THEN???
        // send acknowledgement
        string points;
        while (num) {
            string s = "A";
            strcpy(outbound, s.c_str());
            send(socket_desc, outbound, strlen(outbound) + 1, 0);
            int rec_size = recv(socket_desc, inbound, BUFFER_SIZE, 0);
            string input = string(inbound);

            if (input.at(0) == 'W') {
                size_t space1 = input.find(" ", 2);
                // IS THIS A PROPER CONVERSION BACK??
                double pt_lat = (double)stoll(input.substr(2, space1 - 1));
                double pt_lon = (double)stoll(input.substr(space1 + 1));
                pt_lat /= 100000;
                pt_lon /= 100000;
                points += to_string(pt_lat) + " " + to_string(pt_lon) + "\n";
                num--;
            }

        }
        // write waypoints to plotter
        // WHY DO I NEED ANOTHER ACKNOWLEDGEMENT FOR "E" AT THE END
        // DON'T I ADD IT REGARDLESS??
        points += "E";
        strcpy(inbound, points.c_str());
        write_bytes = write(out, inbound, sizeof inbound);
    }

    // close socket
    close(socket_desc);

    // close pipes and then reclaim the backing files
    close(in);
    close(out);
    unlink(inpipe);
    unlink(outpipe);
    return 0;
}
