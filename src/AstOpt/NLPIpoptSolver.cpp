/// @file      NLPIpoptSolver.cpp
/// @brief     
/// @details   ~
/// @author    jinke18
/// @date      16.11.2025
/// @copyright 版权所有 (C) 2025-present, ast项目.

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
 
 
 
 
 
 
 

#include "NLPIpoptSolver.hpp"

#if defined AST_WITH_IPOPT 
#include "IpTNLP.hpp"
#include "IpIpoptApplication.hpp"
#include "IpSolveStatistics.hpp"

AST_NAMESPACE_BEGIN


class CIpoptNLP : public Ipopt::TNLP
{
public:
    using Index = typename Ipopt::Index;
    using Number = typename Ipopt::Number;
    using SolverReturn = typename Ipopt::SolverReturn;
    using IpoptData = typename Ipopt::IpoptData;
    using IpoptCalculatedQuantities = typename Ipopt::IpoptCalculatedQuantities;
    friend class CNLPIpoptSolver;

    CIpoptNLP(CNLPIpoptSolver* solver, INLPProblem* problem)
        :m_solver{solver}
        ,m_problem{problem}
    {}
    virtual bool get_nlp_info(
        Index& n,
        Index& m,
        Index& nnz_jac_g,
        Index& nnz_h_lag,
        IndexStyleEnum& index_style
    ) override;

    virtual bool get_bounds_info(
        Index   n,
        Number* x_l,
        Number* x_u,
        Index   m,
        Number* g_l,
        Number* g_u
    );

    /** Method to return the starting point for the algorithm */
    virtual bool get_starting_point(
        Index   n,
        bool    init_x,
        Number* x,
        bool    init_z,
        Number* z_L,
        Number* z_U,
        Index   m,
        bool    init_lambda,
        Number* lambda
    ) override;

    /** Method to return the objective value */
    virtual bool eval_f(
        Index         n,
        const Number* x,
        bool          new_x,
        Number& obj_value
    ) override;

    /** Method to return the gradient of the objective */
    virtual bool eval_grad_f(
        Index         n,
        const Number* x,
        bool          new_x,
        Number* grad_f
    ) override;

    /** Method to return the constraint residuals */
    virtual bool eval_g(
        Index         n,
        const Number* x,
        bool          new_x,
        Index         m,
        Number* g
    ) override;

    /** Method to return:
     *   1) The structure of the Jacobian (if "values" is NULL)
     *   2) The values of the Jacobian (if "values" is not NULL)
     */
    virtual bool eval_jac_g(
        Index         n,
        const Number* x,
        bool          new_x,
        Index         m,
        Index         nele_jac,
        Index* iRow,
        Index* jCol,
        Number* values
    ) override;

    /** Method to return:
     *   1) The structure of the Hessian of the Lagrangian (if "values" is NULL)
     *   2) The values of the Hessian of the Lagrangian (if "values" is not NULL)
     */
    virtual bool eval_h(
        Index         n,
        const Number* x,
        bool          new_x,
        Number        obj_factor,
        Index         m,
        const Number* lambda,
        bool          new_lambda,
        Index         nele_hess,
        Index* iRow,
        Index* jCol,
        Number* values
    ) override;

    /** This method is called when the algorithm is complete so the TNLP can store/write the solution */
    virtual void finalize_solution(
        SolverReturn               status,
        Index                      n,
        const Number* x,
        const Number* z_L,
        const Number* z_U,
        Index                      m,
        const Number* g,
        const Number* lambda,
        Number                     obj_value,
        const IpoptData* ip_data,
        IpoptCalculatedQuantities* ip_cq
    ) override;
protected:
    CNLPIpoptSolver* m_solver;
    INLPProblem* m_problem;
    SolverReturn m_status{SolverReturn::UNASSIGNED };
    VectorXd m_xopt;
    VectorXd m_gopt;
    double   m_obj;
};




