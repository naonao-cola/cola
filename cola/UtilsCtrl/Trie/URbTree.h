/**
 * @FilePath     : /cola/cola/UtilsCtrl/Trie/URbtree.h
 * @Description  :
 * @Author       : naonao
 * @Date         : 2024-07-03 13:00:41
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-07-03 15:30:08
 **/
#ifndef NAO_UBRTREE_H
#define NAO_UBRTREE_H

#include "../UtilsObject.h"

NAO_NAMESPACE_BEGIN
template<class Value, class Compare = std::less<>>
struct URbTree : public UtilsObject, private Compare
{
private:
    enum RbColor
    {
        RED,
        BLACK
    };

protected:
    struct RbNode
    {
        RbNode() noexcept;
        friend struct URbTree;

    private:
        RbNode* rbLeft;
        RbNode* rbRight;
        RbNode* rbParent;

    protected:
        URbTree* rbTree;

    private:
        RbColor rbColor;
    };

public:
    struct NodeType : RbNode
    {
        NodeType()           = default;
        NodeType(NodeType&&) = delete;
        ~NodeType() noexcept { erase_from_parent(); }

    protected:
        void erase_from_parent();
    };

private:
    RbNode* root;
    bool    compare(RbNode* left, RbNode* right) const noexcept;
    void    rotateLeft(RbNode* node) noexcept;
    void    rotateRight(RbNode* node) noexcept;
    void    fixViolation(RbNode* node) noexcept;
    void    doInsert(RbNode* node) noexcept;
    void    doErase(RbNode* current) noexcept;
    RbNode* getFront() const noexcept;
    RbNode* getBack() const noexcept;
    template<class Visitor>
    void doTraverseInorder(RbNode* node, Visitor&& visitor);
    void doClear(RbNode* node);
    void doClear();

public:
    URbTree() noexcept(noexcept(Compare()));
    explicit URbTree(Compare comp) noexcept(noexcept(Compare(comp)));
    URbTree(URbTree&&)           = delete;
    ~URbTree() noexcept override = default;
    void   insert(Value& value) noexcept;
    void   erase(Value& value) noexcept;
    bool   empty() const noexcept;
    Value& front() const noexcept;
    Value& back() const noexcept;
    template<class Visitor, class V>
    std::pair<RbNode*, RbNode*> traverseEqualRange(Visitor&& visitor, V&& value);
    template<class Visitor>
    void traverseInorder(Visitor&& visitor);
    void clear();
};
NAO_NAMESPACE_END

#include "URbTree.inl"

#endif