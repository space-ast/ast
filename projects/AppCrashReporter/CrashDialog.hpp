///
/// @file      CrashDialog.hpp
/// @brief     崩溃报告对话框
/// @author    axel
/// @date      1.7.2026
/// @copyright 版权所有 (C) 2026-present, ast项目.

#pragma once

#include <QDialog>
#include <QString>

class QLabel;
class QPushButton;
class QTextEdit;
class QNetworkAccessManager;

/// @brief 崩溃报告对话框（面向最终用户）
class CrashDialog : public QDialog
{
    Q_OBJECT

public:
    /// @param logPath  崩溃日志文件路径
    /// @param appPath  主程序路径（用于重启，可为空）
    /// @param uploadUrl 上传 URL（用于发送反馈，可为空）
    CrashDialog(const QString& logPath, const QString& appPath,
                const QString& uploadUrl, QWidget* parent = nullptr);

    CrashDialog(const CrashDialog&) = delete;
    CrashDialog& operator=(const CrashDialog&) = delete;

private slots:
    void onSendReport();
    void onRestart();
    void onToggleDetails();

private:
    void setupUi();
    void parseLogFile();
    void setUploadingState(bool uploading);

    QString m_logPath;
    QString m_appPath;
    QString m_uploadUrl;

    // 从日志中解析的信息
    QString m_crashType{};
    QString m_crashTime{};
    QString m_pid{};
    QStringList m_stackFrames{};

    // UI 组件
    QLabel* m_iconLabel{};
    QLabel* m_messageLabel{};
    QTextEdit* m_descriptionEdit{};
    QPushButton* m_detailsToggle{};
    QWidget* m_detailsPanel{};
    QLabel* m_detailsContent{};
    QPushButton* m_sendBtn{};
    QPushButton* m_restartBtn{};
    QPushButton* m_closeBtn{};

    QNetworkAccessManager* m_network;
};
