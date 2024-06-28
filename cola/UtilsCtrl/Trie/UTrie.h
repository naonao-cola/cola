/**
 * @FilePath     : /cola/src/UtilsCtrl/Trie/UTrie.h
 * @Description  :
 * @Author       : naonao
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-06-21 14:34:29
 **/
#ifndef NAO_UTRIE_H
#define NAO_UTRIE_H

#include "../UtilsObject.h"
#include "UTrieNode.h"

NAO_NAMESPACE_BEGIN

/**
 * @brief: 将字符串加入链表，链表每一层级有256的子节点。字符串按照asici码加入链表的每一个层级，每一个层级比上一个层级多一个字符，最后的字符的end标志位true
 *         每一个node 是一个链表。链表的层数由字符串的长度决定。使用递归的方式实现
 * @return
 * @note :
**/
class UTrie : public UtilsObject
{
public:
    explicit UTrie();
    ~UTrie() override;

    /**
     * 插入路径信息
     * @param path
     */
    void insert(const std::string& path);

    /**
     * 查询路径信息
     * @param path
     * @return 找到返回1，未找到返回0
     */
    bool find(const std::string& path);

    /**
     * 清空全部路径信息
     */
    void clear();

    /**
     * 删除路径信息
     * @param path
     */
    void eraser(const std::string& path);

protected:
    /**
     * 查询信息
     * @param node
     * @param path
     * @param index
     * @return
     */
    bool innerFind(UTrieNodePtr node, const std::string& path, int index);

    /**
     * 插入信息
     * @param node
     * @param path
     * @param index
     */
    void innerInsert(UTrieNodePtr node, const std::string& path, int index);

    /**
     * 清空所有节点
     * @param node
     */
    void innerClear(UTrieNodePtr node);

    /**
     * 删除特定节点
     * @param node
     * @param path
     * @param index
     * @param isErased
     */
    void innerEraser(UTrieNodePtr node, const std::string& path, int index, bool& isErased);

private:
    UTrieNodePtr head_{nullptr};
};

NAO_NAMESPACE_END

#endif   // NAO_UTRIE_H
