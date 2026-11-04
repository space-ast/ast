///
/// @file      testResultLoader.cpp
/// @brief     ResultLoader 测试
/// @details   覆盖结果计算量的类型分派、嵌套 CalcObject、向量/轨道段/主航天器
///            等延迟链接的解析分支
/// @author    axel
/// @date      2026-11-04
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

#include "ast/Test.hpp"
#include "ast/ResultLoader.hpp"
#include "ast/ValDict.hpp"
#include "ast/Value.hpp"
#include "ast/ObjectLinker.hpp"
#include "ast/RTTIAPI.hpp"
#include "ast/SpaceObject.hpp"
#include "ast/Sequence.hpp"
#include "ast/VectorFixed.hpp"
#include "ast/AxesICRF.hpp"
#include "ast/ScStateCalcAngle.hpp"
#include "ast/ScStateCalcDifference.hpp"
#include "ast/ScStateCalcDifferenceOtherSegment.hpp"
#include "ast/ScStateCalcDeltaFromMaster.hpp"

AST_USING_NAMESPACE

namespace {

/// @brief 创建只带 Type 字段的结果字典
SharedPtr<ValDict> makeResultDict(const char* type)
{
    SharedPtr<ValDict> dict = ValDict::New();
    dict->insert("Type", type);
    return dict;
}

/// @brief 创建嵌套的 CalcObject 字典（走 aValueToResult 的非字符串分支）
Value* makeCalcObjectDict(const char* type)
{
    Value* dict = aNewValueDict();
    dict->insert("Type", type);
    return dict;
}

} // namespace

/// @brief 尚未被任务数据覆盖的内置计算量类型都应当能创建
TEST(ResultLoaderTest, LoadsRemainingBuiltinTypes)
{
    const char* types[] = {
        "AsStateCalcSemiMajorAxis",
        "AsStateCalcEccAnomaly",
        "AsStateCalcAltitudeOfApoapsis",
        "AsStateCalcTimePastAscNode",
    };
    for (const char* type : types)
    {
        auto dict = makeResultDict(type);
        SharedPtr<ObjectCalculation> result;
        EXPECT_EQ(aLoadResult(*dict, result, nullptr), eNoError) << type;
        EXPECT_NE(result.get(), nullptr) << type;
    }
}

/// @brief 非密切根数类型应当给出告警但仍按密切根数创建
TEST(ResultLoaderTest, WarnsOnUnsupportedElementType)
{
    auto dict = makeResultDict("AsStateCalcEccentricity");
    dict->insert("ElementType", "Mean");
    SharedPtr<ObjectCalculation> result;
    EXPECT_EQ(aLoadResult(*dict, result, nullptr), eNoError);
    EXPECT_NE(result.get(), nullptr);
}

/// @brief CalcObject 以字典形式给出时应递归加载参考计算量
TEST(ResultLoaderTest, DifferenceAcceptsNestedCalculation)
{
    auto dict = makeResultDict("CAgAsStateCalcDifference");
    dict->insert("DifferenceOrderToUse", "Initial Minus Current");
    dict->insert("CalcObject", makeCalcObjectDict("AsStateCalcRMag"));

    SharedPtr<ObjectCalculation> result;
    EXPECT_EQ(aLoadResult(*dict, result, nullptr), eNoError);
    auto calculation = aobject_cast<ScStateCalcDifference*>(result.get());
    ASSERT_NE(calculation, nullptr);
    EXPECT_NE(calculation->calculation(), nullptr);
}

