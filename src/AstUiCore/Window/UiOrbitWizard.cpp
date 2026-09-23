///
/// @file      UiOrbitWizard.cpp
/// @brief     轨道向导对话框实现
/// @author    axel
/// @date      2026-05-08
/// @copyright 版权所有 (C) 2026-present, ast项目.
///
/// ast项目（https://github.com/space-ast/ast）
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

#include "UiOrbitWizard.hpp"
#include <QFileDialog>

AST_NAMESPACE_BEGIN

UiOrbitWizard::UiOrbitWizard(QWidget *parent)
    : QDialog(parent)
    , currentColor(Qt::cyan)
{
    setupUi();
    setupConnections();
}

UiOrbitWizard::~UiOrbitWizard()
{
}

void UiOrbitWizard::setupUi()
{
    setWindowTitle("Orbit Wizard");
    setMinimumSize(900, 600);

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    QHBoxLayout* topLayout = new QHBoxLayout();
    
    QGroupBox* leftGroup = new QGroupBox(this);
    QVBoxLayout* leftLayout = new QVBoxLayout(leftGroup);
    
    QHBoxLayout* typeLayout = new QHBoxLayout();
    QLabel* typeLabel = new QLabel("Type:", this);
    typeComboBox = new QComboBox(this);
    typeComboBox->addItem("Sun Synchronous");
    typeComboBox->addItem("Geostationary");
    typeComboBox->addItem("Polar");
    typeComboBox->addItem("LEO");
    typeComboBox->addItem("MEO");
    typeComboBox->addItem("GEO");
    typeComboBox->setCurrentText("Sun Synchronous");
    typeLayout->addWidget(typeLabel);
    typeLayout->addWidget(typeComboBox);
    leftLayout->addLayout(typeLayout);
    
    QHBoxLayout* nameLayout = new QHBoxLayout();
    QLabel* nameLabel = new QLabel("Satellite Name:", this);
    satelliteNameEdit = new QLineEdit("Satellite 1", this);
    nameLayout->addWidget(nameLabel);
    nameLayout->addWidget(satelliteNameEdit);
    leftLayout->addLayout(nameLayout);
    
    geometryGroup = new QGroupBox("Geometry Definition", this);
    QVBoxLayout* geometryLayout = new QVBoxLayout(geometryGroup);
    
    QHBoxLayout* inclinationLayout = new QHBoxLayout();
    inclinationRadio = new QRadioButton("Inclination:", this);
    inclinationEdit = new QLineEdit("97.0346 deg", this);
    inclinationEdit->setReadOnly(true);
    inclinationEdit->setFixedWidth(120);
    inclinationSlider = new QSlider(Qt::Horizontal, this);
    inclinationSlider->setRange(0, 180);
    inclinationSlider->setValue(97);
    inclinationLayout->addWidget(inclinationRadio);
    inclinationLayout->addWidget(inclinationEdit);
    inclinationLayout->addWidget(inclinationSlider);
    geometryLayout->addLayout(inclinationLayout);
    
    QHBoxLayout* altitudeLayout = new QHBoxLayout();
    altitudeRadio = new QRadioButton("Altitude:", this);
    altitudeEdit = new QLineEdit("400 km", this);
    altitudeEdit->setReadOnly(true);
    altitudeEdit->setFixedWidth(120);
    altitudeSlider = new QSlider(Qt::Horizontal, this);
    altitudeSlider->setRange(100, 1000);
    altitudeSlider->setValue(400);
    altitudeLayout->addWidget(altitudeRadio);
    altitudeLayout->addWidget(altitudeEdit);
    altitudeLayout->addWidget(altitudeSlider);
    geometryLayout->addLayout(altitudeLayout);
    
    leftLayout->addWidget(geometryGroup);
    altitudeRadio->setChecked(true);
    
    nodeGroup = new QGroupBox("Node Definition", this);
    QVBoxLayout* nodeLayout = new QVBoxLayout(nodeGroup);
    
    QHBoxLayout* ascendingLayout = new QHBoxLayout();
    ascendingNodeRadio = new QRadioButton("Local Time of Ascending Node:", this);
    ascendingNodeEdit = new QLineEdit("00:00:00.000 HMS", this);
    ascendingNodeEdit->setReadOnly(true);
    ascendingNodeEdit->setFixedWidth(150);
    ascendingNodeSlider = new QSlider(Qt::Horizontal, this);
    ascendingNodeSlider->setRange(0, 240);
    ascendingNodeSlider->setValue(0);
    ascendingLayout->addWidget(ascendingNodeRadio);
    ascendingLayout->addWidget(ascendingNodeEdit);
    ascendingLayout->addWidget(ascendingNodeSlider);
    nodeLayout->addLayout(ascendingLayout);
    
    QHBoxLayout* descendingLayout = new QHBoxLayout();
    descendingNodeRadio = new QRadioButton("Local Time of Descending Node:", this);
    descendingNodeEdit = new QLineEdit("12:00:00.000 HMS", this);
    descendingNodeEdit->setReadOnly(true);
    descendingNodeEdit->setFixedWidth(150);
    descendingNodeSlider = new QSlider(Qt::Horizontal, this);
    descendingNodeSlider->setRange(0, 240);
    descendingNodeSlider->setValue(120);
    descendingLayout->addWidget(descendingNodeRadio);
    descendingLayout->addWidget(descendingNodeEdit);
    descendingLayout->addWidget(descendingNodeSlider);
    nodeLayout->addLayout(descendingLayout);
    
    leftLayout->addWidget(nodeGroup);
    descendingNodeRadio->setChecked(true);
    
    defaultObjectCheckBox = new QCheckBox("Use this tool for default object creation", this);
    leftLayout->addWidget(defaultObjectCheckBox);
    
    leftLayout->addStretch();
    topLayout->addWidget(leftGroup);
    leftGroup->setFixedWidth(400);
    
    QVBoxLayout* centerLayout = new QVBoxLayout();
    
    timePeriodGroup = new QGroupBox("Analysis Time Period", this);
    QHBoxLayout* timePeriodLayout = new QHBoxLayout(timePeriodGroup);
    QLabel* intervalLabel = new QLabel("Interval:", this);
    intervalComboBox = new QComboBox(this);
    intervalComboBox->addItem("Scenario 7 AnalysisInterval");
    intervalComboBox->setFixedWidth(250);
    timePeriodLayout->addWidget(intervalLabel);
    timePeriodLayout->addWidget(intervalComboBox);
    centerLayout->addWidget(timePeriodGroup);
    
    mapFrame = new QFrame(this);
    mapFrame->setFrameStyle(QFrame::Box | QFrame::Sunken);
    mapFrame->setStyleSheet("background-color: #1a1a2e;");
    mapFrame->setFixedHeight(350);
    
    QHBoxLayout* mapInfoLayout = new QHBoxLayout();
    latLonLabel = new QLabel("Lat,Lon:  (88.73239,-59.78910)", this);
    displayingLabel = new QLabel("Displaying: 1 Rev", this);
    mapInfoLayout->addWidget(latLonLabel);
    mapInfoLayout->addStretch();
    mapInfoLayout->addWidget(displayingLabel);
    centerLayout->addLayout(mapInfoLayout);
    centerLayout->addWidget(mapFrame);
    
    topLayout->addLayout(centerLayout);
    topLayout->setStretchFactor(centerLayout, 1);
    
    QGroupBox* rightGroup = new QGroupBox("Graphics", this);
    QVBoxLayout* rightLayout = new QVBoxLayout(rightGroup);
    
    showAllObjectsCheckBox = new QCheckBox("Show All Objects", this);
    rightLayout->addWidget(showAllObjectsCheckBox);
    
    QHBoxLayout* colorLayout = new QHBoxLayout();
    QLabel* colorLabel = new QLabel("Color:", this);
    colorButton = new QPushButton(this);
    colorButton->setFixedSize(100, 25);
    colorButton->setStyleSheet(QString("background-color: %1").arg(currentColor.name()));
    colorLayout->addWidget(colorLabel);
    colorLayout->addWidget(colorButton);
    rightLayout->addLayout(colorLayout);
    
    QHBoxLayout* modelLayout = new QHBoxLayout();
    QLabel* modelLabel = new QLabel("3D Model:", this);
    modelEdit = new QLineEdit("satellite.glb", this);
    modelBrowseButton = new QPushButton("...", this);
    modelBrowseButton->setFixedWidth(30);
    modelLayout->addWidget(modelLabel);
    modelLayout->addWidget(modelEdit);
    modelLayout->addWidget(modelBrowseButton);
    rightLayout->addLayout(modelLayout);
    
    rightLayout->addStretch();
    topLayout->addWidget(rightGroup);
    rightGroup->setFixedWidth(250);
    
    mainLayout->addLayout(topLayout);
    
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();
    
    okButton = new QPushButton("OK", this);
    cancelButton = new QPushButton("Cancel", this);
    applyButton = new QPushButton("Apply", this);
    helpButton = new QPushButton("Help", this);
    
    buttonLayout->addWidget(okButton);
    buttonLayout->addWidget(cancelButton);
    buttonLayout->addWidget(applyButton);
    buttonLayout->addWidget(helpButton);
    
    mainLayout->addLayout(buttonLayout);
}

