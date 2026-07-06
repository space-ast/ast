///
/// @file      point_data_report.cpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-07-03
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

#include "ast/RunTime.hpp"
#include "ast/Vector.hpp"
#include "ast/TimeList.hpp"
#include "ast/CelestialBody.hpp"
#include "ast/Interval.hpp"
#include "ast/OrbitElement.hpp"
#include "ast/FrameICRF.hpp"

AST_USING_NAMESPACE;

struct CartesianData
{
    Vector3d pos_;
    Vector3d vel_;
    double gm_;
};

struct InputData
{
    TimeList timeList_;
    Point* point_;
    Frame* frame_;
};


static void extractCartesian(size_t /*n*/, InputData* data, CartesianData* cartesian)
{
    size_t size = data->timeList_.size();
    auto epoch = data->timeList_.epoch();
    auto gm = data->frame_->getGM();
    for(size_t i = 0; i < size; i++)
    {
        auto second = data->timeList_.seconds()[i];
        TimePoint time = epoch + second;
        data->point_->getPosVelIn(data->frame_, time, cartesian[i].pos_, cartesian[i].vel_);
        cartesian[i].gm_ = gm;
    }
}

static void extractX(size_t n, CartesianData* data, double* x)
{
    for (size_t i = 0; i < n; i++)
    {
        x[i] = data[i].pos_[0];
    }
}

static void extractY(size_t n, CartesianData* data, double* y)
{
    for (size_t i = 0; i < n; i++)
    {
        y[i] = data[i].pos_[1];
    }
}

static void extractZ(size_t n, CartesianData* data, double* z)
{
    for (size_t i = 0; i < n; i++)
    {
        z[i] = data[i].pos_[2];
    }
}

static void extractVx(size_t n, CartesianData* data, double* vx)
{
    for (size_t i = 0; i < n; i++)
    {
        vx[i] = data[i].vel_[0];
    }
}

static void extractVy(size_t n, CartesianData* data, double* vy)
{
    for (size_t i = 0; i < n; i++)
    {
        vy[i] = data[i].vel_[1];
    }
}

static void extractVz(size_t n, CartesianData* data, double* vz)
{
    for (size_t i = 0; i < n; i++)
    {
        vz[i] = data[i].vel_[2];
    }
}

// 提取轨道元素

static void extractOrbElem(size_t n, CartesianData* data, OrbElem* orbelems)
{
    for (size_t i = 0; i < n; i++)
    {
        aCartToOrbElem(data[i].pos_, data[i].vel_, data[i].gm_, orbelems[i]);
    }
}

static void extractA(size_t n, OrbElem* orbelems, double* a)
{
    for (size_t i = 0; i < n; i++)
    {
        a[i] = orbelems[i].a_;
    }
}


static void extractE(size_t n, OrbElem* orbelems, double* e)
{
    for (size_t i = 0; i < n; i++)
    {
        e[i] = orbelems[i].e_;
    }
}

static void extractI(size_t n, OrbElem* orbelems, double* inc)
{
    for (size_t i = 0; i < n; i++)
    {
        inc[i] = orbelems[i].i_;
    }
}

static void extractRAAN(size_t n, OrbElem* orbelems, double* raan)
{
    for (size_t i = 0; i < n; i++)
    {
        raan[i] = orbelems[i].raan_;
    }
}

static void extractOmega(size_t n, OrbElem* orbelems, double* omega)
{
    for (size_t i = 0; i < n; i++)
    {
        omega[i] = orbelems[i].argper_;
    }
}

static void extractTrueA(size_t n, OrbElem* orbelems, double* trueA)
{
    for (size_t i = 0; i < n; i++)
    {
        trueA[i] = orbelems[i].trueA_;
    }
}



