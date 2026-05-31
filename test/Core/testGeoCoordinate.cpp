#include <gtest/gtest.h>
#include "ast/GeoCoordinate.hpp"
#include "ast/AstTestMacro.h"
#include "ast/Math.hpp"
#include <cmath>

using namespace ast;

// WGS84 扁率
// 使用库中定义的 WGS84 地球扁率

// 测试参数构造函数
TEST(GeoCoordinateTest, ParameterConstructor)
{
    GeoCoordinate coord(deg2rad(30.0), deg2rad(120.0), 100.0);
    EXPECT_NEAR(coord.latitude(), deg2rad(30.0), 1e-14);
    EXPECT_NEAR(coord.longitude(), deg2rad(120.0), 1e-14);
    EXPECT_DOUBLE_EQ(coord.altitude(), 100.0);
}

// 测试setter方法
TEST(GeoCoordinateTest, Setters)
{
    GeoCoordinate coord(deg2rad(0.0), deg2rad(0.0), 0.0);
    coord.setLatitude(deg2rad(35.0));
    coord.setLongitude(deg2rad(117.0));
    coord.setAltitude(50.0);
    
    EXPECT_NEAR(coord.latitude(), deg2rad(35.0), 1e-14);
    EXPECT_NEAR(coord.longitude(), deg2rad(117.0), 1e-14);
    EXPECT_DOUBLE_EQ(coord.altitude(), 50.0);
}

// 测试引用访问器
TEST(GeoCoordinateTest, ReferenceAccessors)
{
    GeoCoordinate coord(deg2rad(0.0), deg2rad(0.0), 0.0);
    coord.latitude() = deg2rad(40.0);
    coord.longitude() = deg2rad(117.0);
    coord.altitude() = 200.0;
    
    EXPECT_NEAR(coord.latitude(), deg2rad(40.0), 1e-14);
    EXPECT_NEAR(coord.longitude(), deg2rad(117.0), 1e-14);
    EXPECT_DOUBLE_EQ(coord.altitude(), 200.0);
}

// 测试天顶方向向量（从地心指向地表点的方向，ECEF坐标系）
TEST(GeoCoordinateTest, GetZenith)
{
    // 赤道本初子午线 (0°, 0°) - 天顶指向X轴正方向
    {
        GeoCoordinate coord(0.0, 0.0, 0.0);
        Vector3d zenith = coord.getZenith();
        EXPECT_NEAR(zenith.x(), 1.0, 1e-14);
        EXPECT_NEAR(zenith.y(), 0.0, 1e-14);
        EXPECT_NEAR(zenith.z(), 0.0, 1e-14);
    }
    
    // 赤道东经90° - 天顶指向Y轴正方向
    {
        GeoCoordinate coord(0.0, deg2rad(90.0), 0.0);
        Vector3d zenith = coord.getZenith();
        EXPECT_NEAR(zenith.x(), 0.0, 1e-14);
        EXPECT_NEAR(zenith.y(), 1.0, 1e-14);
        EXPECT_NEAR(zenith.z(), 0.0, 1e-14);
    }
    
    // 北极点 (90°N, 任意经度) - 天顶指向Z轴正方向
    {
        GeoCoordinate coord(deg2rad(90.0), 0.0, 0.0);
        Vector3d zenith = coord.getZenith();
        EXPECT_NEAR(zenith.x(), 0.0, 1e-14);
        EXPECT_NEAR(zenith.y(), 0.0, 1e-14);
        EXPECT_NEAR(zenith.z(), 1.0, 1e-14);
    }
    
    // 南极点 (-90°S, 任意经度) - 天顶指向Z轴负方向
    {
        GeoCoordinate coord(deg2rad(-90.0), 0.0, 0.0);
        Vector3d zenith = coord.getZenith();
        EXPECT_NEAR(zenith.x(), 0.0, 1e-14);
        EXPECT_NEAR(zenith.y(), 0.0, 1e-14);
        EXPECT_NEAR(zenith.z(), -1.0, 1e-14);
    }
    
    // 北半球中纬度 (45°N, 90°E)
    {
        GeoCoordinate coord(deg2rad(45.0), deg2rad(90.0), 0.0);
        Vector3d zenith = coord.getZenith();
        EXPECT_NEAR(zenith.norm(), 1.0, 1e-14);
        EXPECT_NEAR(zenith.x(), 0.0, 1e-14);
        EXPECT_NEAR(zenith.y(), std::cos(deg2rad(45.0)), 1e-14);
        EXPECT_NEAR(zenith.z(), std::sin(deg2rad(45.0)), 1e-14);
    }
    
    // 南半球中纬度 (-45°S, 90°E)
    {
        GeoCoordinate coord(deg2rad(-45.0), deg2rad(90.0), 0.0);
        Vector3d zenith = coord.getZenith();
        EXPECT_NEAR(zenith.norm(), 1.0, 1e-14);
        EXPECT_NEAR(zenith.x(), 0.0, 1e-14);
        EXPECT_NEAR(zenith.y(), std::cos(deg2rad(-45.0)), 1e-14);
        EXPECT_NEAR(zenith.z(), std::sin(deg2rad(-45.0)), 1e-14);
    }
}