void UiOrbitWizard::setupConnections()
{
    connect(inclinationRadio, &QRadioButton::clicked, this, &UiOrbitWizard::onInclinationRadioClicked);
    connect(altitudeRadio, &QRadioButton::clicked, this, &UiOrbitWizard::onAltitudeRadioClicked);
    connect(ascendingNodeRadio, &QRadioButton::clicked, this, &UiOrbitWizard::onAscendingNodeRadioClicked);
    connect(descendingNodeRadio, &QRadioButton::clicked, this, &UiOrbitWizard::onDescendingNodeRadioClicked);
    
    connect(inclinationSlider, &QSlider::valueChanged, this, &UiOrbitWizard::onInclinationSliderChanged);
    connect(altitudeSlider, &QSlider::valueChanged, this, &UiOrbitWizard::onAltitudeSliderChanged);
    connect(ascendingNodeSlider, &QSlider::valueChanged, this, &UiOrbitWizard::onAscendingNodeSliderChanged);
    connect(descendingNodeSlider, &QSlider::valueChanged, this, &UiOrbitWizard::onDescendingNodeSliderChanged);
    
    connect(colorButton, &QPushButton::clicked, this, &UiOrbitWizard::onColorButtonClicked);
    connect(modelBrowseButton, &QPushButton::clicked, this, &UiOrbitWizard::onModelBrowseClicked);
    
    connect(okButton, &QPushButton::clicked, this, &UiOrbitWizard::onOkClicked);
    connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);
    connect(applyButton, &QPushButton::clicked, this, &UiOrbitWizard::onApplyClicked);
}