void exDataPosVel()
{

    TimeList timeList{TimePoint::J2000TT()};
    timeList.seconds() = {0, 60, 120, 180, 240, 300, 360};
    
    auto earth = aGetEarth();
    double gm = earth->getGM();

    auto epoch = timeList.epoch();
    size_t size = timeList.seconds().size();

    std::vector<CartesianData> data;
    data.resize(size);

    // 1. 计算根数据结构体列表
    for (size_t i = 0; i < size; i++)
    {
        auto second = timeList.seconds()[i];
        TimePoint time = epoch + second;
        earth->getPosVelICRF(time, data[i].pos_, data[i].vel_);
        data[i].gm_ = gm;
    }

    // 2. 计算子数据结构体列表
    std::vector<OrbElem> orbElems(size);
    std::transform(data.begin(), data.end(), orbElems.begin(), [=](const CartesianData& d) { 
        OrbElem orbelem;
        aCartToOrbElem(d.pos_, d.vel_, gm, orbelem);
        return orbelem;
    });

    // 3. 计算数据元素列表
    std::vector<double> x(size);
    std::vector<double> y(size);
    std::vector<double> z(size);
    std::vector<double> vx(size);
    std::vector<double> vy(size);
    std::vector<double> vz(size);
    std::vector<double> a(size);
    std::vector<double> e(size);
    std::vector<double> i(size);
    std::vector<double> w(size);
    std::vector<double> trueA(size);

    std::transform(data.begin(), data.end(), x.begin(), [](const CartesianData& d) { return d.pos_[0]; });
    std::transform(data.begin(), data.end(), y.begin(), [](const CartesianData& d) { return d.pos_[1]; });
    std::transform(data.begin(), data.end(), z.begin(), [](const CartesianData& d) { return d.pos_[2]; });
    std::transform(data.begin(), data.end(), vx.begin(), [](const CartesianData& d) { return d.vel_[0]; });
    std::transform(data.begin(), data.end(), vy.begin(), [](const CartesianData& d) { return d.vel_[1]; });
    std::transform(data.begin(), data.end(), vz.begin(), [](const CartesianData& d) { return d.vel_[2]; });

    std::transform(orbElems.begin(), orbElems.end(), a.begin(), [](const OrbElem& o) { return o.a_; });
    std::transform(orbElems.begin(), orbElems.end(), e.begin(), [](const OrbElem& o) { return o.e_; });
    std::transform(orbElems.begin(), orbElems.end(), i.begin(), [](const OrbElem& o) { return o.i_; });
    std::transform(orbElems.begin(), orbElems.end(), w.begin(), [](const OrbElem& o) { return o.argper_; });
    std::transform(orbElems.begin(), orbElems.end(), trueA.begin(), [](const OrbElem& o) { return o.trueA_; });
}


void exDataPosVelByExtractFunction()
{
    // 1. 准备输入数据
    TimeList timeList{TimePoint::J2000TT()};
    timeList.seconds() = {0, 60, 120, 180, 240, 300, 360};

    auto earth = aGetEarth();
    auto frame = aFrameICRF();

    InputData inputData;
    inputData.timeList_ = timeList;
    inputData.point_ = earth;
    inputData.frame_ = frame;

    size_t size = timeList.seconds().size();

    // 2. 计算根数据结构体列表
    std::vector<CartesianData> data(size);
    extractCartesian(size, &inputData, data.data());

    // 3. 计算子数据结构体列表
    std::vector<OrbElem> orbElems(size);
    extractOrbElem(size, data.data(), orbElems.data());

    // 4. 计算数据元素列表
    std::vector<double> x(size);
    std::vector<double> y(size);
    std::vector<double> z(size);
    std::vector<double> vx(size);
    std::vector<double> vy(size);
    std::vector<double> vz(size);
    std::vector<double> a(size);
    std::vector<double> e(size);
    std::vector<double> inc(size);
    std::vector<double> raan(size);
    std::vector<double> omega(size);
    std::vector<double> trueA(size);

    extractX(size, data.data(), x.data());
    extractY(size, data.data(), y.data());
    extractZ(size, data.data(), z.data());
    extractVx(size, data.data(), vx.data());
    extractVy(size, data.data(), vy.data());
    extractVz(size, data.data(), vz.data());

    extractA(size, orbElems.data(), a.data());
    extractE(size, orbElems.data(), e.data());
    extractI(size, orbElems.data(), inc.data());
    extractRAAN(size, orbElems.data(), raan.data());
    extractOmega(size, orbElems.data(), omega.data());
    extractTrueA(size, orbElems.data(), trueA.data());
}

void exDataInterval()
{
    // 1. 根数据结构体列表
    std::vector<Interval> intervals = {{0, 100}, {100, 200}, {200, 300}};

    // 3. 计算数据元素列表
    std::vector<double> start;
    std::vector<double> stop;
    std::vector<double> duration;
    std::transform(intervals.begin(), intervals.end(), start.begin(), [](const Interval& i) { return i.start_; });
    std::transform(intervals.begin(), intervals.end(), stop.begin(), [](const Interval& i) { return i.stop_; });
    std::transform(intervals.begin(), intervals.end(), duration.begin(), [](const Interval& i) { return i.duration(); });
}

int main()
{
    
    exDataPosVel();
    exDataInterval();
}