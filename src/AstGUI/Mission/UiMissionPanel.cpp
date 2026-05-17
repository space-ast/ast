///
/// @file      UiMissionPanel.cpp
/// @brief     任务序列编辑主面板实现
/// @details   ~
/// @author    axel
/// @date      2026-05-17
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

#include "UiMissionPanel.hpp"
#include "UiMissionTree.hpp"
#include "UiSegmentEditor.hpp"
#include "AstCore/MissionModerator.hpp"
#include "AstCore/Sequence.hpp"
#include "AstCore/InitialState.hpp"
#include "AstCore/Propagate.hpp"
#include "AstCore/Maneuver.hpp"
#include "AstCore/TargeterSequence.hpp"
#include "AstLoader/MissionCommandLoader.hpp"
#include "AstUtil/RTTIAPI.hpp"
#include <QSplitter>
#include <QVBoxLayout>
#include <QPlainTextEdit>
#include <QMessageBox>
#include <QFileDialog>

AST_NAMESPACE_BEGIN

UiMissionPanel::UiMissionPanel(QWidget* parent)
    : QWidget(parent)
{
    setupUi();
    setupToolBar();
    setupConnections();
}

UiMissionPanel::~UiMissionPanel() = default;

// ============================================================================
// 界面搭建
// ============================================================================

void UiMissionPanel::setupUi()
{
    auto* rootLayout = new QVBoxLayout(this);
    rootLayout->setContentsMargins(0, 0, 0, 0);

    // 工具栏区域 — toolBar_ 由 setupToolBar 创建
    // (此处仅占位，setupToolBar 在构造函数中先于 setupConnections 调用)

    // 主分割器
    mainSplitter_ = new QSplitter(Qt::Horizontal, this);

    missionTree_ = new UiMissionTree(this);
    missionTree_->setMinimumWidth(200);
    mainSplitter_->addWidget(missionTree_);

    segmentEditor_ = new UiSegmentEditor(this);
    segmentEditor_->setMinimumWidth(300);
    mainSplitter_->addWidget(segmentEditor_);

    mainSplitter_->setStretchFactor(0, 1);
    mainSplitter_->setStretchFactor(1, 2);

    rootLayout->addWidget(mainSplitter_);

    // 输出区域
    outputView_ = new QPlainTextEdit(this);
    outputView_->setReadOnly(true);
    outputView_->setMaximumHeight(120);
    outputView_->setPlaceholderText(tr("输出信息..."));
    rootLayout->addWidget(outputView_);
}

void UiMissionPanel::setupToolBar()
{
    toolBar_ = new QToolBar(tr("Mission"), this);

    openAction_ = toolBar_->addAction(tr("打开"));
    openAction_->setToolTip(tr("打开 .MCSSegment 文件"));

    saveAction_ = toolBar_->addAction(tr("保存"));
    saveAction_->setToolTip(tr("保存任务序列"));

    toolBar_->addSeparator();

    // 添加段菜单
    addMenu_ = new QMenu(tr("添加段"), this);
    addMenu_->addAction(tr("初始状态 (InitialState)"),  this, &UiMissionPanel::onAddInitialState);
    addMenu_->addAction(tr("轨道预报 (Propagate)"),    this, &UiMissionPanel::onAddPropagate);
    addMenu_->addAction(tr("机动 (Maneuver)"),         this, &UiMissionPanel::onAddManeuver);
    addMenu_->addAction(tr("序列 (Sequence)"),         this, &UiMissionPanel::onAddSequence);
    addMenu_->addAction(tr("打靶序列 (TargeterSeq)"),  this, &UiMissionPanel::onAddTargeterSequence);

    auto* addButton = new QAction(tr("+ 添加"), this);
    addButton->setMenu(addMenu_);
    toolBar_->addAction(addButton);

    deleteAction_ = toolBar_->addAction(tr("删除"));
    deleteAction_->setToolTip(tr("删除选中的段"));

    toolBar_->addSeparator();

    runAction_ = toolBar_->addAction(tr("▶ 运行"));
    runAction_->setToolTip(tr("执行任务序列"));

    stopAction_ = toolBar_->addAction(tr("■ 停止"));
    stopAction_->setToolTip(tr("停止执行"));
    stopAction_->setEnabled(false);

    // 将 toolbar 放到布局顶部
    auto* rootLayout = qobject_cast<QVBoxLayout*>(layout());
    if (rootLayout)
        rootLayout->insertWidget(0, toolBar_);
}