// 测试天底方向向量
TEST(GeoCoordinateTest, GetNadir)
{
    GeoCoordinate coord(0.0, 0.0, 0.0);
    Vector3d zenith = coord.getZenith();
    Vector3d nadir = coord.getNadir();
    
    EXPECT_NEAR(nadir.x(), -zenith.x(), 1e-14);
    EXPECT_NEAR(nadir.y(), -zenith.y(), 1e-14);
    EXPECT_NEAR(nadir.z(), -zenith.z(), 1e-14);
}

// 测试东方向向量（切向方向，垂直于天顶，沿经度增大方向）
TEST(GeoCoordinateTest, GetEast)
{
    // 经度0度 - 东方向指向Y轴正方向
    {
        GeoCoordinate coord(0.0, 0.0, 0.0);
        Vector3d east = coord.getEast();
        EXPECT_NEAR(east.x(), 0.0, 1e-14);
        EXPECT_NEAR(east.y(), 1.0, 1e-14);
        EXPECT_NEAR(east.z(), 0.0, 1e-14);
    }
    
    // 经度90度 - 东方向指向X轴负方向
    {
        GeoCoordinate coord(0.0, deg2rad(90.0), 0.0);
        Vector3d east = coord.getEast();
        EXPECT_NEAR(east.x(), -1.0, 1e-14);
        EXPECT_NEAR(east.y(), 0.0, 1e-14);
        EXPECT_NEAR(east.z(), 0.0, 1e-14);
    }
    
    // 南半球东方向
    {
        GeoCoordinate coord(deg2rad(-30.0), deg2rad(45.0), 0.0);
        Vector3d east = coord.getEast();
        EXPECT_NEAR(east.norm(), 1.0, 1e-14);
        EXPECT_NEAR(east.x(), -std::sin(deg2rad(45.0)), 1e-14);
        EXPECT_NEAR(east.y(), std::cos(deg2rad(45.0)), 1e-14);
        EXPECT_NEAR(east.z(), 0.0, 1e-14);
    }
}

// 测试西方向向量
TEST(GeoCoordinateTest, GetWest)
{
    GeoCoordinate coord(0.0, 0.0, 0.0);
    Vector3d east = coord.getEast();
    Vector3d west = coord.getWest();
    
    EXPECT_NEAR(west.x(), -east.x(), 1e-14);
    EXPECT_NEAR(west.y(), -east.y(), 1e-14);
    EXPECT_NEAR(west.z(), -east.z(), 1e-14);
}

// 测试北方向向量（切向方向，垂直于天顶，沿纬度增大方向）
TEST(GeoCoordinateTest, GetNorth)
{
    // 赤道 - 北方向指向Z轴正方向
    {
        GeoCoordinate coord(0.0, 0.0, 0.0);
        Vector3d north = coord.getNorth();
        EXPECT_NEAR(north.x(), 0.0, 1e-14);
        EXPECT_NEAR(north.y(), 0.0, 1e-14);
        EXPECT_NEAR(north.z(), 1.0, 1e-14);
    }
    
    // 北半球中纬度 (45°N, 0°E)
    {
        GeoCoordinate coord(deg2rad(45.0), 0.0, 0.0);
        Vector3d north = coord.getNorth();
        EXPECT_NEAR(north.norm(), 1.0, 1e-14);
        EXPECT_NEAR(north.x(), -std::sin(deg2rad(45.0)), 1e-14);
        EXPECT_NEAR(north.y(), 0.0, 1e-14);
        EXPECT_NEAR(north.z(), std::cos(deg2rad(45.0)), 1e-14);
    }
    
    // 南半球中纬度 (-45°S, 0°E)
    {
        GeoCoordinate coord(deg2rad(-45.0), 0.0, 0.0);
        Vector3d north = coord.getNorth();
        EXPECT_NEAR(north.norm(), 1.0, 1e-14);
        EXPECT_NEAR(north.x(), -std::sin(deg2rad(-45.0)), 1e-14);
        EXPECT_NEAR(north.y(), 0.0, 1e-14);
        EXPECT_NEAR(north.z(), std::cos(deg2rad(-45.0)), 1e-14);
    }
    
    // 高纬度（避开精确的极点，测试极点附近行为）
    {
        GeoCoordinate coord(deg2rad(89.0), 0.0, 0.0);
        Vector3d north = coord.getNorth();
        EXPECT_NEAR(north.norm(), 1.0, 1e-14);
        EXPECT_NEAR(north.x(), -std::sin(deg2rad(89.0)), 1e-14);
        EXPECT_NEAR(north.y(), 0.0, 1e-14);
        EXPECT_NEAR(north.z(), std::cos(deg2rad(89.0)), 1e-14);
    }
}

