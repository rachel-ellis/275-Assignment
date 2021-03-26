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
  HeapItem<T, K> min() const;

  // insert an item with the given key
  // if the item is already in the heap, will still insert a new copy with this key
  void insert(const T& item, const K& key);

  // pop the minimum item from the heap
  void popMin();

  // returns the number of items held in the heap
  int size() const;

private:
  // the array holding the heap
  std::vector< HeapItem<T, K> > heap;

  //  will fix the heap property at index i and recurse with its parent
  void fixHeapUp(int i);

  // will fix the heap property at index i and recurse with the child
  // that received i's item (if appropriate)
  void fixHeapDown(int i);
};