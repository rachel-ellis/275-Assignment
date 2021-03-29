// -----------------------------------
//  Name: Rachel Ellis and Anushka Khare
//  ID: 1618966 and 1617774
//  CMPUT 275
//
//  Assignment Part 1: Trivial Navigation System
// ------------------------------------

#include <vector>

// Only assumes the key type K is totally ordered and comparable via <
template <class T, class K>
struct HeapItem {
  T item;
  K key;
};

template <class T, class K>
class BinaryHeap {
public:
  // constructor not required because the only "initialization"
  // is through the constructor of the variable "heap" which is called by default

  // return the minimum element in the heap
  HeapItem<T, K> min() const {
   return heap[0];
  }

  // insert an item with the given key
  // if the item is already in the heap, will still insert a new copy with this key
  void insert(const T& item, const K& key){
    HeapItem<T, K> next;
    next.item = item;
    next.key = key;
    // insert element by adding the item and key to heap vector
    heap.push_back(next);
    int end = size() - 1;
    // need to fix as the heap property may be violated from the end up
    fixHeapUp(end);
  }

  // pop the minimum item from the heap
  void popMin() {
    // swap the element at the last vertex of the heap and the root
    HeapItem<T, K> temp;
    temp = heap[0];
    int end = size() - 1;
    heap[0] = heap[end];
    heap[end] = temp;
    // pop the last element in the heap
    heap.pop_back();
    // need to fix as the heap property may be violated from the root node down
    fixHeapDown(0); 
  }

  // returns the number of items held in the heap
  int size() const {
    return heap.size();
  }

private:
  // the array holding the heap
  std::vector< HeapItem<T, K> > heap;

  // will fix the heap property at index i and recurse with its parent
  void fixHeapUp(int i) {
    // check if child is less than parent
    while (heap[i].key < heap[(i-1)/2].key) {  
      // swap child and parent
      HeapItem<T, K> temp = heap[(i-1)/2];
      heap[(i-1)/2] = heap[i];
      heap[i] = temp;
      // if the current node has a parent, call fixHeapUp on the parent
      if ((i-1)/2 > 0) {
        fixHeapUp((i-1)/2);
      } else {
        break;
      }
    }
  }

  // will fix the heap property at index i and recurse with the child
  // that received i's item (if appropriate)
  void fixHeapDown(int i) {
    // while node at i has at least a left child node
    while (i*2+1 < size()) {
      int index = minChild(i*2+1);
      // check if parent key is greater than min child key
      if (heap[i].key > heap[index].key) {
        // swap child and parent
        HeapItem<T, K> temp = heap[index];
        heap[index] = heap[i];
        heap[i] = temp;
      } 
      // repeat this fixing process on the child
      i = index;
    }
  }

  int minChild(int k){
    // check if the right child exists
    // if so, determine which child's ket is smaller and return that index
    if (k+1 < size()) {
      if (heap[k].key > heap[k+1].key) {
        return k+1;
      } 
    }
    return k;
  }

};
