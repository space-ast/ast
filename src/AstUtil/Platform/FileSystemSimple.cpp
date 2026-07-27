/// @file      SimpleFileSystem.cpp
/// @brief     
/// @details   ~
/// @author    axel
/// @date      27.11.2025
/// @copyright 版权所有 (C) 2025-present, ast项目.

/// ast项目（https://github.com/space-ast/ast）
/// 本项目基于 Apache 2.0 开源许可证分发。
/// 您可在遵守许可证条款的前提下使用、修改和分发本软件。
/// 许可证全文请见：
/// 
///    http://www.apache.org/licenses/LICENSE-2.0
/// 
/// 重要须知：
/// 软件按“现有状态”提供，无任何明示或暗示的担保条件。
/// 除非法律要求或书面同意，作者与贡献者不承担任何责任。
/// 使用本软件所产生的风险，需由您自行承担。
 
 

#include "FileSystemSimple.hpp"
#include "AstUtil/Encode.hpp"
#include "AstUtil/Logger.hpp"
#include <algorithm>
#include <cstring>

#ifdef _WIN32
#include <windows.h>
#include <fileapi.h>
#include <direct.h>
#include <shlwapi.h>
#ifdef _MSC_VER
#   pragma comment(lib, "shlwapi.lib")
#endif
#else
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/statvfs.h>
#include <fcntl.h>   // 用于 open 和相关标志
#include <dirent.h>
#include <unistd.h>  // 用于 read, write, close
#endif


AST_NAMESPACE_BEGIN

namespace fs_simple
{

    // directory_iterator 的实现
#ifdef _WIN32
    struct directory_iterator::impl
    {
        HANDLE handle;
        WIN32_FIND_DATAW data{};
        path base_path; 
        bool data_valid;
    
        impl(const path& p)
            : handle(INVALID_HANDLE_VALUE)
            , base_path(p)
            , data_valid{false}
        {
        }

        impl(const impl&) = delete;
        impl& operator=(const impl&) = delete;

        bool valid() const
        {
            return (handle != INVALID_HANDLE_VALUE) && data_valid;
        }
        bool first()
        {
            // 使用平台相关的字符串类型
            string_type search_path = (base_path / "*").native();
            std::wstring wide_path;
            aUtf8ToWide(search_path.c_str(), wide_path);  // 转换为宽字符
            handle = FindFirstFileW(wide_path.c_str(), &data);
            if (handle == INVALID_HANDLE_VALUE) {
                data_valid = false;
            }
            else {
                data_valid = true;
                skip_twodot();
            }
            return valid();
        }

        bool next()
        {
            if (handle != INVALID_HANDLE_VALUE) {
                data_valid = (FindNextFileW(handle, &data) != 0);
            }
            return valid();
        }
        void skip_twodot()
        {
            do{
                if (wcscmp(data.cFileName, L".") != 0 && wcscmp(data.cFileName, L"..") != 0)
                {
                    break;
                }
                next();
            } while (valid());
        }
        bool next_entry()
        {
            next();
            skip_twodot();
            return valid();
        }
        void close()
        {
            if (handle != INVALID_HANDLE_VALUE) {
                FindClose(handle);
            }
        }
        directory_entry entry()
        {
            if(data_valid==false)
				return directory_entry();
			std::string current_name;
            aWideToUtf8(data.cFileName, current_name);
            return directory_entry(base_path / path(current_name));
        }

        ~impl()
        {
            close();
        }
    };
#else
    struct directory_iterator::impl
    {
        DIR* dir;
        struct dirent* direntry;
        path base_path;

        impl(const path& p) 
            : dir(nullptr)
            , direntry(nullptr)
            , base_path(p)
        {
            
        }

        impl(const impl&) = delete;
        impl& operator=(const impl&) = delete;
        
