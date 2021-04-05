#include <iostream>
#include <cassert>
#include <fstream>
#include <sstream>
#include <string>
#include <string.h>
#include <list>
#include <unistd.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include "wdigraph.h"
#include "dijkstra.h"
#define PORT 8888
#define LISTEN_BACKLOG 50
#define BUFFER_SIZE 1024
struct Point {
	long long lat, lon;
};

// returns the manhattan distance between two points
long long manhattan(const Point& pt1, const Point& pt2) {
	long long dLat = pt1.lat - pt2.lat, dLon = pt1.lon - pt2.lon;
	return abs(dLat) + abs(dLon);
}

// finds the point that is closest to a given point, pt
int findClosest(const Point& pt, const unordered_map<int, Point>& points) {
	pair<int, Point> best = *points.begin();

	for (const auto& check : points) {
		if (manhattan(pt, check.second) < manhattan(pt, best.second)) {
			best = check;
		}
	}
	return best.first;
}

// reads graph description from the input file and builts a graph instance
void readGraph(const string& filename, WDigraph& g, unordered_map<int, Point>& points) {
	ifstream fin(filename);
	string line;

	while (getline(fin, line)) {
    // split the string around the commas, there will be 4 substrings either way
		string p[4];
		int at = 0;
		for (auto c : line) {
			if (c == ',') {
        // starting a new string
				++at;
			}
			else {
        // appending a character to the string we are building
				p[at] += c;
			}
		}

		if (at != 3) {
      // empty line
			break;
		}

		if (p[0] == "V") {
      // adding a new vertex
			int id = stoi(p[1]);
      assert(id == stoll(p[1])); // sanity check: asserts if some id is not 32-bit
      points[id].lat = static_cast<long long>(stod(p[2])*100000);
      points[id].lon = static_cast<long long>(stod(p[3])*100000);
      g.addVertex(id);
  }
  else {
      // adding a new directed edge
  	int u = stoi(p[1]), v = stoi(p[2]);
  	g.addEdge(u, v, manhattan(points[u], points[v]));
  }
}
}

// Keep in mind that in Part I, your program must handle 1 request
// but in Part 2 you must serve the next request as soon as you are
// done handling the previous one

int main(int argc, char* argv[]) {
	WDigraph graph;
	unordered_map<int, Point> points;

  // build the graph
	readGraph("edmonton-roads-2.0.1.txt", graph, points);

  // In Part 2, client and server communicate using a pair of sockets
  // modify the part below so that the route request is read from a socket
  // (instead of stdin) and the route information is written to a socket
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

    // I THINK THIS IS THE ONLY PART WE HAVE TO CHANGE
    // 4. WRITE THE WAYPOINTS TO OUTPIPE
    while (true) {
        // read in the coordinates of the start and end point
    	int bytes_read;
    	memset(outbound, 0, sizeof(outbound));
    	bytes_read = recv(conn_socket_desc, echobuffer, BUFFER_SIZE, 0);
    	string pt1 = string(outbound);
    	memset(outbound, 0, sizeof(outbound));
    	bytes_read = recv(conn_socket_desc, echobuffer, BUFFER_SIZE, 0);
    	string pt2 = string(outbound);

        // remove the newline character
    	pt1 = pt1.substr(0,pt1.length()-1);
    	pt2 = pt2.substr(0,pt2.length()-1);

        // parse the strings to do the necessary conversions
    	auto space = pt1.find(" ");
    	if(pt1.find("R")){
    		auto lat1 = static_cast<long long>(stod(pt1.substr(0, space-1))*100000);
    		auto lon1 = static_cast<long long>(stod(pt1.substr(space+1))*100000);
    	

    	space = pt2.find(" ");
    	auto lat2 = static_cast<long long>(stod(pt2.substr(0, space-1))*100000);
    	auto lon2 = static_cast<long long>(stod(pt2.substr(space+1))*100000);
    	}

	char c;
	Point sPoint, ePoint;
	sPoint.lat = lat1;
	sPoint.lon = lon1;
	ePoint.lat = lat2;
	ePoint.lon = lon2;
  //cin >> c >> sPoint.lat >> sPoint.lon >> ePoint.lat >> ePoint.lon; // we are not reading from the standard in anymore we need to read from socket 

  // c is guaranteed to be 'R' in part 1, no need to error check until part 2 // error checking

  // get the points closest to the two points we read
  int start = findClosest(sPoint, points), end = findClosest(ePoint, points);

  // run dijkstra's, this is the unoptimized version that does not stop
  // when the end is reached but it is still fast enough
  unordered_map<int, PIL> tree;
  dijkstra(graph, start, tree);

  // no path
  if (tree.find(end) == tree.end()) {
      //cout << "N 0" << endl; // writting to aanother socket
  	 string no_path = "N " + " " + "0" ;
    	strcpy(outbound, no_path.c_str());
  }
  else {
    // read off the path by stepping back through the search tree
  	list<int> path;
  	while (end != start) {
  		path.push_front(end);
  		end = tree[end].first;
  	}
  	path.push_front(start);

    // output the path
    //cout << "N " << path.size() << endl;// writting to a socker
    string path_len = "N " + path.size() ;
    	strcpy(outbound, path_len.c_str());

    	send(socket_desc, outbound, strlen(outbound) + 1, 0);
    for (int v : path) {
    	cout << "W " << points[v].lat << ' ' << points[v].lon << endl;
    	//    // add info back to outbound to write to the socket
    	string waypoints = "W " + points[v].lat + " " + points[v].lon;
    	strcpy(outbound, waypoints.c_str());

    	send(socket_desc, outbound, strlen(outbound) + 1, 0);
    	// declaring the condition for receiving the Ackowlwdgement package

    	//if{
    	//continue;
    //} 
    	// else{
    	// 	break; 
    	// }
    	
    }
    string ending_out = "E " 
    	strcpy(outbound, ending_out.c_str());

    	send(socket_desc, outbound, strlen(outbound) + 1, 0);
}
    // close socket
    close(socket_desc);
    close(lstn_socket_desc);
    close(conn_socket_desc);


return 0;
}


     

  



