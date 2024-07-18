/**
 * @FilePath     : /cola/cola/UtilsCtrl/Config/UCongif.cpp
 * @Description  :
 * @Author       : naonao
 * @Date         : 2024-07-16 08:53:01
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-07-16 09:25:52
 **/
#include <utility>

#include <utility>

#include "UConfig.h"

NAO_NAMESPACE_BEGIN


typedef std::pair<std::string, int> PAIR;

UConfig::UConfig(const std::string& filename, std::string delimiter /*= "="*/, std::string comment /*= "#"*/)
    : delimiter_(std::move(delimiter))
    , comment_(std::move(comment))
{
    std::ifstream input(filename.c_str());
    if (!input) {
        throw File_not_found(filename);
    }
    input >> (*this);
}

UConfig::UConfig()
    : delimiter_(std::string(1, '='))
    , comment_(std::string(1, '#'))
{
    // Construct a Config without a file; empty
}

bool UConfig::fileExist(const std::string& filename)
{
    bool          exist = false;
    std::ifstream input(filename.c_str());
    if (input) {
        exist = true;
    }
    return exist;
}

void UConfig::readFile(const std::string& filename, std::string delimiter /*= "="*/, std::string comment /*= "#"*/)
{
    delimiter_ = std::move(delimiter);
    comment_   = std::move(comment);
    std::ifstream input(filename.c_str());
    if (!input) {
        throw File_not_found(filename);
    }
    input >> (*this);
}

bool UConfig::keyExists(const std::string& in_key) const
{
    auto p = contents_index_.find(in_key);
    return (p != contents_index_.end());
}

void UConfig::addNotes(const std::string& notes)
{
    notes_.push_back(notes);
}

void UConfig::addDescription(const std::string& des)
{
    description_ = des;
}

void UConfig::remove(const std::string& in_key)
{
    mapci p     = contents_index_.find(in_key);
    bool  found = (p != contents_index_.end());
    if (found) {
        size_t index     = p->second;
        contents_[index] = "";   // 置为空。
        contents_index_.erase(contents_index_.find(in_key));
    }
}

bool CmpByValue(const PAIR lhs, const PAIR rhs)
{
    return lhs.second < rhs.second;
}


void UConfig::Trim(std::string& inout_s)
{
    static const char whitespace[] = " \n\t\v\r\f";
    inout_s.erase(0, inout_s.find_first_not_of(whitespace));
    inout_s.erase(inout_s.find_last_not_of(whitespace) + 1U);
}

std::ostream& operator<<(std::ostream& os, const UConfig& cf)
{
    if ((cf.contents_.size() != cf.notes_.size()) || (cf.contents_index_.size() != cf.contents_.size())) {
        throw UConfig::Key_not_found("注释与键值对的条目数量不一致");
    }
    if ((cf.description_.size()) != 0) {
        os << cf.description_ << std::endl;
    }
    std::vector<PAIR> vec(cf.contents_index_.begin(), cf.contents_index_.end());
    std::sort(vec.begin(), vec.end(), CmpByValue);
    int i = 0;
    for (auto p = vec.begin(); p != vec.end(); ++p) {
        os << cf.notes_[i];
        os << "#####################################################################" << std::endl;
        os << p->first << "     " << cf.delimiter_ << "     ";
        size_t index = p->second;
        os << cf.contents_[index] << std::endl << std::endl;
        i++;
    }
    return os;
}

std::istream& operator>>(std::istream& is, UConfig& cf)
{
    typedef std::string::size_type pos;                      // 读入键和值，保持内部空格
    const std::string&             delim = cf.delimiter_;    // 分割符  默认为 =
    const std::string&             comm  = cf.comment_;      // 注释开头 默认为 #
    const pos                      skip  = delim.length();   // 分隔符的长度 （= 的长度）
    std::string                    nextline;                 // might need to read ahead to see where value ends
    while (is || nextline.length() > 0) {
        std::string line;   // 一次读取整行
        if (nextline.length() > 0) {
            line     = nextline;   // we read ahead; use it now
            nextline = "";
        }
        else {
            std::getline(is, line);
        }
        if (line.compare(0, comm.length(), comm) == 0 || line == "") {
            continue;   // 忽略注释行与空白行，注释以#开头
        }
        pos delimPos = line.find(delim);   // 找到键和值的分隔符的位置
        if (delimPos < std::string::npos) {
            std::string key = line.substr(0, delimPos);               // 提取键
            line.replace(0, delimPos + skip, "");                     // 从0 到 delimPos + skip 位置的字符替换为""，此时line为剩下的部分
            if ((line == "" || line.size() < cf.max_size_) && is) {   // 键与值不在同一行，查找下一行，遇到空白行或注释停止，   然后检测下一个键值，直到文件尾
                bool terminate = false;
                while (!terminate && is) {
                    std::getline(is, nextline);
                    terminate          = true;
                    std::string nlcopy = nextline;
                    UConfig::Trim(nlcopy);
                    if (nlcopy.compare(0, comm.length(), comm) == 0 || nlcopy == "") {
                        break;   // 遇到空白行或注释停止
                    }
                    nlcopy = nextline;
                    UConfig::Trim(nlcopy);
                    // if (nlcopy != "") line += "\n";
                    line += nextline;
                    terminate = false;
                }
            }
            UConfig::Trim(key);   // 存储键值对，如果有重复的键，则覆盖
            UConfig::Trim(line);
            cf.contents_index_.insert(std::pair<std::string, int>(key, cf.contents_size_));
            cf.contents_.push_back(line);
            cf.contents_size_++;
        }
    }
    return is;
}

NAO_NAMESPACE_END