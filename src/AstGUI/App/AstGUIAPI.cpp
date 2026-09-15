///
/// @file      GuiAPI.cpp
/// @brief     ~
/// @details   ~
/// @author    axel
/// @date      2025-12-18
/// @copyright 版权所有 (C) 2025-present, ast项目.
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

#include "AstGUI/AstGUIAPI.hpp"
#include "AstGUI/UiMissionAnalysisWindow.hpp"
#include "AstUtil/GUI.hpp"
#include "AstUtil/FileSystem.hpp"
#include "AstCore/RunTime.hpp"
#include <QApplication>
#include <QGuiApplication>
#include <QFontDatabase>
#include <QStyleFactory>
#include <QTranslator>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QLibraryInfo>
#include <QDebug>

AST_NAMESPACE_BEGIN

/// @brief 各平台 Qt 平台插件名（QPA）
/// @details 此处列出常见平台的插件名，便于在不依赖 Qt 私有头文件的前提下探测插件是否存在。
#if defined(_WIN32)
static const char* const kPlatformPluginKeys[] = {
    "qwindows"
};
#elif defined(__APPLE__)
static const char* const kPlatformPluginKeys[] = {
    "qcocoa"
};
#elif defined(__linux__)
static const char* const kPlatformPluginKeys[] = {
    "qxcb", "qwayland"
};
#else
static const char* const kPlatformPluginKeys[] = {};
#endif


/// @brief 判断给定目录下是否存在任一平台插件
/// @param dir 待检查的目录，通常为平台插件目录（即 plugins/platforms）
static bool aPlatformPluginExistsInDir(const QString& dir)
{
    if(dir.isEmpty() || !QFileInfo(dir).isDir())
        return false;

    const QStringList entries = QDir(dir).entryList(QDir::Files);
    for(const QString& entry : entries)
    {
        QString baseName = QFileInfo(entry).completeBaseName();
        #if defined(__linux__)
        // Linux 下插件名为 libqxcb.so 形式，去掉 lib 前缀后再比较
        if(baseName.startsWith(QLatin1String("lib")))
            baseName = baseName.mid(3);
        #endif
        for(const char* key : kPlatformPluginKeys)
        {
            if(baseName == QLatin1String(key))
                return true;
        }
    }
    return false;
}

/// @brief 探测 Qt 平台插件是否可用
/// @details 缺少平台插件时构造 QApplication 会经由 qFatal 调用 abort() 直接终止进程，
///          该行为无法用 C++ 异常捕获，因此必须在构造之前先行探测。
///          探测范围与 Qt 自身的插件查找顺序保持一致：
///          QT_QPA_PLATFORM_PLUGIN_PATH 环境变量、可执行文件目录、Qt 安装目录。
/// @return 找到可用的平台插件返回 true
static bool aQtPlatformPluginAvailable()
{
    // 显式指定平台时尊重调用方的选择
    if(!qgetenv("QT_QPA_PLATFORM").isEmpty())
        return true;

    QStringList dirs;
    // QT_QPA_PLATFORM_PLUGIN_PATH 既可能指向插件根目录，也可能直接指向平台插件目录
    {
        const QByteArray pluginRoot = qgetenv("QT_QPA_PLATFORM_PLUGIN_PATH");
        if(!pluginRoot.isEmpty())
        {
            const QString root = QString::fromLocal8Bit(pluginRoot);
            dirs << root + QLatin1String("/platforms") << root;
        }
    }
    // 可执行文件目录，即发布包中与 exe 同级的 platforms/ 目录。
    // 注意此处不能用 QCoreApplication::applicationDirPath()：本函数在 QCoreApplication
    // 构造之前调用，那时 Qt 尚未记录可执行文件路径，只会返回空串。
    {
        const QString exeDir = QString::fromStdString(aExeDir());
        if(!exeDir.isEmpty())
            dirs << exeDir + QLatin1String("/platforms");
    }
    // Qt 安装目录
    {
        const QString qtPlugins = QLibraryInfo::location(QLibraryInfo::PluginsPath);
        if(!qtPlugins.isEmpty())
            dirs << qtPlugins + QLatin1String("/platforms");
    }

    for(const QString& dir : dirs)
    {
        if(aPlatformPluginExistsInDir(dir))
            return true;
    }
    return false;
}