void UiMissionPanel::setupConnections()
{
    // 树选中 → 编辑器切换
    connect(missionTree_, &UiMissionTree::segmentSelected,
            this, &UiMissionPanel::onSegmentSelected);

    // 树结构变化
    connect(missionTree_, &UiMissionTree::treeModified,
            this, [this]() { appendOutput(tr("任务序列已更新")); });

    // 工具栏动作
    connect(deleteAction_, &QAction::triggered,
            this, &UiMissionPanel::onDeleteSegment);
    connect(openAction_, &QAction::triggered,
            this, &UiMissionPanel::onOpenFile);
    connect(saveAction_, &QAction::triggered,
            this, &UiMissionPanel::onSaveFile);
    connect(runAction_, &QAction::triggered,
            this, &UiMissionPanel::onRun);
    connect(stopAction_, &QAction::triggered,
            this, &UiMissionPanel::onStop);
}

// ============================================================================
// 公共接口
// ============================================================================

void UiMissionPanel::setModerator(MissionModerator* moderator)
{
    moderator_ = moderator;
    missionTree_->setModerator(moderator);
    if (moderator)
        appendOutput(tr("已加载任务序列"));
}

MissionModerator* UiMissionPanel::moderator() const
{
    return moderator_;
}

// ============================================================================
// 段操作
// ============================================================================

void UiMissionPanel::onSegmentSelected(MissionCommand* cmd)
{
    segmentEditor_->editCommand(cmd);
}

static HMissionCommand createSegment(const QString& typeName)
{
    if (typeName == "InitialState")
        return HMissionCommand(aNewObject<InitialState>());
    if (typeName == "Propagate")
        return HMissionCommand(aNewObject<Propagate>());
    if (typeName == "Maneuver")
        return HMissionCommand(aNewObject<Maneuver>());
    if (typeName == "Sequence")
        return HMissionCommand(aNewObject<Sequence>());
    if (typeName == "TargeterSequence")
        return HMissionCommand(aNewObject<TargeterSequence>());
    return nullptr;
}

void UiMissionPanel::onAddInitialState()
{
    if (!moderator_)
    {
        QMessageBox::warning(this, tr("提示"), tr("请先创建或加载任务序列。"));
        return;
    }

    auto cmd = createSegment("InitialState");
    if (!cmd)
        return;

    cmd->setName("Initial State");

    // 添加到根序列的第一个位置
    auto& rootSeq = moderator_->getSequence();
    auto cmds = rootSeq.getCommands();
    cmds.insert(cmds.begin(), cmd);
    rootSeq.setCommands(std::move(cmds));

    // 刷新树
    missionTree_->setModerator(moderator_);

    appendOutput(tr("添加: 初始状态段"));
}

void UiMissionPanel::onAddPropagate()
{
    if (!moderator_)
    {
        QMessageBox::warning(this, tr("提示"), tr("请先创建或加载任务序列。"));
        return;
    }

    auto cmd = createSegment("Propagate");
    if (!cmd)
        return;

    cmd->setName("Propagate");

    auto& rootSeq = moderator_->getSequence();
    auto cmds = rootSeq.getCommands();
    cmds.push_back(cmd);
    rootSeq.setCommands(std::move(cmds));

    missionTree_->setModerator(moderator_);
    appendOutput(tr("添加: 轨道预报段"));
}

void UiMissionPanel::onAddManeuver()
{
    if (!moderator_)
    {
        QMessageBox::warning(this, tr("提示"), tr("请先创建或加载任务序列。"));
        return;
    }

    auto cmd = createSegment("Maneuver");
    if (!cmd)
        return;

    cmd->setName("Maneuver");

    auto& rootSeq = moderator_->getSequence();
    auto cmds = rootSeq.getCommands();
    cmds.push_back(cmd);
    rootSeq.setCommands(std::move(cmds));

    missionTree_->setModerator(moderator_);
    appendOutput(tr("添加: 机动段"));
}

