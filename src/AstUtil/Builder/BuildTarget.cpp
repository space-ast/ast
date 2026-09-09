///
/// @file      BuildTarget.cpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-03-30
/// @copyright 版权所有 (C) 2026-present, SpaceAST项目.
///
/// SpaceAST项目（https://github.com/space-ast/ast）
/// 本软件基于 Apache 2.0 开源许可证分发。
/// 您可在遵守许可证条款的前提下使用、修改和分发本软件。
/// 许可证全文请见：
/// 
///    http://www.apache.org/licenses/LICENSE-2.0
/// 
/// 重要须知：
/// 软件按"现有状态"提供，无任何明示或暗示的担保条件。
/// 除非法律要求或书面同意，作者与贡献者不承担任何责任。
/// 使用本软件所产生的风险，需由您自行承担。

#include "AstUtil/BuildTarget.hpp"
#include "AstUtil/StringView.hpp"
#include "AstUtil/Logger.hpp"
#include "BuildTarget.hpp"

AST_NAMESPACE_BEGIN

BuildTarget::BuildTarget(StringView name)
{
    setName(name);
}

BuildTarget& BuildTarget::setName(StringView name)
{
    name_ = std::string(name);
    return *this;
}

BuildTarget& BuildTarget::setKind(StringView kind)
{
    if(kind == "shared")
    {
        kind_ = EKind::eShared;
    }
    else if(kind == "static")
    {
        kind_ = EKind::eStatic;
    }
    else if(kind == "binary")
    {
        kind_ = EKind::eBinary;
    }
    return *this;
}

BuildTarget &BuildTarget::setKind(EKind kind)
{
    kind_ = kind;
    return *this;
}

BuildTarget& BuildTarget::addFiles(const std::vector<std::string> &files)
{
    files_.insert(files_.end(), files.begin(), files.end());
    return *this;
}

BuildTarget& BuildTarget::addIncludeDirs(const std::vector<std::string> &dirs)
{
    includeDirs_.insert(includeDirs_.end(), dirs.begin(), dirs.end());
    return *this;
}

BuildTarget& BuildTarget::addLinkDirs(const std::vector<std::string> &dirs)
{
    linkDirs_.insert(linkDirs_.end(), dirs.begin(), dirs.end());
    return *this;
}

BuildTarget& BuildTarget::addLinks(const std::vector<std::string> &libs)
{
    links_.insert(links_.end(), libs.begin(), libs.end());
    return *this;
}

BuildTarget& BuildTarget::addDefines(const std::vector<std::string> &defines)
{
    defines_.insert(defines_.end(), defines.begin(), defines.end());
    return *this;
}


errc_t BuildTarget::build()
{
    // 检查目标名称
    if (name_.empty()) {
        aError(_("名称为空"));
        return -1;
    }
    
    // 检查源文件列表
    if (files_.empty()) {
        aError(_("未指定源文件"));
        return -1;
    }
    
    // 构建编译命令
    std::string compileCmd = "g++ -c ";
    
    // 添加包含目录
    for (const auto& dir : includeDirs_) {
        compileCmd += "-I" + dir + " ";
    }
    
    // 添加宏定义
    for (const auto& define : defines_) {
        compileCmd += "-D" + define + " ";
    }
    
    // 添加源文件
    for (const auto& file : files_) {
        compileCmd += file + " ";
    }
    
    // 执行编译命令
    int compileResult = system(compileCmd.c_str());
    if (compileResult != 0) {
        aError(_("编译失败"));
        return -1;
    }
    
    // 构建链接命令
    std::string linkCmd = "g++ -o ";
    
    // 根据目标类型设置输出文件
    switch (kind_) {
    case EKind::eShared:
        linkCmd += "lib" + name_ + ".so ";
        linkCmd += "-shared ";
        break;
    case EKind::eStatic:
        linkCmd = "ar rcs lib" + name_ + ".a ";
        break;
    case EKind::eBinary:
#ifdef _WIN32
        linkCmd += name_ + ".exe ";
#else
        linkCmd += name_ + " ";
#endif
        break;
    }
    
    // 添加目标文件
    for (const auto& file : files_) {
        std::string objFile = file;
        size_t lastDot = objFile.rfind('.');
        if (lastDot != std::string::npos) {
            objFile = objFile.substr(0, lastDot) + ".o";
        } else {
            objFile += ".o";
        }
        linkCmd += objFile + " ";
    }
    
    // 添加链接目录
    for (const auto& dir : linkDirs_) {
        linkCmd += "-L" + dir + " ";
    }
    
    // 添加链接库
    for (const auto& lib : links_) {
        linkCmd += "-l" + lib + " ";
    }
    
    // 执行链接命令
    int linkResult = system(linkCmd.c_str());
    if (linkResult != 0) {
        aError(_("链接失败"));
        return -1;
    }
    
    return 0;
}

errc_t BuildTarget::run()
{
    // 检查目标类型是否为可执行文件
    if (kind_ != EKind::eBinary) {
        aError(_("只有 binary 类型的目标可以运行"));
        return -1;
    }
    
    // 检查目标名称
    if (name_.empty()) {
        aError(_("名称为空"));
        return -1;
    }
    
    // 构建运行命令
    std::string runCmd = "./" + name_;
    
    // 在 Windows 平台上使用不同的命令格式
#ifdef _WIN32
    runCmd = name_ + ".exe";
#endif
    
    // 执行运行命令
    int runResult = system(runCmd.c_str());
    if (runResult != 0) {
        aError(_("执行失败"));
        return -1;
    }
    
    return eNoError;
}

AST_NAMESPACE_END