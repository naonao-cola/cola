/**
 * @FilePath     : /cola/src/UtilsCtrl/Trie/UTrie.cpp
 * @Description  :
 * @Author       : naonao
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-06-21 10:24:48
 **/
#include "UTrie.h"

NAO_NAMESPACE_BEGIN

UTrie::UTrie(){head_ = NAO_SAFE_MALLOC_NOBJECT(UTrieNode)}

UTrie::~UTrie()
{
    clear();
    NAO_DELETE_PTR(head_)
}


void UTrie::insert(const std::string& path)
{
    innerInsert(head_, path, 0);
}


bool UTrie::find(const std::string& path)
{
    return innerFind(head_, path, 0);
}
void UTrie::clear()
{
    innerClear(head_);
}
void UTrie::eraser(const std::string& path)
{
    bool isErased = false;
    innerEraser(head_, path, 0, isErased);
}


bool UTrie::innerFind(UTrieNodePtr node, const std::string& path, int index)
{
    if (nullptr == node) {
        return false;
    }

    int i = (int)path[index];   // char转int，用数字表示
    if (index == path.size()) {
        return node->is_end_;
    }

    index++;
    bool result = false;
    if (node->children_[i]) {
        result = innerFind(node->children_[i], path, index);
    }

    return result;
}

void UTrie::innerInsert(UTrieNodePtr node, const std::string& path, int index)
{
    if (nullptr == node) {
        return;
    }

    if (index == path.size()) {
        node->is_end_ = true;
        return;
    }

    int i = (int)path[index];
    index++;
    if (!node->children_[i]) {
        std::string curPath = path.substr(0, index);
        node->children_[i]  = new UTrieNode(curPath);
    }

    innerInsert(node->children_[i], path, index);
}

void UTrie::innerClear(UTrieNodePtr node)
{
    if (nullptr == node) {
        return;
    }

    for (auto& child : node->children_) {
        innerClear(child);
        NAO_DELETE_PTR(child)
    }
}

void UTrie::innerEraser(UTrieNodePtr node, const std::string& path, int index, bool& isErased)
{
    if (nullptr == node || isErased) {
        return;
    }

    if (index == path.size() && node->path_ == path && node->is_end_) {
        node->is_end_ = false;
        isErased      = true;   // 剪枝逻辑
        return;
    }

    index++;
    for (auto& child : node->children_) {
        if (child) {
            innerEraser(child, path, index, isErased);
        }
    }
}

NAO_NAMESPACE_END
