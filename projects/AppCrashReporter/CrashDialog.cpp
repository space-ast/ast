///
/// @file      CrashDialog.cpp
/// @brief     崩溃报告对话框实现
/// @author    axel
/// @date      1.7.2026
/// @copyright 版权所有 (C) 2026-present, ast项目.

#include "CrashDialog.hpp"

#include <QApplication>
#include <QDialogButtonBox>
#include <QFile>
#include <QFileInfo>
#include <QHBoxLayout>
#include <QHttpMultiPart>
#include <QLabel>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QProcess>
#include <QPushButton>
#include <QScrollArea>
#include <QTextEdit>
#include <QTextStream>
#include <QVBoxLayout>
#include <QWindow>

// ============================================================================
// 构造 & 初始化
// ============================================================================

CrashDialog::CrashDialog(const QString& logPath, const QString& appPath,
                         const QString& uploadUrl, QWidget* parent)
    : QDialog(parent)
    , m_logPath(logPath)
    , m_appPath(appPath)
    , m_uploadUrl(uploadUrl)
    , m_network(new QNetworkAccessManager(this))
{
    parseLogFile();
    setupUi();
}

// ============================================================================
// 解析崩溃日志
// ============================================================================

void CrashDialog::parseLogFile()
{
    QFile file(m_logPath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;

    QTextStream in(&file);
    QString section;
    while (!in.atEnd())
    {
        QString line = in.readLine();

        if (line.contains("Crash Type"))
            m_crashType = line.section(':', 1).trimmed();
        else if (line.contains("Timestamp"))
            m_crashTime = line.section(':', 1).trimmed();
        else if (line.contains("PID"))
            m_pid = line.section(':', 1).trimmed();

        // 收集堆栈帧
        if (line.startsWith("  #") && line.contains("0x"))
            m_stackFrames << line.trimmed();

        // 最多收集 10 帧
        if (m_stackFrames.size() >= 10)
            break;
    }
}

// ============================================================================
// UI 构建
// ============================================================================

void CrashDialog::setupUi()
{
    setWindowTitle(QString::fromUtf8("程序遇到问题"));
    setMinimumSize(460, 420);
    setMaximumSize(560, 620);

    // 隐藏右上角关闭按钮 — 用户必须选择一个操作
    setWindowFlags(Qt::Dialog | Qt::CustomizeWindowHint | Qt::WindowTitleHint);

    auto* mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(12);
    mainLayout->setContentsMargins(24, 20, 24, 20);

    // ---- 图标 ----
    m_iconLabel = new QLabel(this);
    m_iconLabel->setAlignment(Qt::AlignCenter);
    m_iconLabel->setText(QString::fromUtf8("⚠️"));  // ⚠️
    QFont iconFont = m_iconLabel->font();
    iconFont.setPointSize(36);
    m_iconLabel->setFont(iconFont);
    mainLayout->addWidget(m_iconLabel);

    // ---- 主消息 ----
    m_messageLabel = new QLabel(this);
    m_messageLabel->setAlignment(Qt::AlignCenter);
    m_messageLabel->setWordWrap(true);
    m_messageLabel->setText(QString::fromUtf8(
        "<p style='font-size:14px;'>"
        "<b>程序遇到了问题，需要关闭。</b></p>"
        "<p style='font-size:12px; color:#666;'>"
        "此问题可能是由软件错误导致的。</p>"));
    m_messageLabel->setTextFormat(Qt::RichText);
    mainLayout->addWidget(m_messageLabel);

    // ---- 用户描述 ----
    auto* descLabel = new QLabel(QString::fromUtf8(
        "请描述您刚才进行什么操作时遇到了这个问题："), this);
    descLabel->setStyleSheet("font-size:12px;");
    mainLayout->addWidget(descLabel);

    m_descriptionEdit = new QTextEdit(this);
    m_descriptionEdit->setMaximumHeight(80);
    m_descriptionEdit->setPlaceholderText(QString::fromUtf8(
        "（可选）例如：点击导出按钮后程序闪退..."));
    mainLayout->addWidget(m_descriptionEdit);

    // ---- 详情折叠 ----
    m_detailsToggle = new QPushButton(QString::fromUtf8("▶ 查看详细技术信息"), this);
    m_detailsToggle->setFlat(true);
    m_detailsToggle->setCursor(Qt::PointingHandCursor);
    m_detailsToggle->setStyleSheet(
        "QPushButton { text-align: left; color: #0078D4; border: none; font-size:12px; }");
    connect(m_detailsToggle, &QPushButton::clicked, this, &CrashDialog::onToggleDetails);
    mainLayout->addWidget(m_detailsToggle);

    // 详情面板（默认隐藏）
    m_detailsPanel = new QWidget(this);
    auto* detailsLayout = new QVBoxLayout(m_detailsPanel);
    detailsLayout->setContentsMargins(0, 0, 0, 0);

    m_detailsContent = new QLabel(this);
    m_detailsContent->setWordWrap(true);
    m_detailsContent->setStyleSheet(
        "QLabel { background: #F5F5F5; border: 1px solid #DDD; "
        "border-radius: 4px; padding: 8px; font-size:11px; font-family: Consolas, monospace; }");
    m_detailsContent->setTextInteractionFlags(Qt::TextSelectableByMouse);

    // 构建详情内容
    QString details;
    details += QString::fromUtf8("异常类型：") + (m_crashType.isEmpty() ? QString::fromUtf8("未知") : m_crashType) + "\n";
    details += QString::fromUtf8("发生时间：") + m_crashTime + "\n";
    details += QString::fromUtf8("报告文件：") + m_logPath + "\n\n";
    details += QString::fromUtf8("调用栈：\n");

    if (m_stackFrames.isEmpty())
        details += QString::fromUtf8("  (无调用栈信息)\n");
    else
        for (const QString& frame : m_stackFrames)
            details += "  " + frame + "\n";

    m_detailsContent->setText(details);

    auto* scrollArea = new QScrollArea(this);
    scrollArea->setWidget(m_detailsContent);
    scrollArea->setWidgetResizable(true);
    scrollArea->setMaximumHeight(200);
    scrollArea->setStyleSheet("QScrollArea { border: none; }");
    detailsLayout->addWidget(scrollArea);

    m_detailsPanel->setVisible(false);
    mainLayout->addWidget(m_detailsPanel);

    // ---- 主按钮行 ----
    auto* btnLayout = new QHBoxLayout();
    btnLayout->setSpacing(12);

    m_sendBtn = new QPushButton(QString::fromUtf8("✉ 发送反馈（推荐）"), this);
    m_sendBtn->setMinimumHeight(36);
    m_sendBtn->setCursor(Qt::PointingHandCursor);
    m_sendBtn->setStyleSheet(
        "QPushButton { background: #0078D4; color: white; border: none; "
        "border-radius: 4px; padding: 8px 16px; font-size:13px; font-weight: bold; }"
        "QPushButton:hover { background: #106ABE; }"
        "QPushButton:disabled { background: #CCC; }");
    connect(m_sendBtn, &QPushButton::clicked, this, &CrashDialog::onSendReport);

    m_restartBtn = new QPushButton(QString::fromUtf8("↻ 重启程序"), this);
    m_restartBtn->setMinimumHeight(36);
    m_restartBtn->setCursor(Qt::PointingHandCursor);
    m_restartBtn->setStyleSheet(
        "QPushButton { background: #F0F0F0; color: #333; border: 1px solid #CCC; "
        "border-radius: 4px; padding: 8px 16px; font-size:13px; }"
        "QPushButton:hover { background: #E0E0E0; }");
    connect(m_restartBtn, &QPushButton::clicked, this, &CrashDialog::onRestart);

    btnLayout->addWidget(m_sendBtn);
    btnLayout->addWidget(m_restartBtn);
    mainLayout->addLayout(btnLayout);

    // 如果没有配置上传 URL，隐藏发送按钮
    if (m_uploadUrl.isEmpty())
        m_sendBtn->setVisible(false);

    // 如果没有配置应用路径，隐藏重启按钮
    if (m_appPath.isEmpty())
        m_restartBtn->setVisible(false);

    // ---- 关闭按钮 ----
    m_closeBtn = new QPushButton(QString::fromUtf8("关闭"), this);
    m_closeBtn->setFlat(true);
    m_closeBtn->setCursor(Qt::PointingHandCursor);
    m_closeBtn->setStyleSheet(
        "QPushButton { color: #999; border: none; font-size:12px; }"
        "QPushButton:hover { color: #333; }");
    connect(m_closeBtn, &QPushButton::clicked, qApp, &QApplication::quit);
    mainLayout->addWidget(m_closeBtn, 0, Qt::AlignCenter);
}

// ============================================================================
// 按钮槽函数
// ============================================================================

void CrashDialog::onSendReport()
{
    if (m_uploadUrl.isEmpty())
        return;

    setUploadingState(true);

    // 构建 multipart 上传数据
    QHttpMultiPart* multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType, this);

    // 用户描述
    QString description = m_descriptionEdit->toPlainText().trimmed();
    if (!description.isEmpty())
    {
        QHttpPart descPart;
        descPart.setHeader(QNetworkRequest::ContentDispositionHeader,
                           QVariant("form-data; name=\"description\""));
        descPart.setBody(description.toUtf8());
        multiPart->append(descPart);
    }

    // 崩溃日志文件
    QFile* logFile = new QFile(m_logPath, multiPart); // multiPart 作 parent，确保释放
    if (logFile->open(QIODevice::ReadOnly))
    {
        QHttpPart filePart;
        filePart.setHeader(QNetworkRequest::ContentTypeHeader,
                           QVariant("text/plain"));
        filePart.setHeader(QNetworkRequest::ContentDispositionHeader,
                           QVariant("form-data; name=\"crashlog\"; filename=\""
                                    + QFileInfo(m_logPath).fileName() + "\""));
        filePart.setBodyDevice(logFile);
        multiPart->append(filePart);
    }

    QUrl url(m_uploadUrl);
    QNetworkRequest request(url);
    QNetworkReply* reply = m_network->post(request, multiPart);
    multiPart->setParent(reply); // reply 析构时释放

    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        reply->deleteLater();
        setUploadingState(false);

        if (reply->error() == QNetworkReply::NoError)
        {
            m_sendBtn->setText(QString::fromUtf8("✔ 感谢您的反馈！"));
            m_sendBtn->setEnabled(false);
            m_messageLabel->setText(QString::fromUtf8(
                "<p style='font-size:14px; color:#107C10;'><b>"
                "反馈已发送，感谢您的帮助！</b></p>"));
        }
        else
        {
            m_messageLabel->setText(m_messageLabel->text() +
                QString::fromUtf8("<p style='color:#D83B01; font-size:12px;'>"
                                  "发送失败，请检查网络连接。</p>"));
        }
    });
}

void CrashDialog::onRestart()
{
    if (!m_appPath.isEmpty())
        QProcess::startDetached(m_appPath, QStringList());

    qApp->quit();
}

void CrashDialog::onToggleDetails()
{
    bool visible = !m_detailsPanel->isVisible();
    m_detailsPanel->setVisible(visible);
    m_detailsToggle->setText(visible
        ? QString::fromUtf8("▼ 隐藏详细技术信息")
        : QString::fromUtf8("▶ 查看详细技术信息"));
}

void CrashDialog::setUploadingState(bool uploading)
{
    m_sendBtn->setEnabled(!uploading);
    m_sendBtn->setText(uploading
        ? QString::fromUtf8("⏳ 正在发送...")
        : QString::fromUtf8("✉ 发送反馈（推荐）"));
}
