///
/// @file      RecordStep.cpp
/// @brief     录制步骤模板翻译实现
/// @author    axel
/// @date      2026-06-12
/// @copyright 版权所有 (C) 2026-present, SpaceAST项目.

#include "RecordStep.hpp"

AST_NAMESPACE_BEGIN

std::string RecordStep::toTemplate() const
{
    switch (action)
    {
    case ERecordAction::eClick:
        {
            std::string label = widgetText.empty() ? widgetObjName : widgetText;
            if (!label.empty())
                return std::string("点击 ") + widgetClass + " \"" + label + "\"";
            return std::string("点击 ") + widgetClass;
        }
    case ERecordAction::eDblClick:
        {
            std::string label = widgetText.empty() ? widgetObjName : widgetText;
            if (!label.empty())
                return std::string("双击 ") + widgetClass + " \"" + label + "\"";
            return std::string("双击 ") + widgetClass;
        }
    case ERecordAction::eFill:
        {
            std::string label = widgetAccName.empty() ? widgetText : widgetAccName;
            if (label.empty()) label = widgetObjName;
            if (label.empty())
                return std::string("在输入框中填入 \"") + value + "\"";
            return std::string("在 \"") + label + "\" 输入框中填入 \"" + value + "\"";
        }
    case ERecordAction::eSelect:
        return std::string("在 \"") + widgetText + "\" 下拉框中选择 \"" + value + "\"";
    case ERecordAction::ePressKey:
        return std::string("按下 \"") + value + "\" 键";
    case ERecordAction::eShortcut:
        return std::string("使用快捷键 \"") + value + "\"";
    case ERecordAction::eDialogAccept:
        {
            if (!widgetText.empty())
                return std::string("在 \"") + widgetText + "\" 对话框中点击确定";
            return "点击对话框的确定按钮";
        }
    case ERecordAction::eDialogDismiss:
        {
            if (!widgetText.empty())
                return std::string("关闭 \"") + widgetText + "\" 对话框";
            return "关闭对话框";
        }
    case ERecordAction::eDrag:
        return std::string("将 \"") + widgetText + "\" 拖拽到 \"" + value + "\"";
    }
    return "";
}

AST_NAMESPACE_END
