#include <fstream>
#include <iostream>
#include "wdigraph.h"
#include "dijkstra.h"
#include "math.h"
#include "heap.h"
#include <algorithm>
#include <unordered_map>
using namespace std;

typedef pair<int, int> PII;
typedef pair<int, long long> PIL;

struct Point {
    long long lat; // latitude of the point
    long long lon; // longitude of the point
};

long long manhattan(const Point& pt1, const Point& pt2);
void readGraph(string filename, WDigraph& graph, 
    unordered_map<int, Point>& points);

// MAIN IS GETTING FULL - ADD AT LEAST ONE MORE FUNCTION
int main() {
    WDigraph graph;
    unordered_map<int, Point> points;
    unordered_map<int, PIL> tree;
    readGraph("edmonton-roads-2.0.1.txt", graph, points);
    //readGraph("test.txt", graph, points);
    // cout << graph.getCost(620818227, 251365277) << endl;
    /*for (auto elem : tree) {
        cout << elem.second.second << endl;
    } */

    string line;
    getline(cin, line);
    //size_t comma0 = line.find(" ");
    size_t space1 = line.find(" ", 2);
    size_t space2 = line.find(" ", space1 + 1);
    size_t space3 = line.find_last_of(" ");
    long long lat1 = stoll(line.substr(2, space1 - 2));
    long long lon1 = stoll(line.substr(space1 + 1, space2 - space1 - 1));
    long long lat2 = stoll(line.substr(space2 + 1, space3 - space2 - 1));
    long long lon2 = stoll(line.substr(space3 + 1));

    Point startPoint = {lat1, lon1};
    Point endPoint = {lat2, lon2};

    int startID, endID;
    int sMin = 2000000000;
    int eMin = 2000000000;
    for (auto elem : points) {
        if (elem.second.lat == startPoint.lat && elem.second.lon == startPoint.lon) {
            startID = elem.first;
            break;
        } else {
            int dist = manhattan(elem.second, startPoint);
            if (dist < sMin) {
                sMin = dist;
                startID = elem.first;
            }
        }
    }

    for (auto elem : points) {
        if (elem.second.lat == endPoint.lat && elem.second.lon == endPoint.lon) {
            endID = elem.first;
            break;
        } else {
            int dist = manhattan(elem.second, endPoint);
            if (dist < eMin) {
                eMin = dist;
                endID = elem.first;
            }
        }
    }
    cout << endID << endl;
    dijkstra(graph, startID, tree);
    vector <int> waypoints; 
    if (tree.find(endID) == tree.end()) {
        cout << "N 0" << endl;
        return 0;
    }

    int curr = endID;
    cout << "cost " << tree[curr].second << endl;
    while (curr != startID) {
        waypoints.push_back(curr);
        curr = tree[curr].first;
    }
    waypoints.push_back(startID);
    reverse(waypoints.begin(), waypoints.end());
    cout << "N " << waypoints.size() << endl;

    int count = 0;
    while (getline(cin, line)){
        if (line.empty()) {
            break;
        }
        if (count < waypoints.size()){
            cout << "W " << points.at(waypoints[count]).lat << " ";
            cout << points.at(waypoints[count]).lon << endl;
        } else {
            cout << "E" << endl;
            break;
        }
        count++;
    }

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
    long long y2 = pt2.lon;
    long long delta_x = 0;
    long long delta_y = 0;
    long long cost_sum = 0;
    delta_x = abs(x1 - x2);
    delta_y = abs(y1 - y2);
    cost_sum = delta_x + delta_y;
    return cost_sum;
}
