/**
 * @FilePath     : /cola/cola/UtilsCtrl/FileSystem/UFs.cpp
 * @Description  :
 * @Author       : naonao
 * @Date         : 2024-06-21 18:15:37
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-06-28 23:12:03
 **/
#include <array>
#include <filesystem>
#include <iostream>
#include <regex>
#include <utility>

#include "UFs.h"

NAO_NAMESPACE_BEGIN

namespace fs = std::filesystem;
NBool UFs::existsDir(const std::string& dirPath)
{
    return fs::exists(dirPath);
}

NBool UFs::isDir(const std::string& dirPath)
{
    return fs::is_directory(dirPath);
}

NBool UFs::creatDir(const std::string& dirPath)
{
    fs::path dir = dirPath;
    if (fs::exists(dir)) return true;
    fs::create_directory(dir);
    return fs::exists(dir);
}

uintmax_t UFs::removeDir(const std::string& dirPath)
{
    fs::path dir = dirPath;
    return fs::remove_all(dir);
}
NBool UFs::folderIsEmpty(const std::string& dirPath)
{
    fs::path tmp_name = dirPath;
    return fs::is_empty(tmp_name);
}

std::string UFs::getBaseName(const std::string& filePath)
{
    return fs::path(filePath).stem().string();
}

std::string UFs::getExtensionName(const std::string& filePath)
{
    return fs::path(filePath).extension().string();
}
NVoid UFs::reNameFile(const std::string& filename, const std::string& newname)
{
    fs::rename(filename.c_str(), newname.c_str());
}
std::string UFs::getParentParh(const std::string& filePath)
{
    return fs::path(filePath).parent_path().string();
}

uintmax_t UFs::getFileSize(const std::string& filePath)
{
    if (!fs::exists(filePath)) return 0;
    return fs::file_size(filePath);
}

std::vector<std::string> UFs::getAllFiles(const std::string& path, const std::vector<std::string>& filter_directory)
{
    std::vector<std::string> files;
    fs::path                 file_path = path;
    if (!fs::exists(file_path)) return std::move(files);
    if (fs::is_directory(file_path)) {
        for (const auto& f : fs::recursive_directory_iterator(file_path)) {
            int foundFlag = 0;   // 过滤文件夹
            for (const auto& filter_item : filter_directory) {
                std::string::size_type idx = f.path().string().find(filter_item);
                if (idx != std::string::npos) {
                    foundFlag = 1;
                    break;
                }
            }
            if (foundFlag == 0 && !fs::is_directory(f)) {
                files.push_back(f.path().string());
            }
        }
    }
    if (!fs::is_directory(file_path)) files.push_back(path);
    return std::move(files);
}

std::vector<std::string> UFs::getAllFormatFiles(const std::string& path, const std::string& format, const std::vector<std::string>& filter_directory)
{
    std::vector<std::string> files = getAllFiles(path, filter_directory);
    std::regex               Img(format, std::regex_constants::icase);
    std::vector<std::string> files_path;
    for (const auto& file : files) {
        if (std::regex_match(file, Img)) {
            files_path.push_back(file);
        }
    }
    return std::move(files_path);
}

std::vector<std::string> UFs::getAllFormatFiles(const std::vector<std::string>& path, const std::string& format,
                                                const std::vector<std::string>& filter_directory)
{
    std::vector<std::string> files_path;
    for (const auto& path_item : path) {
        std::vector<std::string> temp_file_vec = getAllFormatFiles(path_item, format, filter_directory);
        files_path.insert(files_path.end(), temp_file_vec.begin(), temp_file_vec.end());
    }
    return std::move(files_path);
}

UFile::UFile(UFile&& f) noexcept
    : fin_(std::move(f.fin_))
    , fout_(std::move(f.fout_))
    , path_(std::move(f.path_))
{}

UFile::UFile(NConStr path, std::string mode)
{
    this->open(path, std::move(mode));
}

UFile::UFile(const std::string& path, std::string mode)
    : UFile(path.c_str(), std::move(mode))
{}
UFile::~UFile()
{
    this->close();
};

UFile::operator NBool() const
{
    return fout_.is_open() || fin_.is_open();
}
NBool UFile::operator!() const
{
    return !(NBool)(*this);
}
const std::string& UFile::path() const
{
    if (path_.size() > 0) return path_;
    static std::string kPath;
    return kPath;
}

