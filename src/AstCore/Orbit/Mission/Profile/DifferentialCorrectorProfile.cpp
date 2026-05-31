///
/// @file      DifferentialCorrectorProfile.cpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-04-28
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

#include "DifferentialCorrectorProfile.hpp"
#include "AstCore/Sequence.hpp"
#include "AstUtil/Logger.hpp"
#include "AstUtil/RTTIAPI.hpp"
#include "AstMath/Matrix.hpp"
#include "AstMath/Vector.hpp"
#include <algorithm>

AST_NAMESPACE_BEGIN

DifferentialCorrectorProfile* DifferentialCorrectorProfile::New()
{
    return new DifferentialCorrectorProfile();
}

namespace {

/// @brief 高斯列主元消去法求解线性方程组 Ax = b
/// @param A 方阵（n×n），会被修改为上三角矩阵
/// @param b 右端向量（n维），求解后存放解向量 x
/// @warning 该函数会修改输入参数 A 和 b
/// @return true  求解成功
/// @return false 矩阵奇异，无法求解
bool solveLinearSystem(MatrixXd& A, VectorXd& b)
{
    size_t n = A.row();                   // 获取矩阵行数（方程个数）

    // ---------- 前向消元 ----------
    for (size_t i = 0; i < n; ++i)
    {
        // 1. 列主元选取：在第 i 列中寻找绝对值最大的元素所在行
        size_t maxRow = i;
        for (size_t k = i + 1; k < n; ++k)
        {
            if (fabs(A(k, i)) > fabs(A(maxRow, i)))
                maxRow = k;
        }
        
        // 2. 若主元不在当前行，则交换两行（A 和 b 同时交换）
        if (maxRow != i)
        {
            for (size_t j = 0; j < n; ++j)
            {
                std::swap(A(i, j), A(maxRow, j));
            }
            double temp = b[i];
            b[i] = b[maxRow];
            b[maxRow] = temp;
        }
        
        // 3. 检查主元是否接近零（矩阵奇异，无法继续）
        if (fabs(A(i, i)) < 1e-15)
            return false;
        
        // 4. 消去当前列下方所有行
        for (size_t k = i + 1; k < n; ++k)
        {
            double factor = A(k, i) / A(i, i);      // 消元因子
            for (size_t j = i; j < n; ++j)
                A(k, j) -= factor * A(i, j);        // 更新矩阵行
            b[k] -= factor * b[i];                  // 更新右端项
        }
    }
    
    // ---------- 回代求解 ----------
    // 从最后一行开始向上计算
    for (size_t i = n; i-- > 0; )   // i 从 n-1 递减到 0
    {
        double sum = 0.0;
        for (size_t j = i + 1; j < n; ++j)
            sum += A(i, j) * b[j];               // 已知变量代入累加
        b[i] = (b[i] - sum) / A(i, i);           // 解出当前变量
    }
    
    return true;
}

} // anonymous namespace

