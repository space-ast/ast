///
/// @file      PilotPlayer.cpp
/// @brief     录制脚本回放器实现
/// @author    axel
/// @date      2026-06-12
/// @copyright 版权所有 (C) 2026-present, SpaceAST项目.

#include "PilotPlayer.hpp"
#include "AstUiPilot/PilotSession.hpp"
#include "AstUiPilot/RecordStep.hpp"
#include "AstUtil/JsonValue.hpp"
#include "AstUtil/IO.hpp"
#include <QApplication>
#include <QTimer>
#include <sstream>

AST_NAMESPACE_BEGIN

// ============================================================
//  构造 / 析构
// ============================================================

PilotPlayer::PilotPlayer(PilotSession* session, QObject* parent)
    : QObject(parent)
    , session_(session)
{
}

PilotPlayer::~PilotPlayer()
{
    if (running_) stop();
}

// ============================================================
//  加载脚本
// ============================================================

bool PilotPlayer::loadScript(const std::string& filePath)
{
    JsonValue json = JsonValue::FromFile(filePath);
    if (json.isNull()) return false;

    steps_.clear();

    auto& stepsArr = json["steps"];
    if (!stepsArr.isArray()) return false;

    for (auto& obj : stepsArr.getArray())
    {
        RecordStep s;
        s.action = recordActionFromString(obj["action"].toString());

        if (!obj["widgetClass"].isNull())
            s.widgetClass = obj["widgetClass"].toString();
        if (!obj["widgetText"].isNull())
            s.widgetText = obj["widgetText"].toString();
        if (!obj["widgetObjName"].isNull())
            s.widgetObjName = obj["widgetObjName"].toString();
        if (!obj["widgetAccName"].isNull())
            s.widgetAccName = obj["widgetAccName"].toString();
        if (!obj["parentInfo"].isNull())
            s.parentInfo = obj["parentInfo"].toString();
        if (!obj["value"].isNull())
            s.value = obj["value"].toString();

        // 优先使用润色版，否则用模板版
        if (!obj["natural"].isNull())
            s.naturalLanguage = obj["natural"].toString();
        else if (!obj["template"].isNull())
            s.naturalLanguage = obj["template"].toString();
        else
            s.naturalLanguage = s.toTemplate();

        steps_.push_back(s);
    }

    return true;
}

bool PilotPlayer::loadJson(const std::string& jsonStr)
{
    JsonValue json = JsonValue::FromString(jsonStr);
    if (json.isNull()) return false;

    steps_.clear();

    auto& stepsArr = json["steps"];
    if (!stepsArr.isArray()) return false;

    for (auto& obj : stepsArr.getArray())
    {
        RecordStep s;
        s.action = recordActionFromString(obj["action"].toString());

        if (!obj["widgetClass"].isNull())
            s.widgetClass = obj["widgetClass"].toString();
        if (!obj["widgetText"].isNull())
            s.widgetText = obj["widgetText"].toString();
        if (!obj["widgetObjName"].isNull())
            s.widgetObjName = obj["widgetObjName"].toString();
        if (!obj["widgetAccName"].isNull())
            s.widgetAccName = obj["widgetAccName"].toString();
        if (!obj["parentInfo"].isNull())
            s.parentInfo = obj["parentInfo"].toString();
        if (!obj["value"].isNull())
            s.value = obj["value"].toString();

        if (!obj["natural"].isNull())
            s.naturalLanguage = obj["natural"].toString();
        else if (!obj["template"].isNull())
            s.naturalLanguage = obj["template"].toString();
        else
            s.naturalLanguage = s.toTemplate();

        steps_.push_back(s);
    }

    return true;
}

// ============================================================
//  play / pause / resume / step / stop
// ============================================================

void PilotPlayer::play(EPlayMode mode)
{
    if (running_) return;

    if (steps_.empty()) return;

    running_ = true;
    paused_ = false;
    currentIndex_ = -1;

    if (mode == EPlayMode::Sequential)
        executeNextStep();
}

void PilotPlayer::pause()
{
    paused_ = true;
    emit playbackPaused();
}

void PilotPlayer::resume()
{
    if (!paused_) return;
    paused_ = false;
    executeNextStep();
}

void PilotPlayer::step()
{
    if (!running_)
    {
        running_ = true;
        currentIndex_ = -1;
    }
    executeNextStep();
}

void PilotPlayer::stop()
{
    running_ = false;
    paused_ = false;
}

// ============================================================
//  executeNextStep / executeStep
// ============================================================

void PilotPlayer::executeNextStep()
{
    if (!running_ || paused_)
        return;

    currentIndex_++;

    if (currentIndex_ >= static_cast<int>(steps_.size()))
    {
        running_ = false;
        emit playbackFinished();
        return;
    }

    executeStep(currentIndex_);
}

void PilotPlayer::executeStep(int index)
{
    const auto& s = steps_[index];

    emit stepStarted(index, s.naturalLanguage);

    // 构造上下文：快照 + 步骤描述
    std::ostringstream ctx;
    ctx << u8"[操作指令]\n"
        << u8"请执行以下步骤：\n"
        << s.naturalLanguage << "\n\n"
        << u8"先调用 snapshot() 查看界面，然后定位控件并执行操作。";

    QString resultStr;

    // 同步执行（在 Player 线程）
    std::string response = session_->execute(ctx.str());

    // 判断成功/失败
    if (response.find("✗") != std::string::npos
        || response.find("失败") != std::string::npos
        || response.find("找不到") != std::string::npos
        || response.find("未找到") != std::string::npos)
    {
        // 失败 → 重试
        if (index == currentIndex_) // 确保没有被 stop
        {
            retryStep(index, 2);
        }
    }
    else
    {
        emit stepCompleted(index, response);
        // 继续下一步（延迟，让 UI 有时间更新）
        if (running_ && !paused_)
        {
            QTimer::singleShot(300, this, [this]() {
                continuePlayback();
            });
        }
    }
}

void PilotPlayer::continuePlayback()
{
    if (running_ && !paused_)
        executeNextStep();
}

// ============================================================
//  retryStep
// ============================================================

void PilotPlayer::retryStep(int index, int maxRetries)
{
    for (int retry = 0; retry < maxRetries; retry++)
    {
        if (!running_) return;

        // 等待 UI 稳定后重试
        QApplication::processEvents();

        const auto& s = steps_[index];

        // 重试提示
        std::ostringstream ctx;
        ctx << u8"[重试操作]\n"
            << u8"上一次执行失败了，请重新获取 snapshot() 后再次尝试：\n"
            << s.naturalLanguage;

        std::string response = session_->execute(ctx.str());

        if (response.find("✗") == std::string::npos
            && response.find("失败") == std::string::npos
            && response.find("找不到") == std::string::npos)
        {
            emit stepCompleted(index, response);
            if (running_ && !paused_)
            {
                QTimer::singleShot(300, this, [this]() {
                    continuePlayback();
                });
            }
            return;
        }
    }

    // 所有重试失败
    lastError_ = "步骤 " + std::to_string(index + 1) + " 失败: "
               + steps_[index].naturalLanguage;
    emit stepFailed(index, lastError_);
    paused_ = true;
}

// ============================================================
//  progress
// ============================================================

std::string PilotPlayer::progress() const
{
    std::ostringstream oss;
    oss << (currentIndex_ + 1) << "/" << steps_.size();
    return oss.str();
}

AST_NAMESPACE_END
