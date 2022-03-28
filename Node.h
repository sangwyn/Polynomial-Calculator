#ifndef MNOGOCHLEN__NODE_H_
#define MNOGOCHLEN__NODE_H_

template<typename T>

class Node {
 public:
  T data;
  Node<T> *next;

  Node();
  Node(T data);
  void InsAfter(Node<T> *&q);
  void InsBefore(Node<T> *&q);
};

#endif //MNOGOCHLEN__NODE_H_