bool CIpoptNLP::get_nlp_info(Index& n, Index& m, Index& nnz_jac_g, Index& nnz_h_lag, IndexStyleEnum& index_style)
{
    NLPInfo info;
    m_problem->GetInfo(info);
    n = info.numVariable;
    m = info.numConstraint;
    NLPJacInfo jacinfo;
    m_problem->GetJacInfo(jacinfo);
    nnz_jac_g = jacinfo.idxVariable.size();
    index_style = C_STYLE;
    return true;
}

bool CIpoptNLP::get_bounds_info(Index n, Number* x_l, Number* x_u, Index m, Number* g_l, Number* g_u)
{
    NLPBounds bounds;
    NLPInfo info;
    m_problem->GetInfo(info);
    bounds.initialize(info.numVariable, info.numConstraint);
    m_problem->GetBounds(bounds);
    for (int i = 0; i < n; i++)
    {
        x_l[i] = bounds.lower.variable[i];
        x_u[i] = bounds.upper.variable[i];
    }
    for (int i = 0; i < m; i++)
    {
        g_l[i] = bounds.lower.constraint[i];
        g_u[i] = bounds.upper.constraint[i];
    }
    return true;
}

bool CIpoptNLP::get_starting_point(Index n, bool init_x, Number* x, bool init_z, Number* z_L, Number* z_U, Index m, bool init_lambda, Number* lambda)
{
    m_problem->GetInitialGuess(n, x);
    return true;
}

bool CIpoptNLP::eval_f(Index n, const Number* x, bool new_x, Number& obj_value)
{
    m_problem->EvalObjective(n, x, obj_value);
    return true;
}

bool CIpoptNLP::eval_grad_f(Index n, const Number* x, bool new_x, Number* grad_f)
{
    m_problem->EvalGradient(n, x, grad_f);
    return true;
}

bool CIpoptNLP::eval_g(Index n, const Number* x, bool new_x, Index m, Number* g)
{
    m_problem->EvalConstraint(n, x, m, g);
    return true;
}

bool CIpoptNLP::eval_jac_g(Index n, const Number* x, bool new_x, Index m, Index nele_jac, Index* iRow, Index* jCol, Number* values)
{
    if (values == nullptr)
    {
        NLPJacInfo jacinfo;
        m_problem->GetJacInfo(jacinfo);
        for (int i = 0; i < jacinfo.idxConstraint.size(); i++)
        {
            jCol[i] = jacinfo.idxVariable[i];
            iRow[i] = jacinfo.idxConstraint[i];
        }
    }
    else {
        m_problem->EvalJacobi(n, x, m, nele_jac, values);
    }
    return true;
}

bool CIpoptNLP::eval_h(Index n, const Number* x, bool new_x, Number obj_factor, Index m, const Number* lambda, bool new_lambda, Index nele_hess, Index* iRow, Index* jCol, Number* values)
{
    return false;
}

void CIpoptNLP::finalize_solution(SolverReturn status, Index n, const Number* x, const Number* z_L, const Number* z_U, Index m, const Number* g, const Number* lambda, Number obj_value, const IpoptData* ip_data, IpoptCalculatedQuantities* ip_cq)
{
    m_status = status;
    m_xopt.resize(n);
    m_gopt.resize(m);
    m_obj = obj_value;
    for (int i = 0; i < n; i++)
        m_xopt[i] = x[i];
    for (int i = 0; i < m; i++)
        m_gopt[i] = x[i];
}


err_t CNLPIpoptSolver::Solve(INLPProblem& prob, VectorXd& xopt)
{
    Ipopt::SmartPtr<CIpoptNLP> nlp = new CIpoptNLP{ this ,&prob };

    Ipopt::SmartPtr<Ipopt::IpoptApplication> app = IpoptApplicationFactory();

    app->Options()->SetNumericValue("tol", 1e-6);
    app->Options()->SetStringValue("mu_strategy", "adaptive");
    app->Options()->SetStringValue("hessian_approximation", "limited-memory");
    app->Options()->SetStringValue("output_file", "nlp-ipopt.txt");

    auto status = app->Initialize();
    status = app->OptimizeTNLP(nlp);
    xopt = nlp->m_xopt;

    if(status == Ipopt::SolverReturn::SUCCESS)
        return 0;
    return status;
}

AST_NAMESPACE_END


#endif
