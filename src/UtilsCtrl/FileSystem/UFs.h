/**
 * @FilePath     : /cola/src/UtilsCtrl/FileSystem/UFs.h
 * @Description  :
 * @Author       : naonao
 * @Date         : 2024-06-21 18:15:15
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-06-22 00:39:49
 **/
#ifndef NAO_UFS_H
#define NAO_UFS_H


#include <filesystem>
#include <regex>

#include "../UtilsObject.h"

NAO_NAMESPACE_BEGIN
class UFs : public UtilsObject
{
public:
    UFs();
    ~UFs() override = default;


    /**
     * @brief:
     * @param filePath
     * @return
     * @note :
     **/
    static bool      existsDir(const std::string& dirPath);
    static bool      isDir(const std::string& dirPath);
    static bool      creatDir(const std::string& dirPath);
    static uintmax_t removeDir(const std::string& dirPath);
    static bool      folderIsEmpty(const std::string& dirPath);

    static std::string getBaseName(const std::string& filePath);
    static std::string getParentParh(const std::string& filePath);
    static std::string getExtensionName(const std::string& filePath);
    static void        reNameFile(const std::string& filename, const std::string& newname);

    static uintmax_t getFileSize(const std::string& filePath);

    static std::vector<std::string> getAllFiles(const std::string& path, const std::vector<std::string>& filter_directory = {});
    static std::vector<std::string> getAllFormatFiles(const std::string& path, const std::string& format = "(.*)(.png|jpg|bmp)",
                                                      const std::vector<std::string>& filter_directory = {});

    static std::vector<std::string> getAllFormatFiles(const std::vector<std::string>& path, const std::string& format = "(.*)(.png|jpg|bmp)",
                                                      const std::vector<std::string>& filter_directory = {});
};
NAO_NAMESPACE_END
#endif   // NAO_UFS_H