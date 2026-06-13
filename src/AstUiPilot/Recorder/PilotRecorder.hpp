///
/// @file      PilotRecorder.hpp
/// @brief     界面操作录制器
/// @author    axel
/// @date      2026-06-12
/// @copyright 版权所有 (C) 2026-present, SpaceAST项目.

#pragma once

#include "AstGlobal.h"
#include "RecordStep.hpp"
#include <QObject>
#include <QElapsedTimer>
#include <QList>
#include <QString>
#include <string>

class QWidget;
class QMouseEvent;
class QKeyEvent;
class QFocusEvent;
class QComboBox;
class QDialog;

AST_NAMESPACE_BEGIN

class PilotAgent;
class PilotSession;

/*!
    @addtogroup UiPilot
    @{
*/

/// @brief 界面操作录制器
/// @details 通过全局事件过滤器截获用户操作，转为 RecordStep 序列，
///          导出为结构化 JSON 或自然语言脚本。
class AST_UIPILOT_API PilotRecorder : public QObject
{
    Q_OBJECT

public:
    explicit PilotRecorder(PilotAgent* agent, QObject* parent = nullptr);
    ~PilotRecorder() override;

    // ---- 生命周期 ----
    void start();
    void stop();
    void pause();
    void resume();
    bool isRecording() const { return recording_; }
    bool isPaused()    const { return paused_; }

    // ---- 导出 ----
    int  stepCount() const { return steps_.size(); }
    const RecordStep& stepAt(int index) const;
    std::string toJson() const;
    bool saveToFile(const std::string& path) const;

    // ---- LLM 润色 ----
    std::string polish();

Q_SIGNALS:
    void stepRecorded(int index, const std::string& description);
    void recordingStarted();
    void recordingStopped();
    void recordingPaused();
    void recordingResumed();

protected:
    bool eventFilter(QObject* obj, QEvent* event) override;

private:
    // ---- 事件处理（handleComboChange/handleDialogFinished 需公开供信号连接调用）----
    void handleMousePress(QWidget* w, QMouseEvent* e);
    void handleMouseRelease(QWidget* w, QMouseEvent* e);
    void handleKeyRelease(QWidget* w, QKeyEvent* e);
    void handleFocusIn(QWidget* w, QFocusEvent* e);
    void handleFocusOut(QWidget* w, QFocusEvent* e);

public:
    void handleComboChange(QComboBox* combo);
    void handleDialogFinished(QDialog* dlg, int result);

private:
    // ---- 辅助 ----
    void addStep(const RecordStep& step);
    void flushPendingEdit();
    void populateWidgetInfo(RecordStep& step, QWidget* w);

    PilotAgent* agent_;
    QList<RecordStep> steps_;
    bool recording_ = false;
    bool paused_    = false;
    int64_t startTime_ = 0;

    // 双击检测
    QWidget* lastClickedWidget_ = nullptr;
    int clickCount_ = 0;
    QElapsedTimer clickTimer_;

    // 拖拽跟踪
    QWidget* dragSourceWidget_ = nullptr;
    QString  dragSourceText_;

    // 输入框编辑跟踪
    QWidget* editWidget_ = nullptr;
    QString  editBeforeText_;
};

/*! @} */

AST_NAMESPACE_END
