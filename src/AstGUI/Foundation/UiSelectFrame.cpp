///
/// @file      UiSelectFrame.cpp
/// @brief     坐标系选择器实现
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

#include "UiSelectFrame.hpp"
#include "AstCore/Frame.hpp"
#include "AstCore/CelestialBody.hpp"
#include "AstCore/SolarSystem.hpp"
#include "AstCore/RunTimeSolarSystem.hpp"
#include "AstUtil/RTTIAPI.hpp"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QListWidget>
#include <QDialog>
#include <QDialogButtonBox>
#include <unordered_map>

AST_NAMESPACE_BEGIN

// ============================================================================
// UiSelectFrame
// ============================================================================

UiSelectFrame::UiSelectFrame(QWidget* parent)
    : QWidget(parent)
{
    setupUi();
    populateBodies();
}

UiSelectFrame::~UiSelectFrame() = default;

void UiSelectFrame::setupUi()
{
    auto* layout = new QHBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);

    // 左侧：天体列表
    auto* leftLayout = new QVBoxLayout();
    leftLayout->addWidget(new QLabel(tr(u8"天体"), this));
    bodyList_ = new QListWidget(this);
    bodyList_->setMinimumWidth(130);
    leftLayout->addWidget(bodyList_);
    layout->addLayout(leftLayout);

    // 右侧：坐标系列表
    auto* rightLayout = new QVBoxLayout();
    rightLayout->addWidget(new QLabel(tr(u8"坐标系"), this));
    frameList_ = new QListWidget(this);
    frameList_->setMinimumWidth(180);
    rightLayout->addWidget(frameList_);
    layout->addLayout(rightLayout);

    connect(bodyList_, &QListWidget::currentItemChanged,
            this, &UiSelectFrame::onBodySelected);
    connect(frameList_, &QListWidget::currentItemChanged,
            this, &UiSelectFrame::onFrameSelected);
}

// ============================================================================
// 天体列表
// ============================================================================

bool UiSelectFrame::filterBody(CelestialBody* body)
{
    if(body->getJplIndex() > JplDe::eSun || body->getJplIndex() < JplDe::eMercury)
        return false;
    return true;
}

void UiSelectFrame::populateBodies()
{
    auto* solarSystem = aGetSolarSystem();
    if (!solarSystem)
        return;

    auto children = aFindChildren(solarSystem, CelestialBody::StaticType());
    for (auto* obj : children)
    {
        auto* body = static_cast<CelestialBody*>(obj);
        if (!filterBody(body))
            continue;
        auto* item = new QListWidgetItem(QString::fromStdString(body->getName()));
        item->setData(Qt::UserRole, reinterpret_cast<qulonglong>(body));
        bodyList_->addItem(item);
    }

    // 回退：未注册子对象时通过 getter 收集
    if (bodyList_->count() == 0)
    {
        static CelestialBody* (*const kFallbackGetters[])() = {
            aGetEarth, aGetSun, aGetMoon,
            aGetMercury, aGetVenus, aGetMars,
            aGetJupiter, aGetSaturn, aGetUranus, aGetNeptune, aGetPluto,
            aGetSSBarycenter, aGetEMBarycenter,
        };
        for (auto getter : kFallbackGetters)
        {
            auto* body = getter();
            if (!body || !filterBody(body))
                continue;
            auto* item = new QListWidgetItem(QString::fromStdString(body->getName()));
            item->setData(Qt::UserRole, reinterpret_cast<qulonglong>(body));
            bodyList_->addItem(item);
        }
    }

    if (bodyList_->count() > 0)
        bodyList_->setCurrentRow(0);
}

// ============================================================================
// Frame 列表
// ============================================================================

