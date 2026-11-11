// 从 matplotlib mplutils.h 提取的工具函数 (去除 Python 依赖)
#pragma once
#include <cmath>
#include <cstddef>

inline int mpl_round_to_int(double v) {
    return (int)(v + ((v >= 0.0) ? 0.5 : -0.5));
}

inline double mpl_round(double v) {
    return (double)mpl_round_to_int(v);
}

// Path 命令代码 (与 Agg 兼容)
enum {
    AG_PATH_STOP     = 0,
    AG_PATH_MOVETO   = 1,
    AG_PATH_LINETO   = 2,
    AG_PATH_CURVE3   = 3,
    AG_PATH_CURVE4   = 4,
    AG_PATH_CLOSEPOLY = 0x4f
};
