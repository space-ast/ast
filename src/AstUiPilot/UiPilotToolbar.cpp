///
/// @file      UiPilotToolbar.cpp
/// @brief     浮动录制工具栏实现 — 录制/暂停/继续/回放/润色/复制
/// @author    axel
/// @date      2026-06-13

#include "UiPilotToolbar.hpp"
#include "PilotCommander.hpp"
#include "PilotSession.hpp"
#include "Recorder/PilotRecorder.hpp"
#include "Player/PilotPlayer.hpp"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMouseEvent>
#include <QPainter>
#include <QApplication>
#include <QClipboard>
#include <QScreen>
#include <QScrollBar>
#include <QToolTip>
#include <QMessageBox>

AST_NAMESPACE_BEGIN

// ---- helpers ----
static const char* kBtnBase =
    "QPushButton {"
    "  border: 1px solid #4a4a4a; border-radius: 4px;"
    "  padding: 5px 12px; font-size: 12px;"
    "}";

namespace {
    QString greenStyle() {
        return QString(kBtnBase) +
            "QPushButton { background: #335533; color: #88ff88; border-color: #336633; }"
            "QPushButton:hover { background: #336633; }";
    }
    QString redStyle() {
        return QString(kBtnBase) +
            "QPushButton { background: #553333; color: #ff8888; border-color: #663333; }"
            "QPushButton:hover { background: #663333; }";
    }
    QString yellowStyle() {
        return QString(kBtnBase) +
            "QPushButton { background: #554433; color: #ffcc88; border-color: #665533; }"
            "QPushButton:hover { background: #665533; }";
    }
    QString blueStyle() {
        return QString(kBtnBase) +
            "QPushButton { background: #3a3a4a; color: #aaaaff; border-color: #4a4a5a; }"
            "QPushButton:hover { background: #4a4a5a; }";
    }
    QString grayStyle() {
        return QString(kBtnBase) +
            "QPushButton { background: #3a3a3a; color: #d4d4d4; }"
            "QPushButton:hover { background: #4a4a4a; }";
    }
    QString disabledStyle() {
        return "QPushButton:disabled { background: #333333; color: #555555; border-color: #333333; }";
    }
}

// ============================================================
//  构造 / 析构
// ============================================================

UiPilotToolbar::UiPilotToolbar(PilotCommander* commander, PilotSession* session,
                                 PilotRecorder* recorder, QWidget* parent)
    : QWidget(parent)
    , commander_(commander)
    , session_(session)
    , recorder_(recorder)
{
    setupUi();

    if (recorder_)
    {
        connect(recorder_, &PilotRecorder::recordingStarted,  this, &UiPilotToolbar::onRecordingStarted);
        connect(recorder_, &PilotRecorder::recordingStopped,  this, &UiPilotToolbar::onRecordingStopped);
        connect(recorder_, &PilotRecorder::recordingPaused,   this, &UiPilotToolbar::onRecordingPaused);
        connect(recorder_, &PilotRecorder::recordingResumed,  this, &UiPilotToolbar::onRecordingResumed);
        connect(recorder_, &PilotRecorder::stepRecorded,      this, &UiPilotToolbar::onStepRecorded);

        if (recorder_->isRecording())
        {
            // 同步 recorder 当前状态（可能在工具栏创建前已经开始录制）
            stepCount_ = recorder_->stepCount();
            stepList_->clear();
            for (int i = 0; i < stepCount_; i++)
            {
                const auto& step = recorder_->stepAt(i);
                std::string desc = step.naturalLanguage.empty()
                    ? step.toTemplate() : step.naturalLanguage;
                auto* item = new QListWidgetItem(
                    QString::fromUtf8("%1. ✓ %2").arg(i + 1)
                        .arg(QString::fromUtf8(desc.c_str())));
                item->setForeground(QColor(0xaa, 0xcc, 0xaa));
                stepList_->addItem(item);
            }
            stepStatLabel_->setText(
                QString::fromUtf8("已录制 %1 步").arg(stepCount_));

            if (recorder_->isPaused())
            {
                recIndicator_->setText(QString::fromUtf8("⏸ 已暂停"));
                recIndicator_->setStyleSheet(
                    "QLabel { color: #ffcc44; font-weight: bold; font-size: 12px; }");
                elapsedLabel_->setText("");
                applyState(EState::Paused);
            }
            else
            {
                recordingStart_.start();
                elapsedTimer_->start(1000);
                recIndicator_->setText(QString::fromUtf8("● 录制中"));
                recIndicator_->setStyleSheet(
                    "QLabel { color: #ff4444; font-weight: bold; font-size: 12px; }");
                stepStatLabel_->setStyleSheet(
                    "QLabel { color: #ff8888; font-size: 12px; }");
                updateElapsedDisplay();
                applyState(EState::Recording);
            }
        }
    }

    // 仅当 recorder 不在录制中时，才根据已有步骤数设置初始状态
    if (!recorder_ || !recorder_->isRecording())
        applyState(recorder_ && recorder_->stepCount() > 0 ? EState::Stopped : EState::Idle);

    if (auto* screen = QApplication::primaryScreen())
    {
        QRect geo = screen->availableGeometry();
        move(geo.right() - width() - 16, geo.top() + 80);
    }
}