void UiSelectFrame::ensureFramesInitialized(CelestialBody* body)
{
    if (!body)
        return;

    // 已有 Frame 子对象则跳过
    auto existing = aFindChildren(body, Frame::StaticType());
    if (!existing.empty())
        return;

    // 通过工厂方法创建默认坐标系并注册到天体下
    struct Entry { const char* name; HFrame (CelestialBody::*factory)() const; };
    static const Entry kFactories[] = {
        {"Inertial", &CelestialBody::makeFrameInertial},
        {"ICRF",     &CelestialBody::makeFrameICRF},
        {"J2000",    &CelestialBody::makeFrameJ2000},
        {"TOD",      &CelestialBody::makeFrameTOD},
        {"Fixed",    &CelestialBody::makeFrameFixed},
    };

    // 持有强引用，防止 Frame 在查询间隙被回收
    static std::unordered_map<CelestialBody*, std::vector<HFrame>> s_holder;

    auto& holder = s_holder[body];
    for (auto& entry : kFactories)
    {
        HFrame hf = (body->*entry.factory)();
        if (!hf)
            continue;
        hf->setName(entry.name);
        hf->setParentScope(body);
        holder.push_back(std::move(hf));
    }
}

void UiSelectFrame::populateFrames(CelestialBody* body)
{
    frameList_->clear();
    if (!body)
        return;

    // 若天体下尚无已注册的 Frame，先初始化
    ensureFramesInitialized(body);

    auto frames = aFindChildren(body, Frame::StaticType());
    for (auto* obj : frames)
    {
        auto* frame = static_cast<Frame*>(obj);
        auto* item = new QListWidgetItem(QString::fromStdString(frame->getName()));
        item->setData(Qt::UserRole, reinterpret_cast<qulonglong>(frame));
        frameList_->addItem(item);
    }

    if (frameList_->count() > 0)
        frameList_->setCurrentRow(0);
}

// ============================================================================
// 公共接口
// ============================================================================

Frame* UiSelectFrame::getSelectedFrame()
{
    auto* item = frameList_->currentItem();
    if (!item)
        return nullptr;
    return reinterpret_cast<Frame*>(item->data(Qt::UserRole).value<uintptr_t>());
}

void UiSelectFrame::setSelectedFrame(Frame* frame)
{
    if (!frame)
        return;

    auto* body = frame->getBody();
    if (body)
        setBody(body);

    for (int i = 0; i < frameList_->count(); ++i)
    {
        auto* item = frameList_->item(i);
        auto* f = reinterpret_cast<Frame*>(item->data(Qt::UserRole).value<uintptr_t>());
        if (f == frame)
        {
            frameList_->setCurrentRow(i);
            return;
        }
    }
}

CelestialBody* UiSelectFrame::getSelectedBody() const
{
    auto* item = bodyList_->currentItem();
    if (!item)
        return nullptr;
    return reinterpret_cast<CelestialBody*>(
        item->data(Qt::UserRole).value<uintptr_t>());
}

void UiSelectFrame::setBody(CelestialBody* body)
{
    if (!body)
        return;

    for (int i = 0; i < bodyList_->count(); ++i)
    {
        auto* item = bodyList_->item(i);
        auto* b = reinterpret_cast<CelestialBody*>(
            item->data(Qt::UserRole).value<uintptr_t>());
        if (b == body)
        {
            bodyList_->blockSignals(true);
            bodyList_->setCurrentRow(i);
            bodyList_->blockSignals(false);
            populateFrames(body);
            return;
        }
    }
}

// ============================================================================
// 槽
// ============================================================================

void UiSelectFrame::onBodySelected()
{
    auto* body = getSelectedBody();
    if (body)
        populateFrames(body);
    resolveAndEmit();
}

void UiSelectFrame::onFrameSelected()
{
    resolveAndEmit();
}

void UiSelectFrame::resolveAndEmit()
{
    auto* frame = getSelectedFrame();
    if (frame)
        emit frameChanged(frame);
}

// ============================================================================
// 便捷函数
// ============================================================================

Frame* aUiSelectFrame()
{
    QDialog dlg;
    dlg.setWindowTitle(QObject::tr(u8"选择坐标系"));
    auto* layout = new QVBoxLayout(&dlg);

    auto* frameSelect = new UiSelectFrame(&dlg);
    layout->addWidget(frameSelect);

    auto* buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dlg);
    layout->addWidget(buttons);
    QObject::connect(buttons, &QDialogButtonBox::accepted, &dlg, &QDialog::accept);
    QObject::connect(buttons, &QDialogButtonBox::rejected, &dlg, &QDialog::reject);

    if (dlg.exec() == QDialog::Accepted)
        return frameSelect->getSelectedFrame();

    return nullptr;
}

AST_NAMESPACE_END
