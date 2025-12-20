///
/// @file      TraitPosVel.hpp
/// @brief     
/// @details   ~
/// @author    jinke18
/// @date      23.11.2025
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
/// 软件按“现有状态”提供，无任何明示或暗示的担保条件。
/// 除非法律要求或书面同意，作者与贡献者不承担任何责任。
/// 使用本软件所产生的风险，需由您自行承担。
 
#pragma once
 
#include "AstGlobal.h"
#include "TraitObject.hpp"
 
AST_NAMESPACE_BEGIN

class Type;
 

/// @brief 提供位置速度
class IPosVelPrv
{
public:
    virtual ~IPosVelPrv()
    {};
    virtual Type* type() const = 0;
    virtual err_t getPosIn(System* system, const AbsTime& time, Vector3d& pos) const = 0;
    virtual err_t getPosVelIn(System* system, const AbsTime& time, Vector3d& pos, Vector3d& vel) const = 0;
};



template<typename T>
class ImplFor<IPosVelPrv, T>: public TraitObject<IPosVelPrv>
{
public:
    typedef TraitObject<IPosVelPrv> Base;
    ImplFor(T* obj)
        :Base{ obj }
    {}
    ImplFor(const T& obj)
        :Base{(void*) & obj}
    {}
    Type* type() const override
    {
        return T::staticType(); 
    }
    err_t getValue(const AbsTime& time, double& value) const override
    {
        return get()->getValue(time, value);
    }
    
    T* get() const{return (T*) data_; }
};



 
AST_NAMESPACE_END
 
