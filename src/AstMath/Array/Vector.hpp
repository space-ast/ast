///
/// @file      Vector.hpp
/// @brief     
/// @details   ~
/// @author    jinke18
/// @date      15.11.2025
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
#include "AstMath/MathOperator.hpp"
#include <stdlib.h>    // for malloc
 
AST_NAMESPACE_BEGIN

template<typename _Scalar, size_t N>
class VectorN;


#define _AST_DEF_VECTOR_METHOD(Scalar)                                                  \
    A_DEF_POD_ITERABLE(Scalar)                                                          \
    Scalar operator()(size_t idx) const{return data()[idx];}                            \
    Scalar& operator()(size_t idx) {return data()[idx];}                                \
    Scalar& x() {return data()[0];}                                                     \
    Scalar& y() {return data()[1];}                                                     \
    Scalar& z() {return data()[2];}                                                     \
    Scalar x() const {return data()[0];}                                                \
    Scalar y() const {return data()[1];}                                                \
    Scalar z() const {return data()[2];}                                                \



template<typename _Scalar, size_t N>
class VectorN
{
public:
    typedef _Scalar Scalar;
    enum {Dimension = N};
    _AST_DEF_VECTOR_METHOD(Scalar);
public:
    Scalar m_data[N]; // 不要直接访问m_data，设为public仅为了实现聚合初始化
};


template<typename _Scalar>
class VectorN<_Scalar, 3>
{
public:
    typedef VectorN<_Scalar, 3> Self;
    typedef _Scalar Scalar;
    enum {Dimension = 3};
    
    static Self Zero(){return Self{0,0,0}; }
    static Self UnitX() {return Self{1,0,0}; }
    static Self UnitY() {return Self{0,1,0}; }
    static Self UnitZ() {return Self{0,0,1}; }
    _Scalar at(size_t idx) const{return data()[idx]; }
    _Scalar& at(size_t idx) {return data()[idx]; }
    void normalize(){_ASTMATH normalize(*this);}
    double norm() const{return _ASTMATH norm(*this);}
    Self operator-() const{return Self{-x_, -y_, -z_};}
    _AST_DEF_VECTOR_METHOD(Scalar);
public:
    double x_, y_, z_; // 不要直接访问数据，设为public仅为了实现聚合初始化
};


template<typename _Scalar>
class VectorX
{
public:
    VectorX();
    VectorX(size_t size);
    ~VectorX();

    void resize(size_t size);
    A_DEF_ITERABLE(_Scalar, data_, size_)
public:
    _Scalar* data_;
    size_t   size_;
};


/// inlines


template<typename _Scalar>
inline VectorX<_Scalar>::VectorX()
    : size_{0}
    , data_{nullptr}
{
}

template<typename _Scalar>
inline VectorX<_Scalar>::VectorX(size_t size)
    : size_{ size }
    , data_((_Scalar*)malloc(sizeof(_Scalar)* size))
{

}


template<typename _Scalar>
inline VectorX<_Scalar>::~VectorX()
{
    if(data_)
        free(data_);
}

template<typename _Scalar>
inline void VectorX<_Scalar>::resize(size_t size)
{
    if (size > size_)
    {
        if(data_)
            free(data_);
        data_ = (_Scalar*)malloc(sizeof(_Scalar) * size);
    }
    size_ = size;
}



typedef VectorX<double> VectorXd;


AST_NAMESPACE_END


AST_DECL_TYPE_ALIAS(Vector3d)
AST_DECL_TYPE_ALIAS(VectorXd)

