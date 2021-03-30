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

  /*
  Description: Return the minimum element in the heap

  Arguments: 
    (void)

  Returns:
    (HeapItem<T,K>): root node in the heap
  */
  HeapItem<T, K> min() const {
   return heap[0];
  }

  /*
  Description: Insert an item with the given key into the heap 
               (regardless of whether that item is already in it or not)
  
  Arguments: 
    item (const T&): item of the element to be added to the heap
    key (const K&):  key of the element to be added to the heap

  Returns:
    (void)
  */
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
 
  /*
  Description: Pop the minimum element from the heap
  
  Arguments: 
    (void)

  Returns:
    (void)
  */
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

  /*
  Description: Returns the size of the heap
  
  Arguments: 
    (void)

  Returns:
    (int): size of the heap (number of elements in the heap) 
  */
  int size() const {
    return heap.size();
  }

private:
  // the array holding the heap
  std::vector< HeapItem<T, K> > heap;

  /*
  Description: Fix the heap property at index i and recurse with its parent 
               (if the parent node exists)
  
  Arguments: 
    i (int): Index of child node in heap to fix the heap property at

  Returns:
    (void)
  */
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
 
  /*
  Description:  Fix the heap property at index i and recurse with the child
                that received i's item (if appropriate)
  
  Arguments: 
    i (int): Index of parent node in heap to fix the heap from

  Returns:
    (void)
  */
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

  /*
  Description: Helper function to find the minimum child node

  Arguments: 
    k (int): Index of left child node in heap

  Returns:
    (int): Index of child with minimum key value in the heap
  */
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
