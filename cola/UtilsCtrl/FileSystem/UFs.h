/**
 * @FilePath     : /cola/src/UtilsCtrl/FileSystem/UFs.h
 * @Description  :
 * @Author       : naonao
 * @Date         : 2024-06-21 18:15:15
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-06-22 12:11:01
 **/
#ifndef NAO_UFS_H
#define NAO_UFS_H

#include <fstream>

#include "../UtilsObject.h"

NAO_NAMESPACE_BEGIN

class UFs : public UtilsObject
{
public:
    UFs()=default;
    ~UFs() override = default;
    NAO_NO_ALLOWED_COPY(UFs)
    /**
     * @brief: 目录是否存在
     * @param filePath
     * @return
     * @note :
     **/
    static NBool existsDir(const std::string& dirPath);

    /**
     * @brief: 是否是目录
     * @param dirPath
     * @return
     * @note :
     **/
    static NBool isDir(const std::string& dirPath);

    /**
     * @brief: 创造目录
     * @param dirPath
     * @return
     * @note :
     **/
    static NBool creatDir(const std::string& dirPath);

    /**
     * @brief: 删除目录
     * @param dirPath
     * @return
     * @note :
     **/
    static uintmax_t removeDir(const std::string& dirPath);

    /**
     * @brief: 文件夹是否为空
     * @param dirPath
     * @return
     * @note :
     **/
    static NBool folderIsEmpty(const std::string& dirPath);

    /**
     * @brief: 获取文件名
     * @param filePath
     * @return
     * @note :
     **/
    static std::string getBaseName(const std::string& filePath);

    /**
     * @brief: 获取父目录
     * @param filePath
     * @return
     * @note :
     **/
    static std::string getParentParh(const std::string& filePath);

    /**
     * @brief: 获取扩展名
     * @param filePath
     * @return
     * @note :
     **/
    static std::string getExtensionName(const std::string& filePath);

    /**
     * @brief: 重命名文件，或者移动文件
     * @param filename
     * @param newname
     * @return
     * @note :
     **/
    static NVoid renameFile(const std::string& filename, const std::string& newname);

    /**
     * @brief: 拷贝文件
     * @param filename
     * @param newname
     * @return
     * @note :
     **/
    static NVoid copyFile(const std::string& filename, const std::string& newname);

    /**
     * @brief: 获取文件大小
     * @param filePath
     * @return
     * @note :
     **/
    static uintmax_t getFileSize(const std::string& filePath);

    /**
     * @brief: 获取所有文件
     * @param path
     * @return
     * @note :
     **/
    static std::vector<std::string> getAllFiles(const std::string& path, const std::vector<std::string>& filter_directory = {});

    /**
     * @brief: 获取特定格式文件
     * @param path
     * @param format
     * @return
     * @note :
     **/
    static std::vector<std::string> getAllFormatFiles(const std::string& path, const std::string& format = "(.*)(.png|jpg|bmp)",
                                                      const std::vector<std::string>& filter_directory = {});

    /**
     * @brief: 获取特定格式文件
     * @param format
     * @return
     * @note :
     **/
    static std::vector<std::string> getAllFormatFiles(const std::vector<std::string>& path, const std::string& format = "(.*)(.png|jpg|bmp)",
                                                      const std::vector<std::string>& filter_directory = {});
};

/**
 * @brief 读写文件
 * open mode:
 * 'r': read         open if exists
 * 'a': append       created if not exists
 * 'w': write        created if not exists, truncated if exists
 * 'r_t'文本模式
 * 'a_t'
 * 'w_t'
 */
class UFile : public UtilsObject
{
public:
    static const NInt seek_beg = 0;
    static const NInt seek_cur = 1;
    static const NInt seek_end = 2;

    /**
     * @brief 第 1 个版本是默认构造函数，创建一个空的 file 对象，不会打开任何文件。
     */
    UFile() = default;
    ~UFile() override;

    /**
     * @brief 第 2 个版本是 move 构造函数，支持将 file 对象放到 STL 容器中。
     * @param f
     */
    UFile(UFile&& f) noexcept;

    /**
     * @brief 第 3-4 个版本，打开指定的文件，参数 path 是文件路径，参数 mode 是打开模式。
     * @param path
     * @param mode  r  a  w
     */
    UFile(NConStr path, std::string mode);
    UFile(const std::string& path, std::string mode);
    UFile(const UFile& x)          = delete;
    NVoid operator=(const UFile& x) = delete;
    NVoid operator=(UFile&& x)      = delete;

    /**
     * @brief 将 fs::file 转换为 NBool 类型，文件成功打开时返回 true，否则返回 false。
     */
    explicit operator NBool() const;