// 测试南方向向量
TEST(GeoCoordinateTest, GetSouth)
{
    GeoCoordinate coord(0.0, 0.0, 0.0);
    Vector3d north = coord.getNorth();
    Vector3d south = coord.getSouth();
    
    EXPECT_NEAR(south.x(), -north.x(), 1e-14);
    EXPECT_NEAR(south.y(), -north.y(), 1e-14);
    EXPECT_NEAR(south.z(), -north.z(), 1e-14);
}

// 测试极点特殊情况
TEST(GeoCoordinateTest, PoleCases)
{
    // 北极点
    {
        GeoCoordinate coord(deg2rad(90.0), 0.0, 0.0);
        Vector3d zenith = coord.getZenith();
        EXPECT_NEAR(zenith.norm(), 1.0, 1e-14);
        EXPECT_NEAR(zenith.x(), 0.0, 1e-14);
        EXPECT_NEAR(zenith.y(), 0.0, 1e-14);
        EXPECT_NEAR(zenith.z(), 1.0, 1e-14);
        
        // 极点的东方向和北方向是水平方向（基于极限定义）
        Vector3d east = coord.getEast();
        Vector3d north = coord.getNorth();
        EXPECT_NEAR(east.norm(), 1.0, 1e-14);
        EXPECT_NEAR(east.x(), 0.0, 1e-14);
        EXPECT_NEAR(east.y(), 1.0, 1e-14);
        EXPECT_NEAR(east.z(), 0.0, 1e-14);
        
        EXPECT_NEAR(north.norm(), 1.0, 1e-14);
        EXPECT_NEAR(north.x(), -1.0, 1e-14);
        EXPECT_NEAR(north.y(), 0.0, 1e-14);
        EXPECT_NEAR(north.z(), 0.0, 1e-14);
    }
    
    // 南极点
    {
        GeoCoordinate coord(deg2rad(-90.0), 0.0, 0.0);
        Vector3d zenith = coord.getZenith();
        EXPECT_NEAR(zenith.norm(), 1.0, 1e-14);
        EXPECT_NEAR(zenith.x(), 0.0, 1e-14);
        EXPECT_NEAR(zenith.y(), 0.0, 1e-14);
        EXPECT_NEAR(zenith.z(), -1.0, 1e-14);
        
        // 南极点的东方向和北方向（基于极限定义）
        Vector3d east = coord.getEast();
        Vector3d north = coord.getNorth();
        EXPECT_NEAR(east.norm(), 1.0, 1e-14);
        EXPECT_NEAR(east.x(), 0.0, 1e-14);
        EXPECT_NEAR(east.y(), 1.0, 1e-14);
        EXPECT_NEAR(east.z(), 0.0, 1e-14);
        
        EXPECT_NEAR(north.norm(), 1.0, 1e-14);
        EXPECT_NEAR(north.x(), 1.0, 1e-14);
        EXPECT_NEAR(north.y(), 0.0, 1e-14);
        EXPECT_NEAR(north.z(), 0.0, 1e-14);
    }
}

// 测试正交性
TEST(GeoCoordinateTest, Orthogonality)
{
    // 北半球中纬度
    {
        GeoCoordinate coord(deg2rad(45.0), deg2rad(90.0), 0.0);
        Vector3d zenith = coord.getZenith();
        Vector3d east = coord.getEast();
        Vector3d north = coord.getNorth();
        
        EXPECT_NEAR(zenith.norm(), 1.0, 1e-14);
        EXPECT_NEAR(east.norm(), 1.0, 1e-14);
        EXPECT_NEAR(north.norm(), 1.0, 1e-14);
        
        EXPECT_NEAR(zenith.dot(east), 0.0, 1e-14);
        EXPECT_NEAR(zenith.dot(north), 0.0, 1e-14);
        EXPECT_NEAR(east.dot(north), 0.0, 1e-14);
    }
    
    // 南半球中纬度
    {
        GeoCoordinate coord(deg2rad(-45.0), deg2rad(90.0), 0.0);
        Vector3d zenith = coord.getZenith();
        Vector3d east = coord.getEast();
        Vector3d north = coord.getNorth();
        
        EXPECT_NEAR(zenith.norm(), 1.0, 1e-14);
        EXPECT_NEAR(east.norm(), 1.0, 1e-14);
        EXPECT_NEAR(north.norm(), 1.0, 1e-14);
        
        EXPECT_NEAR(zenith.dot(east), 0.0, 1e-14);
        EXPECT_NEAR(zenith.dot(north), 0.0, 1e-14);
        EXPECT_NEAR(east.dot(north), 0.0, 1e-14);
        
        // 右手系验证：east × north = zenith
        Vector3d cross = east.cross(north);
        EXPECT_NEAR(cross.x(), zenith.x(), 1e-14);
        EXPECT_NEAR(cross.y(), zenith.y(), 1e-14);
        EXPECT_NEAR(cross.z(), zenith.z(), 1e-14);
    }
}