void UiMissionPanel::onAddSequence()
{
    if (!moderator_)
    {
        QMessageBox::warning(this, tr("提示"), tr("请先创建或加载任务序列。"));
        return;
    }

    auto cmd = createSegment("Sequence");
    if (!cmd)
        return;

    cmd->setName("Sequence");

    auto& rootSeq = moderator_->getSequence();
    auto cmds = rootSeq.getCommands();
    cmds.push_back(cmd);
    rootSeq.setCommands(std::move(cmds));

    missionTree_->setModerator(moderator_);
    appendOutput(tr("添加: 子序列段"));
}

void UiMissionPanel::onAddTargeterSequence()
{
    if (!moderator_)
    {
        QMessageBox::warning(this, tr("提示"), tr("请先创建或加载任务序列。"));
        return;
    }

    auto cmd = createSegment("TargeterSequence");
    if (!cmd)
        return;

    cmd->setName("TargeterSequence");

    auto& rootSeq = moderator_->getSequence();
    auto cmds = rootSeq.getCommands();
    cmds.push_back(cmd);
    rootSeq.setCommands(std::move(cmds));

    missionTree_->setModerator(moderator_);
    appendOutput(tr("添加: 打靶序列段"));
}

void UiMissionPanel::onDeleteSegment()
{
    missionTree_->removeSelectedCommand();
}

// ============================================================================
// 文件操作
// ============================================================================

void UiMissionPanel::onOpenFile()
{
    QString filePath = QFileDialog::getOpenFileName(
        this,
        tr("打开任务序列文件"),
        QString(),
        tr("MCSSegment 文件 (*.MCSSegment);;所有文件 (*.*)"));
    if (filePath.isEmpty())
        return;

    std::string pathStr = filePath.toStdString();
    SharedPtr<MissionCommand> loadedCmd;
    errc_t rc = aLoadMissionCommand(StringView(pathStr), loadedCmd);

    if (rc != eNoError)
    {
        QMessageBox::warning(this, tr("加载失败"),
                             tr("无法加载文件: %1\n错误码: %2")
                             .arg(filePath).arg(rc));
        return;
    }

    auto* loadedSeq = aobject_cast<Sequence*>(loadedCmd.get());
    if (!loadedSeq)
    {
        QMessageBox::warning(this, tr("加载失败"),
                             tr("文件不包含有效的任务序列段。"));
        return;
    }

    // 若尚无 moderator，自动创建
    if (!moderator_)
    {
        moderator_ = new MissionModerator();
    }

    // 将加载的数据拷贝到 moderator 的根序列
    auto& rootSeq = moderator_->getSequence();
    rootSeq.setCommands(loadedSeq->getCommands());
    rootSeq.setRepeatCount(loadedSeq->repeatCount());
    rootSeq.setName(loadedSeq->getName());

    // 刷新树
    missionTree_->setModerator(moderator_);

    appendOutput(tr("已加载: %1 (%2 个命令)")
                 .arg(filePath)
                 .arg(loadedSeq->getCommands().size()));
}

void UiMissionPanel::onSaveFile()
{
    if (!moderator_)
    {
        QMessageBox::warning(this, tr("提示"), tr("没有内容可保存。"));
        return;
    }

    QString filePath = QFileDialog::getSaveFileName(
        this,
        tr("保存任务序列文件"),
        QString(),
        tr("MCSSegment 文件 (*.MCSSegment);;所有文件 (*.*)"));
    if (filePath.isEmpty())
        return;

    // TODO: 调用序列化保存 — 当前 AstLoader 尚无导出保存接口
    appendOutput(tr("保存功能尚未实现: %1").arg(filePath));
}

// ============================================================================
// 运行控制
// ============================================================================

void UiMissionPanel::onRun()
{
    runAction_->setEnabled(false);
    stopAction_->setEnabled(true);
    appendOutput(tr("开始执行任务序列..."));

    emit runRequested();
}

void UiMissionPanel::onStop()
{
    runAction_->setEnabled(true);
    stopAction_->setEnabled(false);
    appendOutput(tr("任务序列已停止"));

    emit stopped();
}

void UiMissionPanel::appendOutput(const QString& text)
{
    outputView_->appendPlainText(text);
}

AST_NAMESPACE_END
