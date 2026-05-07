///
/// @file      UiHPOPForceModel.hpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-04-22
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

#pragma once

#include "AstGlobal.h"
#include "AstGUI/UiObject.hpp"
#include <QVBoxLayout>
#include <QGroupBox>
#include <QComboBox>
#include <QRadioButton>
#include <QCheckBox>
#include <QListWidget>
#include <QPushButton>
#include <QLineEdit>
#include <QHBoxLayout>
#include <QLabel>
#include <QTabWidget>
#include <QFileDialog>

#include "AstGUI/UiDragForce.hpp"
#include "AstGUI/UiSolarRadiationPressure.hpp"
#include "AstGUI/UiGravityForce.hpp"
#include "AstGUI/UiThirdBodyForce.hpp"
#include "AstGUI/UiThirdBodyForceList.hpp"

AST_NAMESPACE_BEGIN

/*!
    @addtogroup 
    @{
*/

class HPOPForceModel;

class AST_GUI_API UiHPOPForceModel: public UiObject
{
    Q_OBJECT
public:
    UiHPOPForceModel(Object* object, QWidget *parent = nullptr);
    UiHPOPForceModel(QWidget *parent = nullptr);
    ~UiHPOPForceModel() = default;

    HPOPForceModel* getHPOPForceModel() const;
    void setHPOPForceModel(HPOPForceModel* hpop);
    void refreshUi();
    void apply();
protected:
    void setupUi();
private slots:
    void applyTo(HPOPForceModel* hpop);
private:
    // 布局
    QVBoxLayout* mainLayout_;
    QTabWidget* tabWidget_;
    
    // 引力模型标签页
    QWidget* gravityTab_;
    QVBoxLayout* gravityTabLayout_;
    UiGravityForce* gravityWidget_;
    
    // 三体引力配置
    QWidget* thirdBodyTab_;
    QVBoxLayout* thirdBodyTabLayout_;
    UiThirdBodyForceList* thirdBodyWidget_;
    
    // 大气阻力标签页
    QWidget* dragTab_;
    QVBoxLayout* dragTabLayout_;
    UiDragForce* dragWidget_;
    
    // 太阳光压标签页
    QWidget* srpTab_;
    QVBoxLayout* srpTabLayout_;
    UiSolarRadiationPressure* srpWidget_;
    

};


/*! @} */

AST_NAMESPACE_END