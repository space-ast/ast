#include "SimpleFileSystem.hpp"
#include <algorithm>
#include <cstring>

#ifdef _WIN32
#include <windows.h>
#include <fileapi.h>
#include <direct.h>
#include <shlwapi.h>
#pragma comment(lib, "shlwapi.lib")
#else
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/statvfs.h>
#include <dirent.h>
#include <unistd.h>
#endif


AST_NAMESPACE_BEGIN

namespace simple_fs
{

    // directory_iterator 的实现
#ifdef _WIN32
    struct directory_iterator::impl
    {
        HANDLE handle;
        WIN32_FIND_DATAA data;  // 使用宽字符版本
        bool is_end;

        impl(const path& p) : handle(INVALID_HANDLE_VALUE), is_end(false)
        {
            // 使用平台相关的字符串类型
            string_type search_path = (p / "*").native();
            handle = FindFirstFileA(search_path.c_str(), &data);
            if (handle == INVALID_HANDLE_VALUE) {
                is_end = true;
            }
        }

        ~impl()
        {
            if (handle != INVALID_HANDLE_VALUE) {
                FindClose(handle);
            }
        }
    };
#else
    struct directory_iterator::impl
    {
        DIR* dir;
        struct dirent* entry;

        impl(const path& p) : dir(nullptr), entry(nullptr)
        {
            dir = opendir(p.c_str());
            if (dir) {
                entry = readdir(dir);
            }
        }

        ~impl()
        {
            if (dir) {
                closedir(dir);
            }
        }
    };
#endif

    directory_iterator::directory_iterator(const path& p) : impl_(new impl(p))
    {
        if (impl_) {
        #ifdef _WIN32
            if (!impl_->is_end) {
                // 使用宽字符文件名创建path对象
                entry_ = directory_entry(path(impl_->data.cFileName));
                // 跳过 "." 和 ".."
                while (!impl_->is_end &&
                    (strcmp(impl_->data.cFileName, ".") == 0 ||
                        strcmp(impl_->data.cFileName, "..") == 0)) {
                    read_next_entry();
                }
            }
        #else
            if (impl_->entry) {
                entry_ = directory_entry(path(impl_->entry->d_name));
                // 跳过 "." 和 ".."
                while (impl_->entry &&
                    (strcmp(impl_->entry->d_name, ".") == 0 ||
                        strcmp(impl_->entry->d_name, "..") == 0)) {
                    read_next_entry();
                }
            }
        #endif
        }
    }

    directory_iterator::~directory_iterator()
    {
        delete impl_;
    }

    directory_iterator::directory_iterator(directory_iterator&& other) noexcept
        : impl_(other.impl_), entry_(std::move(other.entry_))
    {
        other.impl_ = nullptr;
    }

    directory_iterator& directory_iterator::operator=(directory_iterator&& other) noexcept
    {
        if (this != &other) {
            delete impl_;
            impl_ = other.impl_;
            entry_ = std::move(other.entry_);
            other.impl_ = nullptr;
        }
        return *this;
    }

    void directory_iterator::read_next_entry()
    {
    #ifdef _WIN32
        if (impl_ && !impl_->is_end) {
            if (FindNextFileA(impl_->handle, &impl_->data)) {
                entry_ = directory_entry(path(impl_->data.cFileName));
            }
            else {
                impl_->is_end = true;
                entry_ = directory_entry();
            }
        }
    #else
        if (impl_ && impl_->dir) {
            impl_->entry = readdir(impl_->dir);
            if (impl_->entry) {
                entry_ = directory_entry(path(impl_->entry->d_name));
            }
            else {
                entry_ = directory_entry();
            }
        }
    #endif
    }

    directory_iterator& directory_iterator::operator++()
    {
        if (impl_) {
            read_next_entry();
        #ifdef _WIN32
            // 在Windows上跳过 "." 和 ".."
            while (!impl_->is_end &&
                (strcmp(impl_->data.cFileName, ".") == 0 ||
                    strcmp(impl_->data.cFileName, "..") == 0)) {
                read_next_entry();
            }
        #else
            // 在Unix上跳过 "." 和 ".."
            while (impl_->entry &&
                (strcmp(impl_->entry->d_name, ".") == 0 ||
                    strcmp(impl_->entry->d_name, "..") == 0)) {
                read_next_entry();
            }
        #endif
        }
        return *this;
    }

    // 文件状态实现
    file_status directory_entry::status() const
    {
        return simple_fs::status(path_);
    }

    // 基础文件操作实现
    bool exists(const path& p)
    {
    #ifdef _WIN32
        DWORD attrs = GetFileAttributesA(p.c_str());
        return (attrs != INVALID_FILE_ATTRIBUTES);
    #else
        struct stat sb;
        return (stat(p.c_str(), &sb) == 0);
    #endif
    }

    bool is_directory(const path& p)
    {
    #ifdef _WIN32
        DWORD attrs = GetFileAttributesA(p.c_str());
        return (attrs != INVALID_FILE_ATTRIBUTES) && (attrs & FILE_ATTRIBUTE_DIRECTORY);
    #else
        struct stat sb;
        if (stat(p.c_str(), &sb) == 0) {
            return S_ISDIR(sb.st_mode);
        }
        return false;
    #endif
    }

