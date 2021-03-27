#include <fstream>
#include <iostream>
#include "wdigraph.h"
#include "math.h"
#include "heap.h"
#include <unordered_map>
using namespace std;

typedef pair<int, int> PII; // aliases or symbolic name of an existing type 
typedef pair<int, long long> PIL;

struct Point {
    long long lat; // latitude of the point
    long long lon; // longitude of the point
};

long long manhattan(const Point& pt1, const Point& pt2);
void readGraph(string filename, WDigraph& graph, 
    unordered_map<int, Point>& points);

void dijkstra(const WDigraph& graph, int startVertex, unordered_map<int, PIL>& tree);

int main() {
    WDigraph graph;
    unordered_map<int, Point> points;
    unordered_map<int, PIL> tree;
    readGraph("edmonton-roads-2.0.1.txt", graph, points);
    // cout << graph.getCost(654589385, 354272311) << endl;
    dijkstra(graph, 29770958, tree);
    /*for (auto elem : tree) {
        cout << elem.second.second << endl;
    } */
    return 0;
}


void readGraph(string filename, WDigraph& graph, 
    unordered_map<int, Point>& points) {
    string line;
    ifstream infile(filename);
    while (getline(infile, line)) {
        // break loop if input line is empty, this allows us to get an unknown
        // amount of input and make sure the loop terminates
        if (line.empty()) {
            break;
        }

        // split string by location of the commas to get the useful info
        string task, id;
        task = line.substr(0, 1);
        size_t comma0 = line.find(",");
        size_t comma1 = line.find(",", comma0 + 1);
        size_t comma2 = line.find_last_of(",");
        id = line.substr(comma0 + 1, comma1 - comma0 - 1);

        // add vertex or edge depending on the task entered
        if (task == "V") {
            graph.addVertex(stoi(id));
            double coord1 = stod(line.substr(comma1 + 1, comma2 - comma1 - 1));
            double coord2 = stod(line.substr(comma2 + 1));
            Point pt1{};
            pt1.lat = static_cast < long long > (coord1 * 100000);
            pt1.lon = static_cast < long long > (coord2 * 100000);
            points[stoi(id)] = pt1; 

        } else if (task == "E") {
            string id2 = line.substr(comma1 + 1, comma2 - comma1 - 1);
            // add edge going both ways to create an undirected graph
            Point pt1 = points.at(stoi(id));
            Point pt2 = points.at(stoi(id2));
            long long cost = manhattan(pt1, pt2);
            graph.addEdge(stoi(id), stoi(id2), cost);
            graph.addEdge(stoi(id2), stoi(id), cost);
        }
    }
}

long long manhattan(const Point& pt1, const Point& pt2) {
    // Return the Manhattan distance between the two given points
    // two point structure objects of are being passed in called 
    long long x1 = pt1.lat;
    long long x2 = pt2.lat;
    long long y1 = pt1.lon;
    long long y2 = pt1.lon;
    long long delta_x = 0;
    long long delta_y = 0;
    long long cost_sum = 0;
    delta_x = abs(x1 - x2);
    delta_y = abs(y1 - y2);
    cost_sum = delta_x + delta_y;
    return cost_sum;
}
/*Arguemnts: const refrence */

void dijkstra(const WDigraph& graph, int startVertex, unordered_map<int, PIL>& tree) {
    //creating a binary heap to store the points being explored
    BinaryHeap <PII, long long> StoringPoints;
    
    
    // no "predecessor" of the startVertex 
    /* the long long is the cost, so the key sessentially is 
    the predeccsoor and the cost while the item is the vertex*/
    //putting in the root node
    StoringPoints.insert(PII(startVertex, -1), 0);
    
    while (StoringPoints.size() != 0) {
        
        // returning the min item
       HeapItem <PII, long long> point = StoringPoints.min();

       // removing the min edge from the heap 
       StoringPoints.popMin(); 
       // relabelling the variables so we can access the elements 
       int first_point = point.item.first;
       int sec_point = point.item.second;
       int dist = point.key;
       // checking if the node is not reached then continue;
        if (tree.find(first_point) != tree.end()) {
            continue;
        }

        // declare that v that is reached at distance "dist" 
        // appnding the current node to the reached unordered map
        // passing in a pair of parent to first_point which is 
        //sec_point  and also assigning a cost which is d
        tree[first_point] = PII(sec_point, dist); 

        // looking at the neighbouring points of the nearest waypoint and adding them to the heap
        for (auto iter = graph.neighbours(first_point); iter != graph.endIterator(first_point); iter++) {
            int nbr = *iter; 
            long long waypoints_cost = dist + graph.getCost(first_point, nbr); //current time is d and then we take the cost of the edge and add that 
            StoringPoints.insert(PII(nbr,first_point), waypoints_cost); // pssing in the neghobur, v is the beginning of the edge and nbr is the end of the edge 
            // and we are also passing in the cost 
        }
    }
}