// 测试地心纬度到大地纬度转换
TEST(GeoCoordinateTest, SurfaceCentricToDeticLat)
{
    // 赤道上
    double centricLat1 = 0.0;
    double deticLat1 = aSurfaceCentricToDeticLat(centricLat1, kEarthFlatFact);
    EXPECT_NEAR(deticLat1, 0.0, 1e-14);
    
    // 北半球中纬度
    double centricLat2 = deg2rad(45.0);
    double deticLat2 = aSurfaceCentricToDeticLat(centricLat2, kEarthFlatFact);
    EXPECT_GT(deticLat2, centricLat2);  // 大地纬度应大于地心纬度
    
    // 南半球中纬度
    double centricLat3 = deg2rad(-45.0);
    double deticLat3 = aSurfaceCentricToDeticLat(centricLat3, kEarthFlatFact);
    EXPECT_LT(deticLat3, centricLat3);  // 大地纬度应小于地心纬度（绝对值更大）
    
    // 北极点测试
    double centricLat4 = deg2rad(90.0);
    double deticLat4 = aSurfaceCentricToDeticLat(centricLat4, kEarthFlatFact);
    EXPECT_NEAR(deticLat4, centricLat4, 1e-14);
    
    // 南极点测试
    double centricLat5 = deg2rad(-90.0);
    double deticLat5 = aSurfaceCentricToDeticLat(centricLat5, kEarthFlatFact);
    EXPECT_NEAR(deticLat5, centricLat5, 1e-14);
}

// 测试大地纬度到地心纬度转换
TEST(GeoCoordinateTest, SurfaceDeticToCentricLat)
{
    // 赤道上
    double deticLat1 = 0.0;
    double centricLat1 = aSurfaceDeticToCentricLat(deticLat1, kEarthFlatFact);
    EXPECT_NEAR(centricLat1, 0.0, 1e-14);
    
    // 北半球中纬度
    double deticLat2 = deg2rad(45.0);
    double centricLat2 = aSurfaceDeticToCentricLat(deticLat2, kEarthFlatFact);
    EXPECT_LT(centricLat2, deticLat2);  // 地心纬度应小于大地纬度
    
    // 南半球中纬度
    double deticLat3 = deg2rad(-45.0);
    double centricLat3 = aSurfaceDeticToCentricLat(deticLat3, kEarthFlatFact);
    EXPECT_GT(centricLat3, deticLat3);  // 地心纬度应大于大地纬度（绝对值更小）
    
    // 北极点
    double deticLat4 = deg2rad(90.0);
    double centricLat4 = aSurfaceDeticToCentricLat(deticLat4, kEarthFlatFact);
    EXPECT_NEAR(centricLat4, deticLat4, 1e-14);
    
    // 南极点
    double deticLat5 = deg2rad(-90.0);
    double centricLat5 = aSurfaceDeticToCentricLat(deticLat5, kEarthFlatFact);
    EXPECT_NEAR(centricLat5, deticLat5, 1e-14);
}

// 测试转换的可逆性
TEST(GeoCoordinateTest, LatConversionReversibility)
{
    // 多个纬度值测试可逆性
    const double testLats[] = {-75.0, -45.0, -15.0, 0.0, 15.0, 45.0, 75.0};
    for (double lat : testLats) {
        double centric = deg2rad(lat);
        double detic = aSurfaceCentricToDeticLat(centric, kEarthFlatFact);
        double centricBack = aSurfaceDeticToCentricLat(detic, kEarthFlatFact);
        EXPECT_NEAR(centric, centricBack, 1e-14);
    }
    
    // 极点附近测试（包含精确极点）
    const double polarLats[] = {-90.0, -89.0, 89.0, 90.0};
    for (double lat : polarLats) {
        double centric = deg2rad(lat);
        double detic = aSurfaceCentricToDeticLat(centric, kEarthFlatFact);
        double centricBack = aSurfaceDeticToCentricLat(detic, kEarthFlatFact);
        EXPECT_NEAR(centric, centricBack, 1e-14);
    }
}

GTEST_MAIN()
