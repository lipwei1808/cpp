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
        // Synchronises with tryPop (head.compare_exchange_weak(...))
        GenNode headPtr = head.load(std::memory_order::acquire);
        while (true) {
            // Synchronises with push (oldDummy->next.store)
            Node* newHead = headPtr.ptr->next.load(std::memory_order::acquire);
            if (newHead == nullptr) {
                return std::nullopt;
            }

            GenNode newHeadPtr{newHead, headPtr.counter + 1};

            // Synchronises with tryPop (head.load(...))
            // Needs to release the memory else there could
            // be a race between headPtr.ptr (read) and
            // a write in getNewNode(...)
            if (head.compare_exchange_weak(headPtr, newHeadPtr,
                                           std::memory_order::acq_rel)) {
                break;
            }
        }
        int res{headPtr.ptr->data};
        recycleNode(headPtr.ptr);
        return res;
    }

    // 1. Need to create new dummy node
    // 2. Exchange new and old dummy node
    // 3. Write the data into the old dummy node
    void push(int val) {
        Node* newDummy = getNewNode();
        newDummy->next.store(nullptr);
        Node* oldDummy = tail.exchange(newDummy);
        oldDummy->data = val;
        // Synchronises with tryPop (headPtr.ptr->next.load)
        oldDummy->next.store(newDummy, std::memory_order::release);
    }

    ~LockFreeQueue() {
        GenNode headPtr = head.load(std::memory_order::relaxed);
        Node* cur = headPtr.ptr;
        while (cur) {
            Node* nxt = cur->next;
            delete cur;
            cur = nxt;
        }

        GenNode stackPtr = stackTop.load(std::memory_order::relaxed);
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
        GenNode oldTop = stackTop.load(std::memory_order::relaxed);
        while (true) {
            Node* ptr = oldTop.ptr;
            if (ptr == nullptr) {
                return new Node{};
            }

            // Synchronises with recycleNode (val->next.store(...))
            Node* next = ptr->next.load(std::memory_order::acquire);
            GenNode newTop{next, oldTop.counter + 1};
            if (stackTop.compare_exchange_weak(oldTop, newTop,
                                               std::memory_order::relaxed)) {
                break;
            }
        }

        return oldTop.ptr;
    }

    void recycleNode(Node* val) {
        GenNode oldTop = stackTop.load(std::memory_order::relaxed);
        while (true) {
            // Synchronises with getNewNode (ptr->next.load)
            // Needs to release the current view of memory for
            // the load if not subsequent uses of that node in
            // getNewNode could cause a data race when someone 
            // wishes to read the node value previously
            val->next.store(oldTop.ptr, std::memory_order::release);
            GenNode newTop{val, oldTop.counter + 1};
            if (stackTop.compare_exchange_weak(oldTop, newTop,
                                               std::memory_order::relaxed)) {
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