        bool valid() const
        {
            return direntry != nullptr;
        }
        bool first()
        {
            dir = opendir(base_path.c_str());
            if (dir) {
                direntry = readdir(dir);
                skip_twodot();
            }
            return valid();
        }
        bool next()
        {
            if(dir)
                direntry = readdir(dir);
            return valid();
        }
        void skip_twodot()
        {
            while (valid())
            {
                if (strcmp(direntry->d_name, ".") != 0 && strcmp(direntry->d_name, "..") != 0)
                    break;
                next();
            };
        }
        bool next_entry()
        {
            next();
            skip_twodot();
            return valid();
        }
        void close()
        {
            if (dir != nullptr) {
                closedir(dir);
            }
        }
        directory_entry entry()
        {
            if(!dir)
                return directory_entry();
            return directory_entry(base_path / path(direntry->d_name));
        }
        ~impl()
        {
            close();
        }
    };
#endif

    directory_iterator::directory_iterator(const path& p)
        : impl_(std::make_shared<impl>(p)), entry_()
    {
        if (impl_->first())
        {
			entry_ = impl_->entry();
        }
        else {
			entry_ = directory_entry();
            impl_.reset();
        }
    }


    directory_iterator::directory_iterator(const directory_iterator& other)
		: impl_(other.impl_), entry_(other.entry_)
    {}

    directory_iterator& directory_iterator::operator=(const directory_iterator& other)
    {
        if (&other != this) {
            this->impl_ = other.impl_;
            this->entry_ = other.entry_;
        }
        return *this;
    }

    directory_iterator::directory_iterator(directory_iterator&& other) noexcept
        : impl_(other.impl_), entry_(std::move(other.entry_))
    {
        other.impl_ = nullptr;
    }

    directory_iterator& directory_iterator::operator=(directory_iterator&& other) noexcept
    {
        if (this != &other) {
            // delete impl_;
            impl_ = other.impl_;
            entry_ = std::move(other.entry_);
            other.impl_.reset();
        }
        return *this;
    }


    directory_iterator& directory_iterator::operator++()
    {
        if(impl_){
			if (impl_->next_entry()) {
                entry_ = impl_->entry();
            }
            else{
                entry_ = directory_entry();
                impl_.reset();
            }
        }
        return *this;
    }

    // 文件状态实现
    file_status directory_entry::status() const
    {
        return fs_simple::status(path_);
    }

    // 基础文件操作实现
    bool exists(const path& p, std::error_code& ec) noexcept
    {
    #ifdef _WIN32
        std::wstring wide_path;
        aUtf8ToWide(p.c_str(), wide_path);  // 转换为宽字符
        DWORD attrs = GetFileAttributesW(wide_path.c_str());
        if (attrs != INVALID_FILE_ATTRIBUTES) return true;
        DWORD err = GetLastError();
        if (err == ERROR_FILE_NOT_FOUND || err == ERROR_PATH_NOT_FOUND) return false;
        ec = std::error_code(static_cast<int>(err), std::system_category());
        return false;
    #else
        struct stat sb;
        if (stat(p.c_str(), &sb) == 0) return true;
        if (errno == ENOENT || errno == ENOTDIR) return false;
        ec = std::error_code(errno, std::system_category());
        return false;
    #endif
    }

    bool exists(const path& p)
    {
        std::error_code ec;
        return exists(p, ec);
    }

    bool is_directory(const path& p, std::error_code& ec) noexcept
    {
        file_status s = status(p, ec);
        if (ec) return false; // status 遇到真实错误（权限等）
        return (s.type() == file_type::directory);
    }

    