UiPilotToolbar::~UiPilotToolbar()
{
    if (player_)
    {
        player_->stop();
        delete player_;
    }
}

// ============================================================
//  setupUi
// ============================================================

void UiPilotToolbar::setupUi()
{
    // 标记为录制器排除对象，避免工具栏自身的操作被录制
    setProperty("ast_recorder_exclude", true);

    setWindowFlags(Qt::Tool | Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_ShowWithoutActivating, true);
    setWindowOpacity(0.93);
    setFixedSize(360, 320);

    auto* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(8, 8, 8, 8);
    mainLayout->setSpacing(6);

    // ---- 顶部状态栏 ----
    auto* topBar = new QHBoxLayout();
    topBar->setSpacing(8);

    recIndicator_ = new QLabel(QString::fromUtf8("○ 就绪"), this);
    recIndicator_->setStyleSheet("QLabel { color: #888888; font-weight: bold; font-size: 12px; }");
    elapsedLabel_ = new QLabel("", this);
    elapsedLabel_->setStyleSheet("QLabel { color: #aaaaaa; font-size: 12px; }");
    stepStatLabel_ = new QLabel("", this);
    stepStatLabel_->setStyleSheet("QLabel { color: #888888; font-size: 12px; }");

    topBar->addWidget(recIndicator_);
    topBar->addStretch();
    topBar->addWidget(elapsedLabel_);
    topBar->addWidget(stepStatLabel_);

    // ---- 步骤列表 ----
    stepList_ = new QListWidget(this);
    stepList_->setFrameShape(QFrame::NoFrame);
    stepList_->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    stepList_->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    stepList_->setSelectionMode(QAbstractItemView::NoSelection);
    stepList_->setFocusPolicy(Qt::NoFocus);
    stepList_->setStyleSheet(
        "QListWidget { background: transparent; color: #d4d4d4; font-size: 12px; border: none; }"
        "QListWidget::item { padding: 2px 4px; border-bottom: 1px solid #3a3a3a; }"
        "QScrollBar:vertical {"
        "  background: #2a2a2e;"
        "  width: 6px;"
        "  margin: 0;"
        "}"
        "QScrollBar::handle:vertical {"
        "  background: #55555a;"
        "  border-radius: 3px;"
        "  min-height: 20px;"
        "}"
        "QScrollBar::handle:vertical:hover {"
        "  background: #77777a;"
        "}"
        "QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {"
        "  height: 0;"
        "  border: none;"
        "}"
        "QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical {"
        "  background: none;"
        "}");

    stepList_->addItem(new QListWidgetItem(
        QString::fromUtf8("  就绪。点击 ⏺ 开始录制")));

    // ---- 底部操作栏 ----
    auto* bottomBar = new QHBoxLayout();
    bottomBar->setSpacing(6);

    recordBtn_ = new QPushButton(QString::fromUtf8("⏺ 开始录制"), this);
    recordBtn_->setCursor(Qt::PointingHandCursor);
    recordBtn_->setStyleSheet(greenStyle());

    pauseBtn_ = new QPushButton(QString::fromUtf8("⏸ 暂停"), this);
    pauseBtn_->setCursor(Qt::PointingHandCursor);
    pauseBtn_->setStyleSheet(yellowStyle());
    pauseBtn_->setVisible(false);

    replayBtn_ = new QPushButton(QString::fromUtf8("▶ 回放"), this);
    replayBtn_->setCursor(Qt::PointingHandCursor);
    replayBtn_->setStyleSheet(blueStyle());

    polishBtn_ = new QPushButton(QString::fromUtf8("💎 润色"), this);
    polishBtn_->setCursor(Qt::PointingHandCursor);
    polishBtn_->setToolTip(QString::fromUtf8("LLM 优化步骤描述"));
    polishBtn_->setStyleSheet(blueStyle() + disabledStyle());

    copyBtn_ = new QPushButton(QString::fromUtf8("📋 复制"), this);
    copyBtn_->setCursor(Qt::PointingHandCursor);
    copyBtn_->setToolTip(QString::fromUtf8("复制 JSON 脚本到剪贴板"));
    copyBtn_->setStyleSheet(grayStyle() + disabledStyle());

    bottomBar->addWidget(recordBtn_);
    bottomBar->addWidget(pauseBtn_);
    bottomBar->addWidget(replayBtn_);
    bottomBar->addStretch();
    bottomBar->addWidget(polishBtn_);
    bottomBar->addWidget(copyBtn_);

    mainLayout->addLayout(topBar);
    mainLayout->addWidget(stepList_, 1);
    mainLayout->addLayout(bottomBar);

    // 信号
    connect(recordBtn_, &QPushButton::clicked, this, &UiPilotToolbar::onRecordToggle);
    connect(pauseBtn_,  &QPushButton::clicked, this, &UiPilotToolbar::onPauseResumeToggle);
    connect(replayBtn_, &QPushButton::clicked, this, &UiPilotToolbar::onReplay);
    connect(polishBtn_, &QPushButton::clicked, this, &UiPilotToolbar::onPolish);
    connect(copyBtn_,   &QPushButton::clicked, this, &UiPilotToolbar::onCopy);

    elapsedTimer_ = new QTimer(this);
    connect(elapsedTimer_, &QTimer::timeout, this, &UiPilotToolbar::updateElapsedDisplay);
}

