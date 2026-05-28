#include "ast/SpiceZpr.hpp"
#include "ast/Vector.hpp"
#include "ast/Matrix.hpp"
#include "ast/Constants.h"
#include <cmath>
#include <cstdio>

AST_USING_NAMESPACE

int main()
{
    // 示例：球坐标转直角坐标
    // 地球平均半径约6371km
    double radius = 6371000.0;  // 米
    double lon = kPI / 4;    // 45度经度
    double lat = 0.0;           // 赤道
    
    Vector3d rectan;
    latrec(radius, lon, lat, rectan);
    
    printf("球坐标 (r=%.0f m, lon=%.2f rad, lat=%.2f rad)\n", radius, lon, lat);
    printf("直角坐标: X=%.2f, Y=%.2f, Z=%.2f\n", rectan[0], rectan[1], rectan[2]);
    
    return 0;
}
