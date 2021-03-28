#include <unordered_map>
#include "wdigraph.h"
#include "dijkstra.h"
#include "heap.h"

using namespace std;
typedef pair<int, int> PII; // aliases or symbolic name of an existing type 
typedef pair<int, long long> PIL;

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
       // change the name 

       // relabelling the variables so we can access the elements 
       int first_point = point.item.first;
       int sec_point = point.item.second;
       int dist = point.key;

       // removing the min edge from the heap 
       StoringPoints.popMin(); 
       
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