/**
 * @FilePath     : /cola/src/UtilsCtrl/Trie/UTrieNode.h
 * @Description  :
 * @Author       : naonao
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-06-20 16:38:08
 **/
#ifndef NAO_UTRIENODE_H
#define NAO_UTRIENODE_H

#include "../UtilsObject.h"

NAO_NAMESPACE_BEGIN

const static unsigned int MAX_TRIE_NODE_SIZE = 256;

class UTrieNode : public UtilsObject
{
public:
    explicit UTrieNode(const std::string& path = NAO_EMPTY)
    {
        path_ = path;
        for (auto& child : children_) {
            child = nullptr;
        }
    }

    ~UTrieNode() override
    {
        for (auto& child : children_) {
            NAO_DELETE_PTR(child)
        }
    }


private:
    bool        is_end_{false};
    std::string path_;
    UTrieNode*  children_[MAX_TRIE_NODE_SIZE]{};

    friend class UTrie;
    friend class UTrieV2;
};

using UTrieNodePtr = UTrieNode*;
using UTrieNodeRef = UTrieNode&;

NAO_NAMESPACE_END

#endif   // NAO_UTRIENODE_H
