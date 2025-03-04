#pragma once

#include <atomic>
#include <optional>

class LockFreeQueue {
public:
    LockFreeQueue() {
        Node* dummy = new Node;
        head = GenNode{dummy, 1};
        tail = dummy;
        stackTop = GenNode{new Node, 1};
    }

    std::optional<int> tryPop() {
        GenNode headPtr = head.load();
        while (true) {
            Node* newHead = headPtr.ptr->next.load();
            if (newHead == nullptr) {
                return std::nullopt;
            }

            GenNode newHeadPtr{newHead, headPtr.counter + 1};
            if (head.compare_exchange_weak(headPtr, newHeadPtr)) {
                break;
            }
        }
        int res{headPtr.ptr->data};
        return res;
    }

    // 1. Need to create new dummy node
    // 2. Exchange new and old dummy node
    // 3. Write the data into the old dummy node
    void push(int val) {
        Node* newDummy = new Node{};
        Node* oldDummy = tail.exchange(newDummy);
        oldDummy->data = val;
        oldDummy->next.store(newDummy);
    }

    ~LockFreeQueue() {
        GenNode headPtr = head.load();
        Node* cur = headPtr.ptr;
        while (cur) {
            Node* nxt = cur->next;
            delete cur;
            cur = nxt;
        }

        GenNode stackPtr = stackTop.load();
        cur = stackPtr.ptr;
        while (cur) {
            Node* nxt = cur->next;
            delete cur;
            cur = nxt;
        }
    }

private:
    struct Node;
    Node* getNewNode() {
        GenNode oldTop = stackTop.load();
        while (true) {
            Node* ptr = oldTop.ptr;
            if (ptr == nullptr) {
                return new Node{};
            }

            Node* next = ptr->next.load();
            GenNode newTop{next, oldTop.counter + 1};
            if (stackTop.compare_exchange_weak(oldTop, newTop)) {
                break;
            }
        }

        return oldTop.ptr;
    }

    void recycleNode(Node* val) {
        GenNode oldTop = stackTop.load();
        while (true) {
            val->next.store(oldTop.ptr);
            GenNode newTop{val, oldTop.counter + 1};
            if (stackTop.compare_exchange_weak(oldTop, newTop)) {
                break;
            }
        }
    }
     
    struct Node {
        // Needs to be atomic as there could be 
        // a read from tryPop while the push is
        // writing the data into the node.
        std::atomic<Node*> next;
        int data;
    };
    
    struct GenNode {
        Node* ptr;
        int counter;
    };

    std::atomic<GenNode> head;
    std::atomic<Node*> tail;
    std::atomic<GenNode> stackTop;
};
