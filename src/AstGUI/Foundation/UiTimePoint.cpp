///
/// @file      UiTimePoint.cpp
/// @brief     ~
/// @details   ~
/// @author    axel
/// @date      2026-03-28
/// @copyright 版权所有 (C) 2026-present, ast项目.
///
/// SpaceAST项目（https://github.com/space-ast/ast）
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

#include "UiTimePoint.hpp"
#include "AstCore/TimePoint.hpp"
#include "AstUtil/Logger.hpp"

AST_NAMESPACE_BEGIN

UiTimePoint::UiTimePoint(QWidget* parent) : UiValueEdit(parent)
{
    // 设置默认时间为当前时间
    TimePoint now{}; // = TimePoint::TodayUTC();
    setTimePoint(now);

    // 设置 tooltip 显示延迟为 0，鼠标靠近时马上出现
    setToolTipDuration(0);
    
    // 连接信号和槽
    connect(this, &UiValueEdit::editingFinished, this, &UiTimePoint::updateTimePoint);
}

void UiTimePoint::setTimePoint(const TimePoint& timePoint)
{
    timePoint_ = timePoint;
    // 将时间点转换为字符串并显示
    std::string timeStr = timePoint.toString();
    setText(QString::fromStdString(timeStr));
}

void UiTimePoint::updateTimePoint()
{
    // 从文本框获取时间字符串
    QString text = this->text();
    std::string timeStr = text.toUtf8().toStdString();
    
    // 解析时间字符串并更新时间点
    TimePoint tp{};
    errc_t rc = aTimePointParse(timeStr, tp);
    if(rc){
        aError("failed to parse time point '%s'", timeStr.c_str());
        // 显示错误提示
        setError(tr("时间格式错误，请输入正确的时间格式"));
    }else{
        // 解析成功，恢复默认样式
        setNormal();
        setTimePoint(tp);
        emit timePointChanged(tp);
    }
}

AST_NAMESPACE_END