// ============================================================
//  applyState — 统一状态切换
// ============================================================

void UiPilotToolbar::applyState(EState state)
{
    state_ = state;

    bool isStoppedOrIdle = (state == EState::Stopped || state == EState::Idle);
    bool isRecording     = (state == EState::Recording);
    bool isPaused        = (state == EState::Paused);
    bool isReplaying     = (state == EState::Replaying);
    bool hasSteps        = (recorder_ && recorder_->stepCount() > 0);

    // 录制按钮
    recordBtn_->setVisible(!isReplaying);
    recordBtn_->setEnabled(isStoppedOrIdle || isRecording || isPaused);
    if (isRecording || isPaused)
    {
        recordBtn_->setText(QString::fromUtf8("⏹ 停止录制"));
        recordBtn_->setStyleSheet(redStyle());
    }
    else
    {
        recordBtn_->setText(QString::fromUtf8("⏺ 开始录制"));
        recordBtn_->setStyleSheet(greenStyle());
    }

    // 暂停按钮
    pauseBtn_->setVisible(isRecording || isPaused);
    if (isPaused)
    {
        pauseBtn_->setText(QString::fromUtf8("▶ 继续"));
        pauseBtn_->setStyleSheet(greenStyle());
    }
    else
    {
        pauseBtn_->setText(QString::fromUtf8("⏸ 暂停"));
        pauseBtn_->setStyleSheet(yellowStyle());
    }

    // 回放按钮
    replayBtn_->setVisible(!isRecording && !isPaused);
    replayBtn_->setEnabled(hasSteps && !isReplaying);
    if (isReplaying)
    {
        replayBtn_->setText(QString::fromUtf8("⏹ 停止回放"));
        replayBtn_->setStyleSheet(redStyle());
    }
    else
    {
        replayBtn_->setText(QString::fromUtf8("▶ 回放"));
        replayBtn_->setStyleSheet(blueStyle());
    }

    // 润色/复制
    polishBtn_->setEnabled(hasSteps && !isRecording && !isPaused && !isReplaying);
    copyBtn_->setEnabled(hasSteps && !isReplaying);
}

