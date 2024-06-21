/**
 * @FilePath     : /cola/src/UtilsCtrl/FileSystem/UFs.cpp
 * @Description  :
 * @Author       : naonao
 * @Date         : 2024-06-21 18:15:37
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-06-22 01:02:14
 **/
#include "UFs.h"

NAO_NAMESPACE_BEGIN

namespace fs = std::filesystem;
bool UFs::existsDir(const std::string& dirPath)
{
    return fs::exists(dirPath);
}

bool UFs::isDir(const std::string& dirPath)
{
    return fs::is_directory(dirPath);
}

bool UFs::creatDir(const std::string& dirPath)
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
bool UFs::folderIsEmpty(const std::string& dirPath)
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
void UFs::reNameFile(const std::string& filename, const std::string& newname)
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
    std::regex               Img(format, std::regex_constants::syntax_option_type::icase);
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
NAO_NAMESPACE_END