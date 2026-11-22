///
/// @file      plugin.cpp
/// @brief     界面工具插件示例：向主程序注册一个挂到“工具”菜单里的工具
/// @details   本动态库在被点击之前不会被加载；工具清单来自下方嵌入的元数据
/// @author    axel
/// @date      2026-11-22
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

#include "AstGlobal.h"
#include "AstUtil/PluginInterface.hpp"
#include "AstUiUtil/UiToolProvider.hpp"

#include <QApplication>
#include <QDialog>
#include <QLabel>
#include <QPushButton>
#include <QScreen>
#include <QStyle>
#include <QVBoxLayout>

AST_USING_NAMESPACE

namespace
{

/// @brief 示例工具提供者
/// @details 一个插件可以承载多个工具，invoke() 按 key 分派
class ExampleToolProvider : public PluginInstance, public IUiToolProvider
{
public:
    void invoke(StringView key, QWidget* parent) override
    {
        QDialog dialog(parent);
        dialog.setWindowTitle(QObject::tr("插件工具"));
        dialog.setMinimumWidth(360);

        auto* layout = new QVBoxLayout(&dialog);

        QString message;
        if (key == "hello")
        {
            message = QObject::tr("你好！这个对话框由插件动态库提供。\n"
                                  "在你点击菜单之前，该动态库并没有被加载。");
        }
        else
        {
            message = QObject::tr("未知工具：%1")
                          .arg(QString::fromUtf8(key.data(), static_cast<int>(key.size())));
        }

        auto* label = new QLabel(message, &dialog);
        label->setWordWrap(true);
        layout->addWidget(label);

        auto* closeButton = new QPushButton(QObject::tr("关闭"), &dialog);
        QObject::connect(closeButton, &QPushButton::clicked, &dialog, &QDialog::accept);
        layout->addWidget(closeButton);

        // 以主窗口为基准居中显示：QDialog 是顶层窗口，geometry 使用全局坐标
        const QSize dialogSize(420, 180);
        QRect anchor = QApplication::primaryScreen()->availableGeometry();
        if (parent)
            anchor = parent->window()->frameGeometry();
        dialog.setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter,
                                               dialogSize, anchor));
        // 位置已显式指定，避免 QDialog::exec() 首次显示时再按父窗口重新调整
        dialog.setAttribute(Qt::WA_Moved, true);

        dialog.exec();
    }
};

} // namespace

/// @brief 插件元数据：工具清单在扫描阶段即可读出，无需加载动态库
AST_PLUGIN_METADATA_JSON(IUiToolProvider, R"json(
{
  "api": 1,
  "tools": {
    "hello": {
      "title":   { "zh": "插件工具示例", "en": "Plugin Tool Example" },
      "tooltip": { "zh": "插件工具示例，点击后显示一个对话框", "en": "Plugin Tool Example, Click to Show Dialog" },
      "order": 900
    }
  }
}
)json")

/// @brief 插件实例入口
AST_PLUGIN_INSTANCE(ExampleToolProvider)