// ============================================================
//  录制回调
// ============================================================

void UiPilotToolbar::onRecordingStarted()
{
    stepCount_ = 0;
    recordingStart_.start();
    elapsedTimer_->start(1000);

    recIndicator_->setText(QString::fromUtf8("● 录制中"));
    recIndicator_->setStyleSheet("QLabel { color: #ff4444; font-weight: bold; font-size: 12px; }");
    stepStatLabel_->setText(QString::fromUtf8("已录制 0 步"));
    stepStatLabel_->setStyleSheet("QLabel { color: #ff8888; font-size: 12px; }");
    updateElapsedDisplay();

    stepList_->clear();
    applyState(EState::Recording);
}

void UiPilotToolbar::onRecordingStopped()
{
    elapsedTimer_->stop();

    recIndicator_->setText(QString::fromUtf8("○ 已停止"));
    recIndicator_->setStyleSheet("QLabel { color: #888888; font-weight: bold; font-size: 12px; }");
    stepStatLabel_->setText(QString::fromUtf8("共录制 %1 步").arg(stepCount_));
    stepStatLabel_->setStyleSheet("QLabel { color: #888888; font-size: 12px; }");

    applyState(stepCount_ > 0 ? EState::Stopped : EState::Idle);
}

void UiPilotToolbar::onRecordingPaused()
{
    elapsedTimer_->stop();

    recIndicator_->setText(QString::fromUtf8("⏸ 已暂停"));
    recIndicator_->setStyleSheet("QLabel { color: #ffcc44; font-weight: bold; font-size: 12px; }");

    // 插入暂停标记
    auto* item = new QListWidgetItem(QString::fromUtf8("  ⏸ — 录制已暂停 —"));
    item->setFlags(Qt::NoItemFlags);
    item->setForeground(QColor(0xff, 0xcc, 0x44));
    stepList_->addItem(item);
    scrollToBottom();

    applyState(EState::Paused);
}

void UiPilotToolbar::onRecordingResumed()
{
    recordingStart_.restart();
    elapsedTimer_->start(1000);

    recIndicator_->setText(QString::fromUtf8("● 录制中"));
    recIndicator_->setStyleSheet("QLabel { color: #ff4444; font-weight: bold; font-size: 12px; }");
    updateElapsedDisplay();

    // 插入恢复标记
    auto* item = new QListWidgetItem(QString::fromUtf8("  ▶ — 录制已恢复 —"));
    item->setFlags(Qt::NoItemFlags);
    item->setForeground(QColor(0x88, 0xff, 0x88));
    stepList_->addItem(item);
    scrollToBottom();

    applyState(EState::Recording);
}

void UiPilotToolbar::onStepRecorded(int, const std::string& description)
{
    stepCount_++;
    stepStatLabel_->setText(QString::fromUtf8("已录制 %1 步").arg(stepCount_));

    auto* item = new QListWidgetItem(
        QString::fromUtf8("%1. ✓ %2").arg(stepCount_)
            .arg(QString::fromUtf8(description.c_str())));
    item->setForeground(QColor(0xaa, 0xcc, 0xaa));
    stepList_->addItem(item);
    scrollToBottom();
}

// ============================================================
//  按钮事件
// ============================================================

