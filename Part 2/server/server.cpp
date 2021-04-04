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

#define PORT 50000
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
  // and the route information is written to a socket

  // declare structure variables that store local and peer socket addresses
  // sockaddr_in is the address sturcture used for IPv4 
  // sockaddr is the protocol independent address structure
  struct sockaddr_in my_addr, peer_addr;

  // zero out the structor variable because it has an unused part
  memset(&my_addr, '\0', sizeof my_addr);

  // declare variables for socket descriptors 
  int lstn_socket_desc, conn_socket_desc;

  char buffer[BUFFER_SIZE] = {};

  lstn_socket_desc = socket(AF_INET, SOCK_STREAM, 0);
  if (lstn_socket_desc == -1) {
    std::cerr << "Listening socket creation failed!\n";
    return 1;
  }

  // prepare sockaddr_in structure variable
  my_addr.sin_family = AF_INET;       // address family (2 bytes)
  my_addr.sin_port = htons(PORT);       // port in network byte order (2 bytes)
                        // htons takes care of host-order to short network-order conversion.
  my_addr.sin_addr.s_addr = htonl(INADDR_ANY);// internet address (4 bytes) INADDR_ANY is all local interfaces
                        // htons takes care of host-order to long network-order conversion.

  // note bind takes in a protocol independent address structure
  // hence we need to cast sockaddr_in* to sockaddr*
  if (bind(lstn_socket_desc, (struct sockaddr *) &my_addr, sizeof my_addr) == -1) {
      std::cerr << "Binding failed!\n";
      close(lstn_socket_desc);
      return 1;
  }
  std::cout << "Binding was successful\n";


    if (listen(lstn_socket_desc, LISTEN_BACKLOG) == -1) {
      std::cerr << "Cannot listen to the specified socket!\n";
        close(lstn_socket_desc);
        return 1;
    }

  socklen_t peer_addr_size = sizeof my_addr;

  // comment the outer while loop to make the server 
  // terminate after accepting one connection request
  while (true) {
    // extract the first connection request from the queue of pending connection requests
    // return a new connection socket descriptor which is not in the listening state
    conn_socket_desc = accept(lstn_socket_desc, (struct sockaddr *) &peer_addr, &peer_addr_size);
    if (conn_socket_desc == -1){
      std::cerr << "Connection socket creation failed!\n";
      // continue;
      return 1;
    }
    std::cout << "Connection request accepted from " << inet_ntoa(peer_addr.sin_addr) << ":" << ntohs(peer_addr.sin_port) << "\n";

    // declare structure variable that represents an elapsed time 
    // it stores the number of whole seconds and the number of microseconds
    struct timeval timer = {.tv_sec = 1, .tv_usec = 10000};

    while (true) {
      // blocking call - blocks until a message arrives 
      // (unless O_NONBLOCK is set on the socket's file descriptor)
      int rec_size = recv(conn_socket_desc, echobuffer, BUFFER_SIZE, 0);
      if (rec_size == -1) {
        std::cout << "Timeout occurred... still waiting!\n";
        continue;
      }
      std::cout << "Message received\n";
      if (strcmp("Q", echobuffer) == 0) {
        std::cout << "Connection will be closed\n";
        break;
      }

      // convert a c-style string to an integer
      int num = atoi(echobuffer);

      // convert the integer to a string
      std::string str = std::to_string(-num);
      send(conn_socket_desc, str.c_str(), str.length() + 1, 0);
      std::cout << "Computation result sent back\n";
    }
  }

  //--------------------- OLD STUFF BELOW-----------------------
  // read a request
  char c;
  Point sPoint, ePoint;
  cin >> c >> sPoint.lat >> sPoint.lon >> ePoint.lat >> ePoint.lon;

  // NEED TO check that c is R

  // get the points closest to the two points we read
  int start = findClosest(sPoint, points), end = findClosest(ePoint, points);

  // run dijkstra's, this is the unoptimized version that does not stop
  // when the end is reached but it is still fast enough
  unordered_map<int, PIL> tree;
  dijkstra(graph, start, tree);

  // no path
  if (tree.find(end) == tree.end()) {
      cout << "N 0" << endl;
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
    cout << "N " << path.size() << endl;
    for (int v : path) {
      cout << "W " << points[v].lat << ' ' << points[v].lon << endl;
    }
    cout << "E" << endl;
  }


  // close socket descriptors
  close(lstn_socket_desc);
  close(conn_socket_desc);
  return 0;
}
