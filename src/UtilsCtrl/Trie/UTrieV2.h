/**
 * @FilePath     : /cola/src/UtilsCtrl/Trie/UTrieV2.h
 * @Description  :
 * @Author       : naonao
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-06-20 16:36:03
 **/
#ifndef NAO_UTRIEV2_H
#define NAO_UTRIEV2_H

#include "../UtilsObject.h"
#include "UTrieNode.h"

NAO_NAMESPACE_BEGIN

class UTrieV2 : public UtilsObject
{
public:
    explicit UTrieV2();
    ~UTrieV2() override;

    /**
     * 插入路径信息
     * @param path
     */
    void insert(const std::string& path);

    /**
     * 查询路径信息
     * @param path
     * @return
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

private:
    UTrieNodePtr head_{nullptr};
};

NAO_NAMESPACE_END

#endif   // NAO_UTRIEV2_H