void UiPilotToolbar::onRecordToggle()
{
    if (!recorder_) return;

    if (recorder_->isRecording())
    {
        recorder_->stop();
        std::string path = "record_output.json";
        recorder_->saveToFile(path);
        QToolTip::showText(mapToGlobal(recordBtn_->pos() + QPoint(0, 30)),
            QString::fromUtf8("已导出到 %1").arg(QString::fromStdString(path)),
            this, QRect(), 2000);
    }
    else
    {
        stepList_->clear();
        stepList_->addItem(new QListWidgetItem(
            QString::fromUtf8("  录制中...")));
        recorder_->start();
    }
}

void UiPilotToolbar::onPauseResumeToggle()
{
    if (!recorder_) return;

    if (recorder_->isPaused())
        recorder_->resume();
    else
        recorder_->pause();
}

// ============================================================
//  回放
// ============================================================

void UiPilotToolbar::onReplay()
{
    if (!recorder_) return;

    // 已在回放中 → 停止
    if (state_ == EState::Replaying && player_)
    {
        player_->stop();
        delete player_;
        player_ = nullptr;
        rebuildStepListFromRecorder();
        applyState(EState::Stopped);
        return;
    }

    if (recorder_->stepCount() == 0) return;

    // 创建回放器
    player_ = new PilotPlayer(session_);
    std::string json = recorder_->toJson();
    if (!player_->loadJson(json))
    {
        delete player_;
        player_ = nullptr;
        return;
    }

    connect(player_, &PilotPlayer::stepStarted,   this, &UiPilotToolbar::onReplayStepStarted);
    connect(player_, &PilotPlayer::stepCompleted, this, &UiPilotToolbar::onReplayStepCompleted);
    connect(player_, &PilotPlayer::stepFailed,    this, &UiPilotToolbar::onReplayStepFailed);
    connect(player_, &PilotPlayer::playbackFinished, this, &UiPilotToolbar::onReplayFinished);

    // 初始化步骤列表
    stepList_->clear();
    for (int i = 0; i < recorder_->stepCount(); i++)
    {
        const auto& step = recorder_->stepAt(i);
        std::string desc = step.naturalLanguage.empty()
            ? step.toTemplate() : step.naturalLanguage;
        auto* item = new QListWidgetItem(
            QString::fromUtf8("%1. ○ %2").arg(i + 1)
                .arg(QString::fromUtf8(desc.c_str())));
        item->setForeground(QColor(0x88, 0x88, 0x88));
        stepList_->addItem(item);
    }

    recIndicator_->setText(QString::fromUtf8("▶ 回放中"));
    recIndicator_->setStyleSheet("QLabel { color: #aaaaff; font-weight: bold; font-size: 12px; }");
    stepStatLabel_->setText(QString::fromUtf8("共 %1 步").arg(recorder_->stepCount()));
    applyState(EState::Replaying);

    player_->play();
}

void UiPilotToolbar::onReplayStepStarted(int index, const std::string&)
{
    if (index < stepList_->count() - 1)
    {
        // 清除上一步的 → 标记
        auto* prev = stepList_->item(index > 0 ? index : 0);
        if (prev && index > 0)
            prev->setForeground(QColor(0xaa, 0xcc, 0xaa));
    }
    if (index < stepList_->count())
    {
        auto* item = stepList_->item(index);
        if (item)
        {
            item->setForeground(QColor(0xff, 0xdd, 0x44));
            // 加上 → 标记
            QString text = item->text();
            text.replace("○", "→");
            item->setText(text);
        }
    }
    stepStatLabel_->setText(
        QString::fromUtf8("步骤 %1/%2").arg(index + 1).arg(stepList_->count()));
    scrollToBottom();
}

void UiPilotToolbar::onReplayStepCompleted(int index, const std::string&)
{
    if (index < stepList_->count())
    {
        auto* item = stepList_->item(index);
        if (item)
        {
            item->setForeground(QColor(0x88, 0xff, 0x88));
            QString text = item->text();
            text.replace("→", "✓");
            item->setText(text);
        }
    }
}

void UiPilotToolbar::onReplayStepFailed(int index, const std::string&)
{
    if (index < stepList_->count())
    {
        auto* item = stepList_->item(index);
        if (item)
        {
            item->setForeground(QColor(0xff, 0x66, 0x66));
            QString text = item->text();
            text.replace("→", "✗");
            item->setText(text);
        }
    }
}

