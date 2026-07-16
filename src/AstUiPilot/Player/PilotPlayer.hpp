///
/// @file      PilotPlayer.hpp
/// @brief     录制脚本回放器
/// @author    axel
/// @date      2026-06-12
/// @copyright 版权所有 (C) 2026-present, SpaceAST项目.

#pragma once

#include "AstGlobal.h"
#include "AstUiPilot/RecordStep.hpp"
#include <QObject>
#include <vector>
#include <string>

AST_NAMESPACE_BEGIN

class PilotSession;

/*!
    @addtogroup UiPilot
    @{
*/

/// @brief 录制脚本回放器
/// @details 加载 JSON 脚本，逐步通过 LLM 执行自然语言描述的操作。
class AST_UIPILOT_API PilotPlayer : public QObject
{
    Q_OBJECT

public:
    enum class EPlayMode { Sequential, StepByStep };

    explicit PilotPlayer(PilotSession* session, QObject* parent = nullptr);
    ~PilotPlayer() override;
    PilotPlayer(const PilotPlayer&) = delete;
    PilotPlayer& operator=(const PilotPlayer&) = delete;

    // ---- 加载 ----
    bool loadScript(const std::string& filePath);
    bool loadJson(const std::string& jsonStr);

    // ---- 执行 ----
    void play(EPlayMode mode = EPlayMode::Sequential);
    void pause();
    void resume();
    void step();           // 单步执行（StepByStep 模式）
    void stop();
    bool isPlaying() const { return running_; }

    // ---- 状态 ----
    int  currentStep() const { return currentIndex_; }
    int  totalSteps() const { return static_cast<int>(steps_.size()); }
    const std::string& lastError() const { return lastError_; }
    std::string progress() const;

Q_SIGNALS:
    void stepStarted(int index, const std::string& description);
    void stepCompleted(int index, const std::string& result);
    void stepFailed(int index, const std::string& error);
    void playbackFinished();
    void playbackPaused();

private:
    void executeNextStep();
    void executeStep(int index);
    void retryStep(int index, int maxRetries = 2);
    void continuePlayback();  // 延迟继续

    PilotSession* session_;
    std::vector<RecordStep> steps_{};
    int currentIndex_ = -1;
    bool running_ = false;
    bool paused_ = false;
    std::string lastError_{};
};

/*! @} */

AST_NAMESPACE_END
