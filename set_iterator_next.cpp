#include <cassert>
#include <deque>
#include <iostream>

using namespace std;

struct Node {
    Node(int v, Node* p)
            : value(v)
            , parent(p)
    {}

    int value;
    Node* left = nullptr;
    Node* right = nullptr;
    Node* parent;
};


class NodeBuilder {
public:
    Node* CreateRoot(int value) {
        nodes.emplace_back(value, nullptr);
        return &nodes.back();
    }

    Node* CreateLeftSon(Node* me, int value) {
        assert( me->left == nullptr );
        nodes.emplace_back(value, me);
        me->left = &nodes.back();
        return me->left;
    }

    Node* CreateRightSon(Node* me, int value) {
        assert( me->right == nullptr );
        nodes.emplace_back(value, me);
        me->right = &nodes.back();
        return me->right;
    }

private:
    deque<Node> nodes;
};

Node* Next(Node* me) {
    if (me -> right){
        auto ptr = me -> right;
        while (ptr -> left){
            ptr = ptr ->left;
        }
        return ptr;
    }
    if (me -> parent && (me -> parent) -> left == me){
        return me -> parent;
    } else {
        while(me -> parent){
            if (me == (me -> parent) -> left){
                return me -> parent;
            }
            me = me -> parent;
        }
        return nullptr;
    }
}
