///
/// @file      PilotPipeServer.cpp
/// @brief     Named Pipe 服务端实现
/// @author    axel
/// @date      2026-06-13
/// @copyright 版权所有 (C) 2026-present, SpaceAST项目.

#include "PilotPipeServer.hpp"
#include "PilotCommander.hpp"
#include <sstream>

#ifdef _WIN32
#include <windows.h>
#else
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#endif

AST_NAMESPACE_BEGIN

// ---- 构造 / 析构 ----

PilotPipeServer::PilotPipeServer(PilotCommander* commander, unsigned long pid)
    : commander_(commander)
    , pid_(pid)
{
}

PilotPipeServer::~PilotPipeServer()
{
    stop();
}

std::string PilotPipeServer::pipeName(unsigned long pid)
{
    std::ostringstream oss;
    oss << "\\\\.\\pipe\\AstUiPilot_" << pid;
    return oss.str();
}

// ---- start / stop ----

void PilotPipeServer::start()
{
    if (running_) return;
    running_ = true;
    serverThread_ = std::thread(&PilotPipeServer::serverLoop, this);
}

void PilotPipeServer::stop()
{
    running_ = false;
    if (serverThread_.joinable())
        serverThread_.join();
}

// ---- 服务线程 ----

#ifdef _WIN32

void PilotPipeServer::serverLoop()
{
    std::string pipeNameStr = pipeName(pid_);

    while (running_)
    {
        // 创建 named pipe 实例
        HANDLE hPipe = CreateNamedPipeA(
            pipeNameStr.c_str(),
            PIPE_ACCESS_DUPLEX,          // 双向
            PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
            1,                            // 最多 1 个实例
            4096,                         // 输出缓冲区
            4096,                         // 输入缓冲区
            0,                            // 默认超时
            NULL                          // 默认安全属性
        );

        if (hPipe == INVALID_HANDLE_VALUE)
        {
            // 创建失败，等待后重试
            Sleep(1000);
            continue;
        }

        // 等待客户端连接
        BOOL connected = ConnectNamedPipe(hPipe, NULL);
        if (!connected && GetLastError() != ERROR_PIPE_CONNECTED)
        {
            CloseHandle(hPipe);
            Sleep(500);
            continue;
        }

        // 客户端已连接 —— 读命令 / 写响应循环
        char buf[8192];
        DWORD bytesRead;
        BOOL success;

        while (running_)
        {
            // 读取一行命令
            success = ReadFile(hPipe, buf, sizeof(buf) - 1, &bytesRead, NULL);
            if (!success || bytesRead == 0)
                break;  // 客户端断开

            buf[bytesRead] = '\0';
            std::string command(buf, bytesRead);

            // 去除末尾的 \r\n
            while (!command.empty() && (command.back() == '\n' || command.back() == '\r'))
                command.pop_back();

            if (command.empty())
                continue;

            // 执行命令
            std::string response = commander_->execute(command);

            // 发送响应 + \0 结束符
            response.push_back('\0');
            DWORD bytesWritten;
            success = WriteFile(hPipe, response.data(),
                                static_cast<DWORD>(response.size()),
                                &bytesWritten, NULL);
            if (!success)
                break;
        }

        // 断开当前客户端
        FlushFileBuffers(hPipe);
        DisconnectNamedPipe(hPipe);
        CloseHandle(hPipe);

        // 短暂等待后重新等待下一个连接
        Sleep(100);
    }
}

#else
// Linux / macOS: 使用 Unix domain socket（暂未实现，预留接口）
void PilotPipeServer::serverLoop()
{
    // TODO: Unix domain socket 实现
    while (running_)
    {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}
#endif

AST_NAMESPACE_END