    uintmax_t file_size(const path& p)
    {
    #ifdef _WIN32
        std::wstring wide_path;
        aUtf8ToWide(p.c_str(), wide_path);  // 转换为宽字符
        HANDLE hFile = CreateFileW(wide_path.c_str(), GENERIC_READ, FILE_SHARE_READ,
            NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
        if (hFile == INVALID_HANDLE_VALUE) {
            return static_cast<uintmax_t>(-1);
        }

        DWORD sizeLow = 0, sizeHigh = 0;
        sizeLow = GetFileSize(hFile, &sizeHigh);
        CloseHandle(hFile);

        if (sizeLow == INVALID_FILE_SIZE && GetLastError() != NO_ERROR) {
            return static_cast<uintmax_t>(-1);
        }

        return (static_cast<uintmax_t>(sizeHigh) << 32) | sizeLow;
    #else
        struct stat sb;
        if (stat(p.c_str(), &sb) == 0) {
            return static_cast<uintmax_t>(sb.st_size);
        }
        return static_cast<uintmax_t>(-1);
    #endif
    }

    file_status status(const path& p, std::error_code& ec) noexcept
    {
    #ifdef _WIN32
        std::wstring wide_path;
        aUtf8ToWide(p.c_str(), wide_path);  // 转换为宽字符
        DWORD attrs = GetFileAttributesW(wide_path.c_str());
        if (attrs == INVALID_FILE_ATTRIBUTES) {
            DWORD err = GetLastError();
            if (err != ERROR_FILE_NOT_FOUND && err != ERROR_PATH_NOT_FOUND) {
                ec = std::error_code(static_cast<int>(err), std::system_category());
            }
            return file_status(file_type::not_found);
        } else if (attrs & FILE_ATTRIBUTE_DIRECTORY) {
            return file_status(file_type::directory);
        } else {
            return file_status(file_type::regular);
        }
        // return file_status(file_type::unknown);
    #else
        struct stat sb;
        if (stat(p.c_str(), &sb) == 0) {
            if (S_ISDIR(sb.st_mode))
            {
                return file_status(file_type::directory);
            }
            else if (S_ISREG(sb.st_mode)) 
            {
                return file_status(file_type::regular);
            }
            else {
                return file_status(file_type::unknown);
            }
        } else {
            if (errno != ENOENT && errno != ENOTDIR) {
                ec = std::error_code(errno, std::system_category());
            }
            return file_status(file_type::not_found);
        }
    #endif
    }

    file_status status(const path& p) noexcept
    {
        std::error_code ec;
        return status(p, ec);
    }

    // 目录操作实现
    bool create_directory(const path& p, std::error_code& ec) noexcept
    {
    #ifdef _WIN32
        std::wstring wide_path;
        aUtf8ToWide(p.c_str(), wide_path);  // 转换为宽字符
        if (CreateDirectoryW(wide_path.c_str(), NULL)) return true;
        DWORD err = GetLastError();
        if (err == ERROR_ALREADY_EXISTS) return false;
        ec = std::error_code(static_cast<int>(err), std::system_category());
        return false;
    #else
        if (mkdir(p.c_str(), 0755) == 0) return true;
        if (errno == EEXIST) return false;
        ec = std::error_code(errno, std::system_category());
        return false;
    #endif
    }

    bool create_directory(const path& p) noexcept
    {
        std::error_code ec;
        return create_directory(p, ec);
    }

    bool create_directories(const path& p, std::error_code& ec) noexcept
    {
        if (p.empty()) return false;

        // 如果目录已存在
        if (is_directory(p, ec)) return true;

        // 创建父目录
        auto parent = p.parent_path();
        if (!parent.empty() && !exists(parent, ec)) {
            if (!create_directories(parent, ec)) {
                return false;
            }
        }

        // 创建当前目录
        return create_directory(p, ec);
    }

    bool create_directories(const path& p) noexcept
    {
        std::error_code ec;
        return create_directories(p, ec);
    }

    bool remove(const path& p, std::error_code& ec) noexcept
    {
    #ifdef _WIN32
        std::wstring wide_path;
        aUtf8ToWide(p.c_str(), wide_path);  // 转换为宽字符
        if (is_directory(p, ec)) {
            if (RemoveDirectoryW(wide_path.c_str())) return true;
        } else {
            if (DeleteFileW(wide_path.c_str())) return true;
        }
        DWORD err = GetLastError();
        if (err == ERROR_FILE_NOT_FOUND || err == ERROR_PATH_NOT_FOUND) return false;
        ec = std::error_code(static_cast<int>(err), std::system_category());
        return false;
    #else
        if (is_directory(p, ec)) {
            if (rmdir(p.c_str()) == 0) return true;
        }
        else {
            if (unlink(p.c_str()) == 0) return true;
        }
        if (errno == ENOENT) return false;
        ec = std::error_code(errno, std::system_category());
        return false;
    #endif
    }

    bool remove(const path& p) noexcept
    {
        std::error_code ec;
        return remove(p, ec);
    }

    uintmax_t remove_all(const path& p) noexcept
    {
        if (!exists(p)) return 0;

        uintmax_t count = 0;

        if (is_directory(p)) {
            // 递归删除目录内容
            for (directory_iterator it(p); it != directory_iterator(); ++it) {
                count += remove_all(it->path());
            }
            // 删除空目录
            if (remove(p)) {
                count++;
            }
        }
        else {
            // 删除文件
            if (remove(p)) {
                count++;
            }
        }

        return count;
    }

    // 文件操作实现
    // 在非Windows平台上使用Linux系统调用
    // 文件操作实现
    bool copy_file(const path& from, const path& to) noexcept
    {
    #ifdef _WIN32
        std::wstring wide_from, wide_to;
        aUtf8ToWide(from.c_str(), wide_from);  // 转换为宽字符
        aUtf8ToWide(to.c_str(), wide_to);      // 转换为宽字符
        return CopyFileW(wide_from.c_str(), wide_to.c_str(), FALSE) != 0;
    #else
        // 使用Linux系统调用实现文件复制
        int source_fd = -1, dest_fd = -1;
        const size_t buffer_size = 8192;  // 8KB缓冲区
        char buffer[buffer_size];
        ssize_t bytes_read, bytes_written;
        bool success = false;
        
        // 打开源文件，只读模式
        source_fd = open(from.c_str(), O_RDONLY);
        if (source_fd < 0) {
            return false;  // 源文件打开失败
        }
        
        // 创建或截断目标文件，读写模式
        // O_WRONLY | O_CREAT | O_TRUNC：只写模式，文件不存在则创建，存在则截断
        // 0644：文件权限 - 所有者读写，其他用户只读
        dest_fd = open(to.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (dest_fd < 0) {
            close(source_fd);
            return false;  // 目标文件创建失败
        }
        
        // 循环读取源文件内容并写入目标文件
        while ((bytes_read = read(source_fd, buffer, buffer_size)) > 0) {
            bytes_written = write(dest_fd, buffer, bytes_read);
            
            // 检查写入是否成功，且写入的字节数与读取的字节数相同
            if (bytes_written != bytes_read) {
                goto cleanup;  // 写入失败，跳转到清理代码
            }
        }
        
        // 检查读取过程是否正常结束
        if (bytes_read == 0) {
            success = true;  // 复制成功
        }
        
    cleanup:
        // 关闭文件描述符
        if (source_fd >= 0) {
            close(source_fd);
        }
        if (dest_fd >= 0) {
            close(dest_fd);
        }
        
        return success;
    #endif
    }

    bool rename(const path& old_p, const path& new_p) noexcept
    {
    #ifdef _WIN32
        std::wstring wide_old, wide_new;
        aUtf8ToWide(old_p.c_str(), wide_old);  // 转换为宽字符
        aUtf8ToWide(new_p.c_str(), wide_new);  // 转换为宽字符
        return MoveFileW(wide_old.c_str(), wide_new.c_str()) != 0;
    #else
        return ::rename(old_p.c_str(), new_p.c_str()) == 0;
    #endif
    }

    // 空间信息实现
    space_info space(const path& p)
    {
        space_info info{};

    #ifdef _WIN32
        std::wstring wide_path;
        aUtf8ToWide(p.c_str(), wide_path);  // 转换为宽字符
        ULARGE_INTEGER free, total, avail;
        if (GetDiskFreeSpaceExW(wide_path.c_str(), &avail, &total, &free)) {
            info.capacity = total.QuadPart;
            info.free = free.QuadPart;
            info.available = avail.QuadPart;
        }
    #else
        struct statvfs buf;
        if (statvfs(p.c_str(), &buf) == 0) {
            info.capacity = buf.f_blocks * buf.f_frsize;
            info.free = buf.f_bfree * buf.f_frsize;
            info.available = buf.f_bavail * buf.f_frsize;
        }
    #endif

        return info;
    }

    // 最后修改时间实现
    std::time_t last_write_time(const path& p)
    {
    #ifdef _WIN32
        std::wstring wide_path;
        aUtf8ToWide(p.c_str(), wide_path);  // 转换为宽字符
        HANDLE hFile = CreateFileW(wide_path.c_str(), GENERIC_READ, FILE_SHARE_READ,
            NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
        if (hFile == INVALID_HANDLE_VALUE) {
            return static_cast<std::time_t>(-1);
        }

        FILETIME ftWrite;
        if (!GetFileTime(hFile, NULL, NULL, &ftWrite)) {
            CloseHandle(hFile);
            return static_cast<std::time_t>(-1);
        }

        CloseHandle(hFile);

        // 将 FILETIME 转换为 time_t
        ULARGE_INTEGER ull;
        ull.LowPart = ftWrite.dwLowDateTime;
        ull.HighPart = ftWrite.dwHighDateTime;

        return static_cast<std::time_t>((ull.QuadPart - 116444736000000000ULL) / 10000000ULL);
    #else
        struct stat sb;
        if (stat(p.c_str(), &sb) == 0) {
            return sb.st_mtime;
        }
        return static_cast<std::time_t>(-1);
    #endif
    }

    path current_path()
    {
    #ifdef _WIN32
        // Windows 实现 - 使用宽字符 API
        DWORD size = GetCurrentDirectoryW(0, nullptr);
        if (size == 0) {
            throw filesystem_error("Failed to get current directory size");
        }

        std::vector<wchar_t> buffer(size);
        DWORD result = GetCurrentDirectoryW(size, buffer.data());
        if (result == 0 || result > size) {
            throw filesystem_error("Failed to get current directory");
        }

        std::string utf8_path;
        aWideToUtf8(buffer.data(), utf8_path);  // 转换为UTF8
        return path(utf8_path);
    #else
        // POSIX 实现
        char buffer[PATH_MAX];
        if (getcwd(buffer, sizeof(buffer)) == nullptr) {
            throw filesystem_error("Failed to get current working directory");
        }
        return path(buffer);
    #endif
    }

    path current_path(std::error_code& ec) noexcept
    {
        ec.clear();
        try {
            return current_path();
        }
        catch (const filesystem_error& ) {
            ec = std::make_error_code(std::errc::io_error);
            return path();
        }
        catch (...) {
            ec = std::make_error_code(std::errc::io_error);
            return path();
        }
    }

    void current_path(const path& new_path)
    {
    #ifdef _WIN32
        std::wstring wide_path;
        aUtf8ToWide(new_path.c_str(), wide_path);  // 转换为宽字符
        if (!SetCurrentDirectoryW(wide_path.c_str())) {
            throw filesystem_error("Failed to set current directory");
        }
    #else
        if (chdir(new_path.c_str()) != 0) {
            throw filesystem_error("Failed to set current directory");
        }
    #endif
    }

    void current_path(const path& new_path, std::error_code& ec) noexcept
    {
        ec.clear();
        try {
            current_path(new_path);
        }
        catch (const filesystem_error& ) {
            ec = std::make_error_code(std::errc::io_error);
        }
        catch (...) {
            ec = std::make_error_code(std::errc::io_error);
        }
    }


// ================================================================
// relative / lexically_relative（对标 std::filesystem）
// ================================================================

namespace
{
    // 分离根名称（如 "C:"）和根目录（如 "\"）
    void split_root(const std::string& p, std::string& rootName, std::string& rootDir)
    {
        rootName.clear();
        rootDir.clear();

        if (p.empty()) return;

#ifdef _WIN32
        // UNC: \\server\share
        if (p.size() >= 2 && path::is_separator(p[0]) && path::is_separator(p[1]))
        {
            size_t pos = p.find_first_of(path::separators(), 2);
            if (pos != std::string::npos)
            {
                pos = p.find_first_of(path::separators(), pos + 1);
                if (pos != std::string::npos)
                {
                    rootName = p.substr(0, pos);
                    rootDir = p.substr(pos, 1);
                    return;
                }
            }
            rootName = p; // 不完整的 UNC，整个当作根名
            return;
        }

        // 盘符: C:
        if (p.size() >= 2 && p[1] == ':')
        {
            rootName = p.substr(0, 2);
            if (p.size() > 2 && path::is_separator(p[2]))
                rootDir = p.substr(2, 1);
            return;
        }
#endif

        // POSIX 或 Windows 无盘符根
        if (path::is_separator(p[0]))
        {
            rootDir = p.substr(0, 1);
            return;
        }
    }

    // 将路径拆分为组件列表（不含根）
    std::vector<std::string> split_components(const std::string& p, size_t offset)
    {
        std::vector<std::string> comps;
        size_t i = offset;
        while (i < p.size())
        {
            // 跳过连续分隔符
            while (i < p.size() && path::is_separator(p[i])) ++i;
            if (i >= p.size()) break;

            size_t start = i;
            while (i < p.size() && !path::is_separator(p[i])) ++i;
            std::string comp = p.substr(start, i - start);
            if (comp == ".") continue; // 忽略单点
            comps.push_back(comp);
        }
        return comps;
    }
}

path path::lexically_relative(const path& base) const
{
    using std::string;

    const string& pNative = native();
    const string& bNative = base.native();

    // 分离根
    string pRootName, pRootDir, bRootName, bRootDir;
    split_root(pNative, pRootName, pRootDir);
    split_root(bNative, bRootName, bRootDir);

    // 根不同 → 无法计算相对路径，返回空
    if (pRootName != bRootName || pRootDir != bRootDir)
        return path();

    // 拆分组件
    size_t pOff = (pRootName + pRootDir).size();
    size_t bOff = (bRootName + bRootDir).size();
    auto pComps = split_components(pNative, pOff);
    auto bComps = split_components(bNative, bOff);

    // 找公共前缀
    size_t common = 0;
    while (common < pComps.size() && common < bComps.size() && pComps[common] == bComps[common])
        ++common;

    // 构建相对路径
    string result;
    for (size_t i = common; i < bComps.size(); ++i)
    {
        if (!result.empty()) result += preferred_separator();
        result += "..";
    }
    for (size_t i = common; i < pComps.size(); ++i)
    {
        if (!result.empty()) result += preferred_separator();
        result += pComps[i];
    }

    return result;
}

path relative(const path& p, const path& base, std::error_code& ec) noexcept
{
    ec.clear();

    // 不允许相对路径作为参数（对标 std::filesystem）
    if (p.is_relative() || base.is_relative())
    {
        ec = std::make_error_code(std::errc::invalid_argument);
        return path();
    }

    // 检查根是否匹配
    std::string pRootName, pRootDir, bRootName, bRootDir;
    split_root(p.native(), pRootName, pRootDir);
    split_root(base.native(), bRootName, bRootDir);

    if (pRootName != bRootName || pRootDir != bRootDir)
    {
        ec = std::make_error_code(std::errc::invalid_argument);
        return path();
    }

    return p.lexically_relative(base);
}

} // namespace simple_fs




AST_NAMESPACE_END