    /**
     * @brief 文件未打开或打开失败时返回 true，否则返回 false。
     * @return 返回结果
     */
    NBool operator!() const;

    /**
     * @brief 此方法返回 file 内部文件路径的引用。
     * 若 file 对象并未关联任何文件，则返回值会引用一个空字符串。
     * @return 返回结果
     */
    const std::string& path() const;

    /**
     * @brief 此方法打开指定的文件，path 是文件路径，mode 是打开模式，见构造函数中的说明。此方法在打开文件前，会先关闭之前打开的文件。
     * @param path 路径
     * @param mode 模式
     * @param type 默认以二进制形式读写
     * @return
     */
    NBool open(NConStr path, std::string mode);
    NBool open(const std::string& path, std::string mode);
    /**
     * @brief 关闭文件，析构函数中会自动调用此方法。
     * 多次调用此方法是安全的。
     */
    NVoid close();

    /**
     * @brief 设置文件指针的当前位置，参数 off 是偏移位置，参数 whence 是起始位置，可以是file::seek_beg, file::seek_cur, file::seek_end
     * 中的一种。此方法对以 'a' (append) 模式打开的文件无效。
     * @param off
     * @param whence
     */
    NVoid seek(int64_t off, NInt whence = seek_beg);

    /**
     * @brief 第 1 个版本读取数据到指定的 buffer 中，n 是要读取的字节数，返回实际读取的字节数。第 2 个版本与第 1 个版本类似，但以 fastring
     * 的形式返回读取的数据，n 是要读取的字节数。此方法在遇到文件尾或发生错误时，实际读取的字节数可能小于 n。
     * @param buf
     * @param n
     * @return
     */
    NSize      read(NVoidPtr buf, NSize n);
    std::string read(NSize n);

    /**
     * @brief 写入数据
     * @param s
     * @param n
     * @return
     * 字符串写入还是字符串
     */
    NSize write(const void* buf, NSize n);
    NSize write(NConStr s);
    NSize write(const std::string& s);
    NSize write(NChar c);

private:
    std::ifstream fin_;
    std::ofstream fout_;
    std::string   path_;
};

/**
 * @brief UFile 不支持缓存，写小文件性能较差，为此，另外实现了支持缓存的 fl::fstream 类，fl::fstream 只用于写文件，不支持读操作。
 */
class UFstream : public UtilsObject
{
public:
    /**
     * @brief 第 1 个版本是默认构造函数，内部缓存大小为 8k。
     */
    UFstream();

    /**
     * @brief 第 2 个版本是 move 构造函数，可以将 UFstream 对象放到 STL 容器中。
     * @param fs
     */
    UFstream(UFstream&& fs) noexcept;

    /**
     * @brief 第 3 个版本用参数 cap 指定缓存的大小。
     * @param cap
     */
    explicit UFstream(NSize cap);

    /**
     * @brief 第 4-5 个版本打开指定的文件，path 是文件路径，mode 是模式，cap 是缓存大小，默认为 8k。
     * @param path
     * @param mode
     * @param cap
     */
    UFstream(NConStr path, const std::string& mode, NSize cap = 8192);
    UFstream(const std::string& path, const std::string& mode, NSize cap = 8192);
    ~UFstream() noexcept override;

    /**
     * @brief 将 fs::fstream 转换为 NBool 类型，文件成功打开时返回 true，否则返回 false。
     */
    explicit operator NBool() const;
    NBool     operator!() const;

    /**
     * @brief 打开指定的文件，参数 path 是文件路径，参数 mode 是打开模式，见构造函数中的说明
     * @param path
     * @param mode
     * @return
     */
    NBool open(NConStr path, const std::string mode);
    NBool open(const std::string& path, std::string mode);

    /**
     * @brief 调整缓存容量，参数 n 是容量大小。若 n 小于之前的容量，则缓存容量保持不变。
     * @param n
     */
    NVoid reserve(size_t n);

    NVoid flush();

    NVoid close();

    /**
     * @brief 追加数据，参数 n 是数据的长度。
     * @param s
     * @param n
     * @return
     */
    UFstream& append(const void* s, NSize n);

    UFstream& operator<<(NConStr s);

    UFstream& operator<<(const std::string& s);

    // 其他类型的直接写
    template<typename T>
    UFstream& operator<<(T v)
    {
        if (cap_ < s_.size() + 24) this->flush();
        this->append((const char*)&v, sizeof(T));
        return *this;
    }
private:
    UFile       f_;
    std::string s_;
    NSize      cap_;
};
NAO_NAMESPACE_END
#endif   // NAO_UFS_H