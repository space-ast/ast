///
/// @file      testSurface3D.cpp
/// @brief     Test Qwt3D SurfacePlot with raw data, no matplot/backend
/// @details   ~
/// @author    axel
/// @date      2026-05-20
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

#include <QApplication>
#include <qwt3d_surfaceplot.h>

using namespace Qwt3D;

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    SurfacePlot plot;
    plot.resize(800, 600);
    plot.show();  // 必须先 show() 初始化 OpenGL 上下文，再加载数据

    plot.setTitle("Qwt3D SurfacePlot — Direct Test");

    int cols = 50, rows = 50;
    double minx = -3.0, maxx = 3.0;
    double miny = -3.0, maxy = 3.0;

    std::vector<Triple*> triples(cols);
    std::vector<std::vector<Triple>> tStore(cols, std::vector<Triple>(rows));
    for (int c = 0; c < cols; ++c) {
        double x = minx + (maxx - minx) * c / (cols - 1);
        for (int r = 0; r < rows; ++r) {
            double y = miny + (maxy - miny) * r / (rows - 1);
            tStore[c][r] = Triple(x, y, sin(x) * cos(y));
        }
        triples[c] = tStore[c].data();
    }

    plot.loadFromData(triples.data(), cols, rows);
    plot.setFloorStyle(NOFLOOR);
    plot.setRotation(30, 0, 15);
    plot.setZoom(0.9);

    return app.exec();
}
