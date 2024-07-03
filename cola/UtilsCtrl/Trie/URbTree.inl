/**
 * @FilePath     : /cola/cola/UtilsCtrl/Trie/URbtree.cpp
 * @Description  :
 * @Author       : naonao
 * @Date         : 2024-07-03 13:01:00
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-07-03 13:33:14
 **/
#ifndef NAO_URBTREE_INL
#define NAO_URBTREE_INL
#include "URbTree.h"

NAO_NAMESPACE_BEGIN

template<class Value, class Compare>
URbTree<Value, Compare>::RbNode::RbNode() noexcept
    : rbLeft(nullptr)
    , rbRight(nullptr)
    , rbParent(nullptr)
    , rbTree(nullptr)
    , rbColor(RED)
{
}

template<class Value, class Compare>
void URbTree<Value, Compare>::NodeType::erase_from_parent()
{
    static_assert(std::is_base_of_v<URbTree<Value, Compare>::NodeType, Value>, "Value type must be derived from RbTree<Value>::NodeType");
    if (this->rbTree) {
        this->rbTree->doErase(this);
        this->rbTree = nullptr;
    }
}

template<class Value, class Compare>
bool URbTree<Value, Compare>::compare(RbNode* left, RbNode* right) const noexcept
{
    return static_cast<Compare const&>(*this)(static_cast<Value&>(*left), static_cast<Value&>(*right));
}

template<class Value, class Compare>
void URbTree<Value, Compare>::rotateLeft(RbNode* node) noexcept
{
    RbNode* rightChild = node->rbRight;
    node->rbRight      = rightChild->rbLeft;
    if (rightChild->rbLeft != nullptr) {
        rightChild->rbLeft->rbParent = node;
    }
    rightChild->rbParent = node->rbParent;
    if (node->rbParent == nullptr) {
        root = rightChild;
    }
    else if (node == node->rbParent->rbLeft) {
        node->rbParent->rbLeft = rightChild;
    }
    else {
        node->rbParent->rbRight = rightChild;
    }
    rightChild->rbLeft = node;
    node->rbParent     = rightChild;
}

template<class Value, class Compare>
void URbTree<Value, Compare>::rotateRight(RbNode* node) noexcept
{
    RbNode* leftChild = node->rbLeft;
    node->rbLeft      = leftChild->rbRight;
    if (leftChild->rbRight != nullptr) {
        leftChild->rbRight->rbParent = node;
    }
    leftChild->rbParent = node->rbParent;
    if (node->rbParent == nullptr) {
        root = leftChild;
    }
    else if (node == node->rbParent->rbRight) {
        node->rbParent->rbRight = leftChild;
    }
    else {
        node->rbParent->rbLeft = leftChild;
    }
    leftChild->rbRight = node;
    node->rbParent     = leftChild;
}

template<class Value, class Compare>
void URbTree<Value, Compare>::fixViolation(RbNode* node) noexcept
{
    RbNode* parent      = nullptr;
    RbNode* grandParent = nullptr;
    while (node != root && node->rbColor != BLACK && node->rbParent->rbColor == RED) {
        parent      = node->rbParent;
        grandParent = parent->rbParent;
        if (parent == grandParent->rbLeft) {
            RbNode* uncle = grandParent->rbRight;
            if (uncle != nullptr && uncle->rbColor == RED) {
                grandParent->rbColor = RED;
                parent->rbColor      = BLACK;
                uncle->rbColor       = BLACK;
                node                 = grandParent;
            }
            else {
                if (node == parent->rbRight) {
                    rotateLeft(parent);
                    node   = parent;
                    parent = node->rbParent;
                }
                rotateRight(grandParent);
                std::swap(parent->rbColor, grandParent->rbColor);
                node = parent;
            }
        }
        else {
            RbNode* uncle = grandParent->rbLeft;
            if (uncle != nullptr && uncle->rbColor == RED) {
                grandParent->rbColor = RED;
                parent->rbColor      = BLACK;
                uncle->rbColor       = BLACK;
                node                 = grandParent;
            }
            else {
                if (node == parent->rbLeft) {
                    rotateRight(parent);
                    node   = parent;
                    parent = node->rbParent;
                }
                rotateLeft(grandParent);
                std::swap(parent->rbColor, grandParent->rbColor);
                node = parent;
            }
        }
    }
    root->rbColor = BLACK;
}

template<class Value, class Compare>
void URbTree<Value, Compare>::doInsert(RbNode* node) noexcept
{
    node->rbLeft    = nullptr;
    node->rbRight   = nullptr;
    node->rbTree    = this;
    node->rbColor   = RED;
    RbNode* parent  = nullptr;
    RbNode* current = root;
    while (current != nullptr) {
        parent = current;
        if (compare(node, current)) {
            current = current->rbLeft;
        }
        else {
            current = current->rbRight;
        }
    }
    node->rbParent = parent;
    if (parent == nullptr) {
        root = node;
    }
    else if (compare(node, parent)) {
        parent->rbLeft = node;
    }
    else {
        parent->rbRight = node;
    }
    fixViolation(node);
}

