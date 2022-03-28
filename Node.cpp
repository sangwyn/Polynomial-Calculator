#include "Node.h"
#include "Term.h"

template<typename T>
Node<T>::Node() {
    next = nullptr;
}

template<typename T>
Node<T>::Node(T data) {
    this->data = data;
    next = nullptr;
}

template<typename T>
void Node<T>::InsAfter(Node<T> *&q) {
    q->next = this->next;
    this->next = q;
}

template<typename T>
void Node<T>::InsBefore(Node<T> *&q) {
    T tmp = q->data;
    q->data = this->data;
    this->data = tmp;
    q->next = this->next;
    this->next = q;
}

template class Node<Term *>;
template class Node<Node<Term *> *>;