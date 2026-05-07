///
/// @file      Sequence.hpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-04-15
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

#pragma once

#include "AstGlobal.h"
#include "MissionCommand.hpp"
#include "Segment.hpp"
#include "AstCore/ScriptingToolProfile.hpp"

AST_NAMESPACE_BEGIN

/*!
    @addtogroup 
    @{
*/


/*
    MCS 的全程是 Mission Command Sequence (任务命令序列) 或者 Mission Control Sequence (任务控制序列)
*/


/// @brief 任务序列段，内部包含多个任务命令或轨道段，并会按顺序执行
class AST_CORE_API Sequence: public Segment
{
public:
    AST_OBJECT(Sequence)
    Sequence() = default;
    ~Sequence() = default;
public:
    errc_t execute() override;
    void setCommands(const std::vector<HMissionCommand>& commands);
    void setCommands(std::vector<HMissionCommand>&& commands);
    const std::vector<HMissionCommand>& getCommands() const;

    ScriptingToolProfile* scriptingTool() const{return scriptingTool_.get();}
    void setScriptingTool(ScriptingToolProfile* tool){scriptingTool_ = tool;}

    /// @brief 根据路径获取子段，例如"a.b.c"
    /// @param path 路径
    /// @return Segment* 子段指针
    Segment* getSegmentByPath(StringView path);
    MissionCommand* getCommandByPath(StringView path);

    /// @brief 根据名称获取子段
    /// @param name 子段名称
    /// @return Segment* 子段指针
    Segment* getSegmentByName(StringView name);
    MissionCommand* getCommandByName(StringView name);

    /// @brief 获取重复次数
    /// @return int 重复次数
    int repeatCount() const{return repeatCount_;}
    void setRepeatCount(int count){repeatCount_ = count;}
protected:
    /// @brief 链接任务命令
    void linkCommands();
private:
    int repeatCount_{1};                                    ///< 重复次数
    std::vector<HMissionCommand> commands_;                 ///< 任务命令序列
    SharedPtr<ScriptingToolProfile> scriptingTool_;         ///< 脚本工具
};


/*! @} */

AST_NAMESPACE_END