/// @brief 向量按名称解析到已注册的对象
TEST(ResultLoaderTest, AngleResolvesVectorNames)
{
    // 注册到对象管理器后由 SharedPtr 持有，避免栈对象的弱引用在退出时失效
    SharedPtr<VectorFixed> vector1 = new VectorFixed();
    SharedPtr<VectorFixed> vector2 = new VectorFixed();
    vector1->setName("ResultLoaderVector1");
    vector2->setName("ResultLoaderVector2");
    vector1->setAxes(aAxesICRF());
    vector2->setAxes(aAxesICRF());
    vector1->setValue({1.0, 0.0, 0.0});
    vector2->setValue({0.0, 1.0, 0.0});
    aAddObject(vector1.get());
    aAddObject(vector2.get());

    auto dict = makeResultDict("CAgAsStateCalcAngle");
    dict->insert("Vector1", "ResultLoaderVector1");
    dict->insert("Vector2", "ResultLoaderVector2");

    SharedPtr<ObjectCalculation> result;
    EXPECT_EQ(aLoadResult(*dict, result, nullptr), eNoError);
    auto calculation = aobject_cast<ScStateCalcAngle*>(result.get());
    ASSERT_NE(calculation, nullptr);

    result->resolveLinks();
    EXPECT_EQ(calculation->getVector1(), vector1.get());
    EXPECT_EQ(calculation->getVector2(), vector2.get());
}

/// @brief 轨道段状态与差值顺序的合法取值应被读入
TEST(ResultLoaderTest, DifferenceOtherSegmentReadsOptions)
{
    auto dict = makeResultDict("CAgAsStateCalcDifferenceOtherSegment");
    dict->insert("SegmentStateToUse", "Initial");
    dict->insert("DifferenceOrderToUse", "Other Segment Minus Current");
    dict->insert("OtherSegment", "Segment1");
    dict->insert("CalcObject", makeCalcObjectDict("AsStateCalcSemiMajorAxis"));

    SharedPtr<ObjectCalculation> result;
    EXPECT_EQ(aLoadResult(*dict, result, nullptr), eNoError);
    auto calculation = aobject_cast<ScStateCalcDifferenceOtherSegment*>(result.get());
    ASSERT_NE(calculation, nullptr);
    EXPECT_EQ(calculation->segmentStateToUse(), ESegmentStateToUse::eInitial);
    EXPECT_EQ(calculation->differenceOrderToUse(), ESegmentDifferenceOrder::eSegmentMinusCurrent);

    // 计算量不在 Sequence 下：解析时应当报错并保持 OtherSegment 为空
    result->resolveLinks();
    EXPECT_EQ(calculation->otherSegment(), nullptr);
}

/// @brief 非法的轨道段状态与差值顺序回退到默认值
TEST(ResultLoaderTest, DifferenceOtherSegmentFallsBackToDefaults)
{
    auto dict = makeResultDict("CAgAsStateCalcDifferenceOtherSegment");
    dict->insert("SegmentStateToUse", "Bogus");
    dict->insert("DifferenceOrderToUse", "Bogus");
    dict->insert("OtherSegment", "Segment1");
    dict->insert("CalcObject", makeCalcObjectDict("AsStateCalcSemiMajorAxis"));

    SharedPtr<ObjectCalculation> result;
    EXPECT_EQ(aLoadResult(*dict, result, nullptr), eNoError);
    auto calculation = aobject_cast<ScStateCalcDifferenceOtherSegment*>(result.get());
    ASSERT_NE(calculation, nullptr);
    EXPECT_EQ(calculation->segmentStateToUse(), ESegmentStateToUse::eFinal);
    EXPECT_EQ(calculation->differenceOrderToUse(), ESegmentDifferenceOrder::eCurrentMinusSegment);

    result->resolveLinks();
}

/// @brief 计算量位于 Sequence 下但路径不存在时应保持 OtherSegment 为空
TEST(ResultLoaderTest, DifferenceOtherSegmentReportsUnknownSegment)
{
    SharedPtr<Sequence> sequence = new Sequence();
    sequence->setName("ResultLoaderSequence");

    auto dict = makeResultDict("CAgAsStateCalcDifferenceOtherSegment");
    dict->insert("OtherSegment", "NoSuchSegment");
    dict->insert("CalcObject", makeCalcObjectDict("AsStateCalcSemiMajorAxis"));

    SharedPtr<ObjectCalculation> result;
    EXPECT_EQ(aLoadResult(*dict, result, nullptr), eNoError);
    auto calculation = aobject_cast<ScStateCalcDifferenceOtherSegment*>(result.get());
    ASSERT_NE(calculation, nullptr);

    result->setParentScope(sequence.get());
    result->resolveLinks();
    EXPECT_EQ(calculation->otherSegment(), nullptr);
}