errc_t DifferentialCorrectorProfile::execute()
{
    std::vector<ShooterControl*> activeControls;
    std::vector<ShooterResult*> activeResults;
    
    for (auto& ctrl : controls_)
    {
        if (ctrl->active())
            activeControls.push_back(ctrl.get());
    }
    
    for (auto& result : results_)
    {
        if (result->active())
            activeResults.push_back(result.get());
    }
    
    size_t n = activeControls.size();
    size_t m = activeResults.size();
    
    if (n == 0 || m == 0)
    {
        aError("no active controls or results");
        return eErrorInvalidParam;
    }
    
    if (m != n)
    {
        aError("number of controls (%zu) must equal number of results (%zu)", n, m);
        return eErrorInvalidParam;
    }
    
    // 获取一次基准状态的结果值
    std::vector<double> baseValues(m);
    if (errc_t err = executeRelatedCommand())
    {
        aError("failed to execute related command for baseline");
        return err;
    }
    for (size_t i = 0; i < m; ++i)
    {
        if (errc_t err = activeResults[i]->getValue(baseValues[i]))
        {
            aError("failed to get baseline result value for '%.*s'",
                   activeResults[i]->name().size(), activeResults[i]->name().data());
            return err;
        }
    }
    
    for (int iter = 0; iter < maxIterations_; ++iter)
    {
        // 更新基准值（每次迭代开始时的系统状态）
        if (iter > 0)
        {
            if (errc_t err = executeRelatedCommand())
            {
                aError("failed to execute related command at start of iteration %d", iter);
                return err;
            }
            for (size_t i = 0; i < m; ++i)
            {
                if (errc_t err = activeResults[i]->getValue(baseValues[i]))
                {
                    aError("failed to get result value for '%.*s' at iteration start",
                           activeResults[i]->name().size(), activeResults[i]->name().data());
                    return err;
                }
            }
        }
        
        // 计算残差
        VectorXd residuals(m);
        bool converged = true;
        
        for (size_t i = 0; i < m; ++i)
        {
            double value = 0;
            if (errc_t err = activeResults[i]->getValue(value))
            {
                aError("failed to get result value for '%.*s'",
                       activeResults[i]->name().size(), activeResults[i]->name().data());
                return err;
            }
            
            double scale = activeResults[i]->scale();
            if (scale <= 0)
            {
                aWarning("result '%.*s' scale is <= 0, using 1.0",
                         activeResults[i]->name().size(), activeResults[i]->name().data());
                scale = 1.0;
            }
            
            residuals[i] = (value - activeResults[i]->desired()) / scale;
            
            if (fabs(residuals[i]) > activeResults[i]->tolerance())
                converged = false;
        }
        
        if (converged)
        {
            aInfo("differential corrector converged in %d iterations", iter + 1);
            return eNoError;
        }
        
        // 避免 maximumRunsToLog_ 为 0 时取模崩溃
        int logInterval = maximumRunsToLog_;
        if (logInterval > 0 && (iter % logInterval == 0))
        {
            double maxResidual = 0;
            for (size_t i = 0; i < m; ++i)
                maxResidual = std::max(maxResidual, fabs(residuals[i]));
            aInfo("iteration %d: max residual = %.6e", iter + 1, maxResidual);
        }
        else if (logInterval <= 0)
        {
            // 若 logInterval 为 0 或负数，不输出中间日志
        }
        
        // 构建雅可比矩阵
        MatrixXd jacobian(m, n);
        jacobian.setZero();
        
        for (size_t j = 0; j < n; ++j)
        {
            double origValue = 0;
            if (errc_t err = activeControls[j]->getValue(origValue))
            {
                aError("failed to get control value for '%.*s'",
                       activeControls[j]->name().size(), activeControls[j]->name().data());
                return err;
            }
            
            double perturbation = activeControls[j]->perturbation();
            if(perturbation <=0 )
            {
                perturbation = fabs(origValue) * 0.01;
                aWarning("control '%.*s' perturbation is <= 0, using %.6e",
                         activeControls[j]->name().size(), activeControls[j]->name().data(), perturbation);
            }
            double scale = activeControls[j]->scale();
            if (scale <= 0)
            {
                aWarning("control '%.*s' scale is <= 0, using 1.0",
                         activeControls[j]->name().size(), activeControls[j]->name().data());
                scale = 1.0;
            }
            
            double delta = perturbation * scale;
            
            // 一个辅助 lambda，用于读取当前所有结果值
            auto getCurrentResults = [&](std::vector<double>& out) -> errc_t {
                out.resize(m);
                for (size_t i = 0; i < m; ++i)
                {
                    if (errc_t e = activeResults[i]->getValue(out[i]))
                        return e;
                }
                return eNoError;
            };
            
            // 恢复控制变量原值并执行命令，确保回到基准状态
            auto restoreControl = [&]() -> errc_t {
                return activeControls[j]->setValue(origValue);
                // if (errc_t e = activeControls[j]->setValue(origValue))
                //     return e;
                // return executeRelatedCommand();
            };
            
            switch (finiteDifferenceMethod_)
            {
            case EFiniteDifferenceMethod::eForwardDifference:
            {
                // 正向扰动
                if (errc_t e = activeControls[j]->setValue(origValue + delta))
                    return e;
                if (errc_t e = executeRelatedCommand())
                {
                    restoreControl();  // 尝试恢复，忽略可能的二次错误
                    aError("failed to execute related command during forward perturbation");
                    return e;
                }
                
                std::vector<double> perturbedValues(m);
                if (errc_t e = getCurrentResults(perturbedValues))
                {
                    restoreControl();
                    aError("failed to get result values during forward perturbation");
                    return e;
                }
                
                // 恢复原值并执行命令
                if (errc_t e = restoreControl())
                {
                    aError("failed to restore control value for '%.*s'",
                           activeControls[j]->name().size(), activeControls[j]->name().data());
                    return e;
                }
                
                for (size_t i = 0; i < m; ++i)
                {
                    double resultScale = activeResults[i]->scale();
                    if (resultScale <= 0) resultScale = 1.0;
                    jacobian(i, j) = (perturbedValues[i] - baseValues[i]) / (delta * resultScale / scale);
                }
                break;
            }
            case EFiniteDifferenceMethod::eCentralDifference:
            {
                // 正向扰动
                if (errc_t e = activeControls[j]->setValue(origValue + delta))
                    return e;
                if (errc_t e = executeRelatedCommand())
                {
                    restoreControl();
                    aError("failed to execute related command during forward perturbation");
                    return e;
                }
                std::vector<double> forwardValues(m);
                if (errc_t e = getCurrentResults(forwardValues))
                {
                    restoreControl();
                    aError("failed to get result values during forward perturbation");
                    return e;
                }
                
                // 负向扰动
                if (errc_t e = activeControls[j]->setValue(origValue - delta))
                {
                    restoreControl();
                    aError("failed to set control value for backward perturbation");
                    return e;
                }
                if (errc_t e = executeRelatedCommand())
                {
                    restoreControl();
                    aError("failed to execute related command during backward perturbation");
                    return e;
                }
                std::vector<double> backwardValues(m);
                if (errc_t e = getCurrentResults(backwardValues))
                {
                    restoreControl();
                    aError("failed to get result values during backward perturbation");
                    return e;
                }
                
                // 恢复原值并执行命令
                if (errc_t e = restoreControl())
                {
                    aError("failed to restore control value for '%.*s'",
                           activeControls[j]->name().size(), activeControls[j]->name().data());
                    return e;
                }
                
                for (size_t i = 0; i < m; ++i)
                {
                    double resultScale = activeResults[i]->scale();
                    if (resultScale <= 0) resultScale = 1.0;
                    jacobian(i, j) = (forwardValues[i] - backwardValues[i]) / (2.0 * delta * resultScale / scale);
                }
                break;
            }
            default:
                aError("unknown finite difference method: %d", static_cast<int>(finiteDifferenceMethod_));
                return eErrorInvalidParam;
            }
        }
        
        // 负残差作为右端项
        for (size_t i = 0; i < m; ++i)
            residuals[i] = -residuals[i];
        
        if (!solveLinearSystem(jacobian, residuals))
        {
            aError("failed to solve linear system - singular matrix");
            return eErrorDivideByZero;
        }
        
        // 应用修正量
        for (size_t j = 0; j < n; ++j)
        {
            double origValue = 0;
            if (errc_t err = activeControls[j]->getValue(origValue))
            {
                aError("failed to get control value for '%.*s'",
                       activeControls[j]->name().size(), activeControls[j]->name().data());
                return err;
            }
            
            double scale = activeControls[j]->scale();
            if (scale <= 0)
            {
                aWarning("control '%.*s' scale is <= 0 during correction, using 1.0",
                         activeControls[j]->name().size(), activeControls[j]->name().data());
                scale = 1.0;
            }
            
            double correction = residuals[j] * scale;
            
            double maxStep = activeControls[j]->maxStep();
            if (fabs(correction) > maxStep)
                correction = correction > 0 ? maxStep : -maxStep;
            
            activeControls[j]->setCorrection(correction);
            activeControls[j]->setTotalCorrection(activeControls[j]->totalCorrection() + correction);
            
            if (errc_t err = activeControls[j]->setValue(origValue + correction))
            {
                aError("failed to set control value for '%.*s'",
                       activeControls[j]->name().size(), activeControls[j]->name().data());
                return err;
            }
        }
    }
    
    aError("differential corrector failed to converge after %d iterations", maxIterations_);
    return eErrorMaxIter;
}

errc_t DifferentialCorrectorProfile::executeRelatedCommand() const
{
    Command* command = getRelatedCommand();
    if(!command)
    {
        aError("failed to find related command");
        return eErrorNullPtr;
    }

    if(Sequence* sequence = aobject_cast<Sequence*>(command))
        return sequence->Sequence::execute();
    
    return command->execute();
}

Sequence* DifferentialCorrectorProfile::getRelatedSequence() const
{
    return aobject_cast<Sequence*>(getRelatedCommand());
}

Command* DifferentialCorrectorProfile::getRelatedCommand() const
{
    if(auto command = relatedCommand_.get())
        return command;
    auto command = aGetAncestorScope<Command*>(const_cast<DifferentialCorrectorProfile*>(this));
    relatedCommand_ = command;
    return command;
}



ShooterResult* DifferentialCorrectorProfile::getResult(StringView name) const
{
    for(auto& result : results_)
        if(name == result->name())
            return result.get();
    return nullptr;
}

ShooterControl* DifferentialCorrectorProfile::getControl(StringView name) const
{
    for(auto& control : controls_)
        if(name == control->name())
            return control.get();
    return nullptr;
}



AST_NAMESPACE_END