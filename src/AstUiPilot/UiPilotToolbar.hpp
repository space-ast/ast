///
/// @file      UiPilotToolbar.hpp
/// @brief     浮动录制工具栏 — 半透明、置顶、实时步骤列表
/// @author    axel
/// @date      2026-06-13
/// @copyright 版权所有 (C) 2026-present, SpaceAST项目.

#pragma once

#include "AstGlobal.h"
#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QListWidget>
#include <QTimer>
#include <QElapsedTimer>

AST_NAMESPACE_BEGIN

class PilotCommander;
class PilotRecorder;
class PilotPlayer;
class PilotSession;

/// @brief 浮动录制工具栏
/// @details 半透明置顶窗口。录制时实时滚动显示步骤列表，
///          录制中可暂停/继续，停止后可回放、润色、复制。
class AST_UIPILOT_API UiPilotToolbar : public QWidget
{
    Q_OBJECT

public:
    explicit UiPilotToolbar(PilotCommander* commander, PilotSession* session,
                            PilotRecorder* recorder, QWidget* parent = nullptr);
    ~UiPilotToolbar() override;

Q_SIGNALS:
    void consoleToggleRequested();
    void closed();

protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseDoubleClickEvent(QMouseEvent* event) override;
    void paintEvent(QPaintEvent* event) override;

private Q_SLOTS:
    void onRecordToggle();
    void onPauseResumeToggle();
    void onReplay();
    void onPolish();
    void onCopy();

    void onRecordingStarted();
    void onRecordingStopped();
    void onRecordingPaused();
    void onRecordingResumed();
    void onStepRecorded(int index, const std::string& description);

    void onReplayStepStarted(int index, const std::string& description);
    void onReplayStepCompleted(int index, const std::string& result);
    void onReplayStepFailed(int index, const std::string& error);
    void onReplayFinished();

private:
    enum class EState { Idle, Recording, Paused, Stopped, Replaying };

    void setupUi();
    void applyState(EState state);
    void updateElapsedDisplay();
    void scrollToBottom();
    void rebuildStepListFromRecorder();

    PilotCommander*  commander_;
    PilotSession*    session_;
    PilotRecorder*   recorder_;
    PilotPlayer*     player_ = nullptr;

    // 顶部
    QLabel*          recIndicator_;
    QLabel*          elapsedLabel_;
    QLabel*          stepStatLabel_;

    // 步骤
    QListWidget*     stepList_;

    // 底部按钮
    QPushButton*     recordBtn_;      // ⏺ 开始录制 / ⏹ 停止录制
    QPushButton*     pauseBtn_;       // ⏸ 暂停 / ▶ 继续
    QPushButton*     replayBtn_;      // ▶ 回放 / ⏹ 停止回放
    QPushButton*     polishBtn_;      // 💎 润色
    QPushButton*     copyBtn_;        // 📋 复制

    // 计时
    QTimer*          elapsedTimer_;
    QElapsedTimer    recordingStart_;
    int              stepCount_  = 0;
    EState           state_      = EState::Idle;

    // 拖拽
    QPoint           dragStartPos_;
    bool             dragging_   = false;
};

AST_NAMESPACE_END