/// @brief 无航天器父对象且未给出参考路径时，主航天器保持为空
TEST(ResultLoaderTest, DeltaFromMasterWithoutSpaceObject)
{
    auto dict = makeResultDict("CAgAsStateCalcDeltaFromMaster");
    dict->insert("ReferenceSelection", "Select Object");
    dict->insert("CalcObject", makeCalcObjectDict("AsStateCalcSemiMajorAxis"));

    SharedPtr<ObjectCalculation> result;
    EXPECT_EQ(aLoadResult(*dict, result, nullptr), eNoError);
    auto calculation = aobject_cast<ScStateCalcDeltaFromMaster*>(result.get());
    ASSERT_NE(calculation, nullptr);

    result->resolveLinks();
    EXPECT_EQ(calculation->reference(), nullptr);
}

/// @brief 航天器未设置参考航天器时，主航天器保持为空
TEST(ResultLoaderTest, DeltaFromMasterWithoutReferenceVehicle)
{
    SharedPtr<SpaceObject> vehicle = new SpaceObject();
    vehicle->setName("ResultLoaderVehicle");

    auto dict = makeResultDict("CAgAsStateCalcDeltaFromMaster");
    dict->insert("ReferenceSelection", "Use Basic->Reference");
    dict->insert("CalcObject", makeCalcObjectDict("AsStateCalcSemiMajorAxis"));

    SharedPtr<ObjectCalculation> result;
    EXPECT_EQ(aLoadResult(*dict, result, nullptr), eNoError);
    auto calculation = aobject_cast<ScStateCalcDeltaFromMaster*>(result.get());
    ASSERT_NE(calculation, nullptr);

    result->setParentScope(vehicle.get());
    result->resolveLinks();
    EXPECT_EQ(calculation->reference(), nullptr);
}

/// @brief 主航天器取航天器基本属性中的参考航天器
TEST(ResultLoaderTest, DeltaFromMasterUsesReferenceVehicle)
{
    SharedPtr<SpaceObject> vehicle = new SpaceObject();
    SharedPtr<SpaceObject> referenceVehicle = new SpaceObject();
    vehicle->setName("ResultLoaderVehicle");
    referenceVehicle->setName("ResultLoaderReferenceVehicle");
    aSpaceObject_SetReferenceVehicle(vehicle.get(), referenceVehicle.get());

    auto dict = makeResultDict("CAgAsStateCalcDeltaFromMaster");
    dict->insert("ReferenceSelection", "Use Basic->Reference");
    dict->insert("CalcObject", makeCalcObjectDict("AsStateCalcSemiMajorAxis"));

    SharedPtr<ObjectCalculation> result;
    EXPECT_EQ(aLoadResult(*dict, result, nullptr), eNoError);
    auto calculation = aobject_cast<ScStateCalcDeltaFromMaster*>(result.get());
    ASSERT_NE(calculation, nullptr);

    result->setParentScope(vehicle.get());
    result->resolveLinks();
    EXPECT_EQ(calculation->reference(), static_cast<Point*>(referenceVehicle.get()));
}

/// @brief 显式给出的参考对象解析失败时，主航天器保持为空
TEST(ResultLoaderTest, DeltaFromMasterReportsMissingReference)
{
    auto dict = makeResultDict("CAgAsStateCalcDeltaFromMaster");
    dict->insert("Reference", "NoSuchPoint");
    dict->insert("CalcObject", makeCalcObjectDict("AsStateCalcSemiMajorAxis"));

    SharedPtr<ObjectCalculation> result;
    EXPECT_EQ(aLoadResult(*dict, result, nullptr), eNoError);
    auto calculation = aobject_cast<ScStateCalcDeltaFromMaster*>(result.get());
    ASSERT_NE(calculation, nullptr);

    result->resolveLinks();
    EXPECT_EQ(calculation->reference(), nullptr);
}

GTEST_MAIN()