void UiPilotToolbar::onReplayFinished()
{
    recIndicator_->setText(QString::fromUtf8("○ 回放完成"));
    recIndicator_->setStyleSheet("QLabel { color: #888888; font-weight: bold; font-size: 12px; }");
    stepStatLabel_->setText(QString::fromUtf8("共 %1 步").arg(stepList_->count()));

    delete player_;
    player_ = nullptr;

    applyState(EState::Stopped);
}

void UiPilotToolbar::rebuildStepListFromRecorder()
{
    stepList_->clear();
    if (!recorder_) return;
    for (int i = 0; i < recorder_->stepCount(); i++)
    {
        const auto& step = recorder_->stepAt(i);
        std::string desc = step.naturalLanguage.empty()
            ? step.toTemplate() : step.naturalLanguage;
        auto* item = new QListWidgetItem(
            QString::fromUtf8("%1. ✓ %2").arg(i + 1)
                .arg(QString::fromUtf8(desc.c_str())));
        item->setForeground(QColor(0xaa, 0xcc, 0xaa));
        stepList_->addItem(item);
    }
}

// ============================================================
//  润色 / 复制
// ============================================================

void UiPilotToolbar::onPolish()
{
    if (!recorder_ || !session_ || recorder_->stepCount() == 0) return;

    polishBtn_->setEnabled(false);
    polishBtn_->setText(QString::fromUtf8("💎 润色中..."));

    recorder_->polish(session_);
    rebuildStepListFromRecorder();

    recorder_->saveToFile("record_polished.json");

    polishBtn_->setText(QString::fromUtf8("💎 润色"));
    polishBtn_->setEnabled(true);

    QToolTip::showText(mapToGlobal(polishBtn_->pos() + QPoint(0, 30)),
        QString::fromUtf8("润色完成 → record_polished.json"), this, QRect(), 2000);
}

void UiPilotToolbar::onCopy()
{
    if (!recorder_) return;
    QApplication::clipboard()->setText(QString::fromStdString(recorder_->toJson()));
    QToolTip::showText(mapToGlobal(copyBtn_->pos() + QPoint(0, 30)),
        QString::fromUtf8("已复制 %1 步到剪贴板").arg(recorder_->stepCount()),
        this, QRect(), 2000);
}

// ============================================================
//  计时 / 滚动
// ============================================================

void UiPilotToolbar::updateElapsedDisplay()
{
    if (!recorder_ || !recorder_->isRecording() || recorder_->isPaused()) return;
    qint64 secs = recordingStart_.elapsed() / 1000;
    elapsedLabel_->setText(
        QString::fromUtf8("(%1:%2)")
            .arg(static_cast<int>(secs / 60), 2, 10, QChar('0'))
            .arg(static_cast<int>(secs % 60), 2, 10, QChar('0')));
}

void UiPilotToolbar::scrollToBottom()
{
    if (auto* sb = stepList_->verticalScrollBar())
        sb->setValue(sb->maximum());
}

// ============================================================
//  外观 / 拖拽
// ============================================================

void UiPilotToolbar::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setBrush(QColor(0x28, 0x28, 0x2c, 0xf0));
    painter.setPen(QPen(QColor(0x55, 0x55, 0x5a, 0xc0), 1));
    painter.drawRoundedRect(rect().adjusted(0, 0, -1, -1), 8, 8);
}

void UiPilotToolbar::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton)
    {
        dragStartPos_ = event->globalPos() - frameGeometry().topLeft();
        dragging_ = true;
    }
    QWidget::mousePressEvent(event);
}

void UiPilotToolbar::mouseMoveEvent(QMouseEvent* event)
{
    if (dragging_ && (event->buttons() & Qt::LeftButton))
        move(event->globalPos() - dragStartPos_);
    QWidget::mouseMoveEvent(event);
}

void UiPilotToolbar::mouseDoubleClickEvent(QMouseEvent*)
{
    setWindowOpacity(windowOpacity() < 0.5 ? 0.93 : 0.35);
}

AST_NAMESPACE_END
