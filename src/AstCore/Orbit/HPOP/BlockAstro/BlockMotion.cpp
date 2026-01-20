///
/// @file      BlockMotion.cpp
/// @brief     ~
/// @details   ~
/// @author    axel
/// @date      2026-01-20
/// @copyright 版权所有 (C) 2026-present, ast项目.
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

#include "BlockMotion.hpp"
#include "AstCore/BlockAstro.hpp"
#include "AstUtil/IdentifierAPI.hpp"

AST_NAMESPACE_BEGIN

BlockMotion::BlockMotion()
    : BlockDerivative{}
    , velocity_{&vectorBuffer}
    , positionDerivative_{&vectorBuffer}
    , vectorBuffer{}
{
    static auto identifierVel = aIdentifier(kIdentifierVel);
    static auto identifierPos= aIdentifier(kIdentifierPos);

    inputPorts_ = {
        // 速度
        {
            identifierVel,
            (signal_t*)&velocity_,
            3,
            DataPort::eDouble
        }
    };

    derivativePorts_ = {
        // 位置导数
        {
            identifierPos,
            (signal_t*)&positionDerivative_,
            3,
            DataPort::eDouble
        }
    };

}


err_t BlockMotion::evaluate(const SimTime& simTime)
{
    // @fixme: 需要使用 += 适应未来可能出现的、有多个函数块共同贡献同一个状态量导数的情况吗？
    // *positionDerivative_ += *velocity_;
    
    *positionDerivative_ = *velocity_;
    return 0;
}

AST_NAMESPACE_END