bool aGuiAvailable()
{
    return qobject_cast<QApplication*>(QCoreApplication::instance()) != nullptr;
}

bool aInitAppAttributes()
{
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling); // 启用高DPI缩放
    QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);    // 启用高分辨率位图支持
    QApplication::setAttribute(Qt::AA_ShareOpenGLContexts);  // 共享OpenGL上下文
    return true;
}

static bool s_initAppAttributes = aInitAppAttributes();

errc_t aGUIInit()
{
    errc_t rc = 0;
    if(QCoreApplication::instance() == nullptr)
    {
        int argc = 0;
        char *argv[] = { nullptr };
        rc = aQAppInit(argc, argv);
    }
    return rc;
}


errc_t aQAppInit(int argc, char *argv[])
{
    auto coreApp = QCoreApplication::instance();
    if(!coreApp)
    {
        bool canDisplayGUI = aCanDisplayGUI();
        if (canDisplayGUI && aQtPlatformPluginAvailable())
        {
            aDebug(_("检测到GUI环境"));
            coreApp = new QApplication(argc, argv);
        }else{
            if(canDisplayGUI)
            {
                // 有显示环境但缺少平台插件，此时构造 QApplication 会直接 abort，
                // 故降级为 QCoreApplication：日志、翻译等功能仍可用，仅 GUI 功能不可用。
                const std::string pluginDir = (fs::path(aExeDir()) / "platforms").string();
                aWarning(_("未找到可用的 Qt 平台插件，请将平台插件部署到 '%s'"), pluginDir.c_str());
            }else{
                aDebug(_("未检测到GUI环境"));
            }
            coreApp = new QCoreApplication(argc, argv);
        }
    }
    static bool initialized = false;
    if (!initialized)
    {
        initialized = true;
        // 加载翻译文件
        {
            auto translator = new QTranslator(QCoreApplication::instance());
            QString qmPath = QCoreApplication::applicationDirPath() + "/Ast_zh.qm";
            if (!translator->load(qmPath)) {
                qmPath = QString::fromStdString(aDataDir()) + "/Ast_zh.qm";
                bool loaded = translator->load(qmPath);
                if (!loaded)
                {
                    aDebug(_("加载翻译文件失败: '%s'"), qmPath.toStdString().c_str());
                }
            }
            QCoreApplication::installTranslator(translator);
        }
        if (QGuiApplication* guiApp = qobject_cast<QGuiApplication *>(QCoreApplication::instance()))
        {
            // 加载自带的中文字体（桌面平台作为备选，WASM 平台必需）
            #ifdef A_WASM
            // wasm 不会存在data目录和exe目录分离的情况，所以直接使用相对路径
            QString fontPath = QStringLiteral("data/fonts/NotoSansSC-Regular.ttf");
            #else
            // 其他平台需要通过aDataDir获取data目录路径，避免其他软件调用ast库时的路径错误
            QString fontPath = QString::fromStdString(aDataDir()) + "/fonts/NotoSansSC-Regular.ttf";
            #endif
            int fontId = QFontDatabase::addApplicationFont(fontPath);
            if (fontId != -1) {
                QStringList families = QFontDatabase::applicationFontFamilies(fontId);
                if (!families.isEmpty()) {
                    guiApp->setFont(QFont(families.first()));
                }
            }
            else
            {
                aDebug(_("加载字体文件失败: '%s'"), fontPath.toStdString().c_str());
            }
        }
        // 加载默认主题样式
        if(QApplication* app = qobject_cast<QApplication *>(QCoreApplication::instance()))
        {
            QString qssPath = QCoreApplication::applicationDirPath() + "/data/style/default.qss";
            QFile file(qssPath);
            if (file.open(QFile::ReadOnly | QFile::Text))
            {
                app->setStyleSheet(QString::fromUtf8(file.readAll()));
                file.close();
            }
        }
        
    }
    aDebug(_("UI环境初始化完成"));
    return 0;
}

QMainWindow *aUiNewMainWindow()
{
    if(!aGuiAvailable())
    {
        aError(_("当前环境没有可用的 GUI（缺少 Qt 平台插件），无法创建主窗口"));
        return nullptr;
    }
    return new UiMissionAnalysisWindow();
}

AST_NAMESPACE_END