template<class Value, class Compare>
void URbTree<Value, Compare>::doErase(RbNode* current) noexcept
{
    current->rbTree = nullptr;
    RbNode* node    = nullptr;
    RbNode* child   = nullptr;
    RbColor color   = RED;
    if (current->rbLeft != nullptr && current->rbRight != nullptr) {
        RbNode* replace = current;
        replace         = replace->rbRight;
        while (replace->rbLeft != nullptr) {
            replace = replace->rbLeft;
        }
        if (current != replace->rbParent) {
            current->rbParent->rbLeft  = replace->rbRight;
            replace->rbRight           = current->rbRight;
            current->rbRight->rbParent = replace;
        }
        else {
            replace->rbParent = current;
        }
        if (current == root) {
            root = replace;
        }
        else if (current->rbParent->rbLeft == current) {
            current->rbParent->rbLeft = replace;
        }
        else {
            current->rbParent->rbRight = replace;
        }
        replace->rbLeft           = current->rbLeft;
        current->rbLeft->rbParent = replace;
        node                      = replace;
        color                     = node->rbColor;
        child                     = node->rbRight;
    }
    else {
        node  = current;
        color = node->rbColor;
        child = (node->rbLeft != nullptr) ? node->rbLeft : node->rbRight;
    }
    if (child != nullptr) {
        child->rbParent = node->rbParent;
    }
    if (node == root) {
        root = child;
    }
    else if (node->rbParent->rbLeft == node) {
        node->rbParent->rbLeft = child;
    }
    else {
        node->rbParent->rbRight = child;
    }
    if (color == BLACK && root) {
        fixViolation(child ? child : node->rbParent);
    }
}

template<class Value, class Compare>
typename URbTree<Value, Compare>::RbNode* URbTree<Value, Compare>::getFront() const noexcept
{
    RbNode* current = root;
    while (current->rbLeft != nullptr) {
        current = current->rbLeft;
    }
    return current;
}

template<class Value, class Compare>
typename URbTree<Value, Compare>::RbNode* URbTree<Value, Compare>::getBack() const noexcept
{
    RbNode* current = root;
    while (current->rbRight != nullptr) {
        current = current->rbRight;
    }
    return current;
}
template<class Value, class Compare>
template<class Visitor>
void URbTree<Value, Compare>::doTraverseInorder(RbNode* node, Visitor&& visitor)
{
    if (node == nullptr) {
        return;
    }
    doTraverseInorder(node->rbLeft, visitor);
    visitor(node);
    doTraverseInorder(node->rbRight, visitor);
}

template<class Value, class Compare>
void URbTree<Value, Compare>::doClear(RbNode* node)
{
    if (node == nullptr) {
        return;
    }
    doClear(node->rbLeft);
    node->rbTree = nullptr;
    doClear(node->rbRight);
}

template<class Value, class Compare>
void URbTree<Value, Compare>::doClear()
{
    doClear(root);
    root = nullptr;
}
template<class Value, class Compare>
URbTree<Value, Compare>::URbTree() noexcept(noexcept(Compare()))
    : Compare()
    , root(nullptr)
{
}


template<class Value, class Compare>
URbTree<Value, Compare>::URbTree(Compare comp) noexcept(noexcept(Compare(comp)))
    : Compare(comp)
    , root(nullptr)
{
}



template<class Value, class Compare>
void URbTree<Value, Compare>::insert(Value& value) noexcept
{
    doInsert(&static_cast<RbNode&>(value));
}

template<class Value, class Compare>
void URbTree<Value, Compare>::erase(Value& value) noexcept
{
    doErase(&static_cast<RbNode&>(value));
}

template<class Value, class Compare>
bool URbTree<Value, Compare>::empty() const noexcept
{
    return root == nullptr;
}

template<class Value, class Compare>
Value& URbTree<Value, Compare>::front() const noexcept
{
    return static_cast<Value&>(*getFront());
}

template<class Value, class Compare>
Value& URbTree<Value, Compare>::back() const noexcept
{
    return static_cast<Value&>(*getBack());
}

template<class Value, class Compare>
template<class Visitor, class V>
std::pair<typename URbTree<Value, Compare>::RbNode*, typename URbTree<Value, Compare>::RbNode*>
URbTree<Value, Compare>::traverseEqualRange(Visitor&& visitor, V&& value)
{
}

template<class Value, class Compare>
template<class Visitor>
void URbTree<Value, Compare>::traverseInorder(Visitor&& visitor)
{
    doTraverseInorder(root, [visitor = std::forward<Visitor>(visitor)](RbNode* node) mutable { visitor(static_cast<Value&>(*node)); });
}

template<class Value, class Compare>
void URbTree<Value, Compare>::clear()
{
    doClear();
}
NAO_NAMESPACE_END

#endif