    bool is_regular_file(const path& p)
    {
    #ifdef _WIN32
        DWORD attrs = GetFileAttributesA(p.c_str());
        return (attrs != INVALID_FILE_ATTRIBUTES) && !(attrs & FILE_ATTRIBUTE_DIRECTORY);
    #else
        struct stat sb;
        if (stat(p.c_str(), &sb) == 0) {
            return S_ISREG(sb.st_mode);
        }
        return false;
    #endif
    }

    uintmax_t file_size(const path& p)
    {
    #ifdef _WIN32
        HANDLE hFile = CreateFileA(p.c_str(), GENERIC_READ, FILE_SHARE_READ,
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

    file_status status(const path& p)
    {
        if (!exists(p)) {
            return file_status(file_type::not_found);
        }

        if (is_directory(p)) {
            return file_status(file_type::directory);
        }

        if (is_regular_file(p)) {
            return file_status(file_type::regular);
        }

        return file_status(file_type::unknown);
    }

    // 目录操作实现
    bool create_directory(const path& p)
    {
    #ifdef _WIN32
        return CreateDirectoryA(p.c_str(), NULL) != 0;
    #else
        return mkdir(p.c_str(), 0755) == 0;
    #endif
    }

    bool create_directories(const path& p)
    {
        if (p.empty()) return false;

        // 如果目录已存在
        if (is_directory(p)) return true;

        // 创建父目录
        auto parent = p.parent_path();
        if (!parent.empty() && !exists(parent)) {
            if (!create_directories(parent)) {
                return false;
            }
        }

        // 创建当前目录
        return create_directory(p);
    }

    bool remove(const path& p)
    {
    #ifdef _WIN32
        if (is_directory(p)) {
            return RemoveDirectoryA(p.c_str()) != 0;
        }
        else {
            return DeleteFileA(p.c_str()) != 0;
        }
    #else
        if (is_directory(p)) {
            return rmdir(p.c_str()) == 0;
        }
        else {
            return unlink(p.c_str()) == 0;
        }
    #endif
    }

    uintmax_t remove_all(const path& p)
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
    bool copy_file(const path& from, const path& to)
    {
    #ifdef _WIN32
        return CopyFileA(from.c_str(), to.c_str(), FALSE) != 0;
    #else
        // 简化实现：使用系统命令
        std::string cmd = "cp \"" + from.string() + "\" \"" + to.string() + "\"";
        return system(cmd.c_str()) == 0;
    #endif
    }

    bool rename(const path& old_p, const path& new_p)
    {
    #ifdef _WIN32
        return MoveFileA(old_p.c_str(), new_p.c_str()) != 0;
    #else
        return ::rename(old_p.c_str(), new_p.c_str()) == 0;
    #endif
    }

    // 空间信息实现
    space_info space(const path& p)
    {
        space_info info{};

    #ifdef _WIN32
        ULARGE_INTEGER free, total, avail;
        if (GetDiskFreeSpaceExA(p.c_str(), &avail, &total, &free)) {
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
        HANDLE hFile = CreateFileA(p.c_str(), GENERIC_READ, FILE_SHARE_READ,
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
        // Windows 实现 - 直接使用宽字符 API
        DWORD size = GetCurrentDirectoryA(0, nullptr);
        if (size == 0) {
            throw filesystem_error("Failed to get current directory size");
        }

        std::vector<char> buffer(size);
        DWORD result = GetCurrentDirectoryA(size, buffer.data());
        if (result == 0 || result > size) {
            throw filesystem_error("Failed to get current directory");
        }

        return path(buffer.data());
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
        catch (const filesystem_error& e) {
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
        if (!SetCurrentDirectoryA(new_path.c_str())) {
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
        catch (const filesystem_error& e) {
            ec = std::make_error_code(std::errc::io_error);
        }
        catch (...) {
            ec = std::make_error_code(std::errc::io_error);
        }
    }


} // namespace simple_fs

#ifdef _WIN32   
void _aANSIToWide(const char* ansi, std::wstring& wide)
{
    int len = MultiByteToWideChar(CP_ACP, 0, ansi, -1, nullptr, 0);
    if (len > 0)
    {
        std::vector<wchar_t> buffer(len);
        MultiByteToWideChar(CP_ACP, 0, ansi, -1, buffer.data(), len);
        wide = buffer.data();
    }
}

void _aWideToANSI(const wchar_t* wide, std::string& ansi)
{
    int len = WideCharToMultiByte(CP_ACP, 0, wide, -1, nullptr, 0, nullptr, nullptr);
    if (len > 0)
    {
        std::vector<char> buffer(len);
        WideCharToMultiByte(CP_ACP, 0, wide, -1, buffer.data(), len, nullptr, nullptr);
        ansi = buffer.data();
    }
}
#endif



AST_NAMESPACE_END