void UiOrbitWizard::onInclinationRadioClicked()
{
    inclinationSlider->setEnabled(true);
    altitudeSlider->setEnabled(false);
}

void UiOrbitWizard::onAltitudeRadioClicked()
{
    altitudeSlider->setEnabled(true);
    inclinationSlider->setEnabled(false);
}

void UiOrbitWizard::onAscendingNodeRadioClicked()
{
    ascendingNodeSlider->setEnabled(true);
    descendingNodeSlider->setEnabled(false);
}

void UiOrbitWizard::onDescendingNodeRadioClicked()
{
    descendingNodeSlider->setEnabled(true);
    ascendingNodeSlider->setEnabled(false);
}

void UiOrbitWizard::onInclinationSliderChanged(int value)
{
    double inclination = value + 0.0346;
    inclinationEdit->setText(QString("%1 deg").arg(inclination, 0, 'f', 4));
}

void UiOrbitWizard::onAltitudeSliderChanged(int value)
{
    altitudeEdit->setText(QString("%1 km").arg(value));
}

void UiOrbitWizard::onAscendingNodeSliderChanged(int value)
{
    int hours = value / 10;
    int minutes = (value % 10) * 6;
    ascendingNodeEdit->setText(QString("%1:%2:%3.000 HMS")
        .arg(hours, 2, 10, QChar('0'))
        .arg(minutes, 2, 10, QChar('0'))
        .arg("00"));
}

void UiOrbitWizard::onDescendingNodeSliderChanged(int value)
{
    int hours = value / 10;
    int minutes = (value % 10) * 6;
    descendingNodeEdit->setText(QString("%1:%2:%3.000 HMS")
        .arg(hours, 2, 10, QChar('0'))
        .arg(minutes, 2, 10, QChar('0'))
        .arg("00"));
}

void UiOrbitWizard::onColorButtonClicked()
{
    QColor color = QColorDialog::getColor(currentColor, this, "Select Color");
    if (color.isValid()) {
        currentColor = color;
        colorButton->setStyleSheet(QString("background-color: %1").arg(currentColor.name()));
    }
}

void UiOrbitWizard::onModelBrowseClicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Select 3D Model", "", 
        "GLB Files (*.glb);;GLTF Files (*.gltf);;All Files (*.*)");
    if (!fileName.isEmpty()) {
        modelEdit->setText(QFileInfo(fileName).fileName());
    }
}

void UiOrbitWizard::onApplyClicked()
{
}

void UiOrbitWizard::onOkClicked()
{
    accept();
}

AST_NAMESPACE_END