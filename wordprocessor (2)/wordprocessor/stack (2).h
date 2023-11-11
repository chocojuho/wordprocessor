#include <iostream>
#include <assert.h>
#include "msvcfix.h"

template <typename T>
class Stack {
private:
    struct Node {
        T data;
        Node* next;
    };
    Node* top;
public:
    Stack() : top(nullptr) {}
    ~Stack() {
        while (!isEmpty()) {
            pop();
        }
    }
    void push(const T& value) {
        Node* newNode = new Node;
        newNode->data = value;
        newNode->next = top;
        top = newNode;
    }
    void pop() {
        assert(!isEmpty());
        Node* temp = top;
        top = top->next;
        delete temp;
    }
    const T& peek() const {
        assert(!isEmpty());
        return top->data;
    }
    bool isEmpty() const {
        return top == nullptr;
    }
    T Top() {
        return top->data;
    }
};