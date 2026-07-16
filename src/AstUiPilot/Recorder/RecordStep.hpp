///
/// @file      RecordStep.hpp
/// @brief     录制步骤数据结构
/// @author    axel
/// @date      2026-06-12
/// @copyright 版权所有 (C) 2026-present, SpaceAST项目.

#pragma once

#include "AstGlobal.h"
#include <string>
#include <cstdint>

AST_NAMESPACE_BEGIN

/*!
    @addtogroup UiPilot
    @{
*/

/// @brief 录制操作类型
enum class ERecordAction
{
    eClick,             // 点击按钮/控件
    eDblClick,          // 双击
    eFill,              // 填入文本（替换输入框内容）
    eSelect,            // 选择下拉框/列表项
    ePressKey,          // 按键
    eShortcut,          // 组合快捷键
    eDrag,              // 拖拽
    eDialogAccept,      // 接受对话框
    eDialogDismiss,     // 关闭对话框
};

/// @brief 操作类型 → JSON 字符串
inline const char* recordActionToString(ERecordAction action)
{
    switch (action)
    {
    case ERecordAction::eClick:         return "eClick";
    case ERecordAction::eDblClick:      return "eDblClick";
    case ERecordAction::eFill:          return "eFill";
    case ERecordAction::eSelect:        return "eSelect";
    case ERecordAction::ePressKey:      return "ePressKey";
    case ERecordAction::eShortcut:      return "eShortcut";
    case ERecordAction::eDrag:          return "eDrag";
    case ERecordAction::eDialogAccept:  return "eDialogAccept";
    case ERecordAction::eDialogDismiss: return "eDialogDismiss";
    }
    return "";
}

/// @brief JSON 字符串 → 操作类型
inline ERecordAction recordActionFromString(const std::string& s)
{
    if (s == "eClick")         return ERecordAction::eClick;
    if (s == "eDblClick")      return ERecordAction::eDblClick;
    if (s == "eFill")          return ERecordAction::eFill;
    if (s == "eSelect")        return ERecordAction::eSelect;
    if (s == "ePressKey")      return ERecordAction::ePressKey;
    if (s == "eShortcut")      return ERecordAction::eShortcut;
    if (s == "eDrag")          return ERecordAction::eDrag;
    if (s == "eDialogAccept")  return ERecordAction::eDialogAccept;
    if (s == "eDialogDismiss") return ERecordAction::eDialogDismiss;
    return ERecordAction::eClick; // fallback
}

/// @brief 录制步骤结构
struct RecordStep
{
    ERecordAction action{};           ///< 操作类型
    std::string   widgetClass{};      ///< 控件类名
    std::string   widgetText{};       ///< 按钮文本 / 标签文本 / combo当前项
    std::string   widgetObjName{};    ///< Qt objectName
    std::string   widgetAccName{};    ///< accessibleName
    std::string   parentInfo{};       ///< 父控件描述
    std::string   value{};            ///< fill的值 / select的选项 / press的键名
    int64_t       timestampMs = 0;  ///< 相对录制开始的时间偏移
    int           siblingIndex = -1;///< 同级同类控件中的序号
    std::string   naturalLanguage{};  ///< 自然语言描述

    /// @brief 生成模板描述
    std::string toTemplate() const;
};

/*! @} */

AST_NAMESPACE_END
