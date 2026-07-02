///
/// @file      CrashHandler.hpp
/// @brief     崩溃捕获与记录
/// @details   安装信号处理器和异常过滤器，在程序崩溃时生成诊断报告。
/// @author    axel
/// @date      1.7.2026
/// @copyright 版权所有 (C) 2026-present, ast项目.
///
/// ast项目（https://github.com/space-ast/ast）
/// 本项目基于 Apache 2.0 开源许可证分发。
/// 您可在遵守许可证条款的前提下使用、修改和分发本软件。
/// 许可证全文请见：
///
///    http://www.apache.org/licenses/LICENSE-2.0
///
/// 重要须知：
/// 软件按"现有状态"提供，无任何明示或暗示的担保条件。
/// 除非法律要求或书面同意，作者与贡献者不承担任何责任。
/// 使用本软件所产生的风险，需由您自行承担。

#pragma once

#include "AstGlobal.h"

AST_NAMESPACE_BEGIN

/*!
    @addtogroup Fault
    @{
*/

/// @brief 崩溃回调函数类型
/// @param crashType  崩溃类型字符串（如 "SIGSEGV"、"SIGABRT"、"TERMINATE"）
/// @param signalNum  信号编号（terminate 时为 0）
/// @param faultAddr  故障地址（不可用时为 nullptr）
/// @param stackFrames 调用栈原始返回地址数组
/// @param frameCount  调用栈帧数
/// @details 在崩溃日志写入文件之前调用。回调中不应执行复杂操作（如内存分配）。
typedef void (*ACrashCallback)(const char* crashType, int signalNum,
                                const void* faultAddr,
                                void* const* stackFrames, int frameCount);

/// @brief 安装崩溃处理器
/// @param callback  可选的用户回调，在写入崩溃日志前调用
/// @return eNoError 成功，eError 其他错误
/// @details
/// 注册以下处理器：
/// - Linux: SIGSEGV、SIGABRT、SIGFPE、SIGILL 的 sigaction 处理器
/// - Windows: 未处理异常过滤器（SEH）和 SIGABRT 信号处理器
/// - 跨平台: std::terminate 处理器（未捕获的 C++ 异常）
///
/// 重复安装是安全的（第二次调用无操作）。
AST_FAULT_CAPI errc_t aCrashHandlerInstall(ACrashCallback callback);

/// @brief 卸载崩溃处理器
/// @details 将所有处理器恢复为默认行为。重复卸载是安全的。
AST_FAULT_CAPI void aCrashHandlerUninstall();

/// @brief 设置崩溃日志输出目录
/// @param dir  目录路径（内部会复制到固定大小的缓冲区，最大 511 字符）
/// @return eNoError 成功，eError 路径过长
/// @details 默认目录为 aDataDir() + "/crash/"。如指定目录不存在，将尝试创建。
AST_FAULT_CAPI errc_t aCrashHandlerSetLogDir(const char* dir);

/// @brief 获取最后一次崩溃日志的完整路径
/// @return 日志文件路径（尚未发生崩溃时返回空字符串）
AST_FAULT_API const char* aCrashHandlerLastLogPath();

/// @brief 解析崩溃日志中的堆栈地址，附加符号信息
/// @param logPath  原始崩溃日志文件路径
/// @param outPath  输出文件路径（为 nullptr 时覆盖原文件）
/// @return eNoError 成功，eError 打开文件失败或格式错误
/// @details
/// 在正常上下文中调用（非信号处理器），解析原始地址的符号信息：
/// - Windows: 使用 SymFromAddr + SymGetLineFromAddr64 (DbgHelp)
/// - Linux: 使用 dladdr + addr2line
///
/// 将 "#NN 0x..." 行替换为 "#NN 0x...  functionName  [file.cpp:line]" 格式。
/// 需要调试符号（.pdb / 未strip的 .so）可用时才能解析到文件名和行号。
AST_FAULT_CAPI errc_t aCrashResolveSymbols(const char* logPath, const char* outPath);

/// @brief 设置崩溃后的报告命令
/// @param command  崩溃后执行的命令（如崩溃报告程序路径）
/// @details
/// 崩溃处理器写完日志后，将调用此命令并传入日志路径作为参数。
/// 设为 nullptr 则使用默认行为（Windows: MessageBox，Linux: stderr）。
///
/// 命令格式: "command --flag" — 日志路径会作为最后一个参数追加。
/// 仅在程序启动时（非信号上下文中）调用此函数。
AST_FAULT_CAPI errc_t aCrashSetReporterCommand(const char* command);

/// @brief 获取当前设置的报告命令
/// @return 报告命令字符串（未设置时返回空字符串）
AST_FAULT_API const char* aCrashGetReporterCommand();

/*! @} */

AST_NAMESPACE_END