NBool UFile::open(NConStr path, std::string mode)
{
    this->close();
    path_ = std::string(path);
    if (mode == "r") {
        fin_.open(path_, std::ios::binary);
        return fin_.is_open();
    }
    if (mode == "r_t") {
        fin_.open(path_, std::ios::in);
        return fin_.is_open();
    }
    if (mode == "a") {
        fout_.open(path_, std::ios::binary | std::ios::app);
        return fout_.is_open();
    }
    if (mode == "a_t") {
        fout_.open(path_, std::ios::out | std::ios::app);
        return fout_.is_open();
    }
    if (mode == "w") {
        fout_.open(path_, std::ios::binary);
        return fout_.is_open();
    }
    if (mode == "w_t") {
        fout_.open(path_, std::ios::out);
        return fout_.is_open();
    }
    return false;
}

NBool UFile::open(const std::string& path, std::string mode)
{
    return this->open(path.c_str(), std::move(mode));
}
NVoid UFile::close()
{
    if (fin_.is_open()) fin_.close();
    if (fout_.is_open()) fout_.close();
}

NVoid UFile::seek(int64_t off, NInt whence)
{
    static std::array<NInt, 3> seekfrom = {std::ios::beg, std::ios::cur, std::ios::end};
    whence                              = seekfrom[whence];
    if (fin_.is_open()) fin_.seekg(off, whence);
}

NSize UFile::read(void* buf, NSize n)
{
    if (!fin_.is_open()) return 0;
    const NSize N = 1u << 30;   // 1G
    if (n >= N) {
        throw std::logic_error("单次读取文件过大,超过1G");
    }
    // 判断读取大小是否超限
    {
        std::streampos cur_pos = fin_.tellg();
        fin_.seekg(0, std::ios::end);
        std::streampos remain = fin_.tellg();
        if (n > static_cast<size_t>(remain - cur_pos)) n = remain - cur_pos;
        if (n == 0) return 0;
        fin_.seekg(cur_pos);
    }
    fin_.read(static_cast<char*>(buf), n);
    return n;
}

std::string UFile::read(NSize n)
{
    std::string s(n + 1, ' ');
    s.resize(this->read(static_cast<void*>(s.data()), n));
    return s;
}
NSize UFile::write(const void* buf, NSize n)
{
    if (!fout_.is_open()) return 0;
    const NSize N = 1u << 30;   // 1G
    if (n >= N) {
        throw std::logic_error("单次读取文件过大,超过1G");
    }
    fout_.write((char*)(buf), n);
    return n;
}

NSize UFile::write(NConStr s)
{
    return this->write(s, strlen(s));
}

NSize UFile::write(const std::string& s)
{
    return this->write(s.data(), s.size());
}

NSize UFile::write(char c)
{
    return this->write(&c, 1);
}

UFstream::UFstream()
    : cap_(8192)
{
    s_.resize(0);
}

UFstream::UFstream(UFstream&& fs) noexcept
    : s_(std::move(fs.s_))
    , f_(std::move(fs.f_))
    , cap_(fs.cap_)
{}

UFstream::UFstream(size_t cap)
    : cap_(cap)
{
    s_.resize(0);
}

UFstream::UFstream(NConStr path, const std::string& mode, NSize cap)
    : f_(path, mode == "w" ? "w" : "a")
    , cap_(cap)
{
    s_.resize(0);
}

UFstream::UFstream(const std::string& path, const std::string& mode, NSize cap)
    : UFstream(path.c_str(), mode, cap)
{}

UFstream::~UFstream() noexcept
{
    this->close();
}
UFstream::operator bool() const
{
    return (NBool)f_;
}
NBool UFstream::operator!() const
{
    return !(NBool)f_;
}

NBool UFstream::open(NConStr path, const std::string mode)
{
    this->close();
    return f_.open(path, mode == "w" ? "w" : "a");
}
NBool UFstream::open(const std::string& path, std::string mode)
{
    return this->open(path.c_str(), std::move(mode));
}

NVoid UFstream::reserve(NSize n)
{
    if (n <= cap_) {
        return;
    }
    cap_ = n;
    s_.resize(cap_);
}

NVoid UFstream::flush()
{
    if (!s_.empty()) {
        f_.write(s_.data(), s_.size());
        s_.clear();
    }
}

NVoid UFstream::close()
{
    this->flush();
    f_.close();
}

UFstream& UFstream::append(const void* s, NSize n)
{
    if (cap_ < s_.size() + n) this->flush();
    n <= cap_ ? ((void)s_.append((char*)s, n)) : ((void)f_.write(s, n));
    return *this;
}

UFstream& UFstream::operator<<(NConStr s)
{
    return this->append(s, strlen(s));
}

UFstream& UFstream::operator<<(const std::string& s)
{
    return this->append(s.data(), s.size());
}

NAO_NAMESPACE_END