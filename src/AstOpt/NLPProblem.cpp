/// @file      NLPProblem.cpp
/// @brief     
/// @details   ~
/// @author    axel
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
 
 
#include "NLPProblem.hpp"
#include "AstCore/MathOperator.hpp"
#include "AstUtil/LocalBuffer.hpp"
#include <memory>			// for std::shared_ptr
#include <algorithm>		// for std::copy_n std::fill_n
#include <cstdio>			// for printf
#include <cmath>			// for std::isnan

#define INFBND 1.1e20

 
AST_NAMESPACE_BEGIN
 
 


/// @brief 检测浮点模式中特殊值行为是否符合IEEE754规范
/// @return 
bool aCheckFloatingBehaviorIEEE754()
{
	float gf0 = -0.0;
	float f1 = INFINITY;
	float f2 = NAN;
	float f3 = -INFINITY;
	bool a, b;
	float c, d, e;
	a = (f1 == f1);
	b = (f2 == f2);
	c = (f1 - f1);
	d = (f2 - f2);
	e = (gf0 / f3);
	// printf("INFINITY == INFINITY : %d\n", a);
	// printf("NAN == NAN           : %d\n", b);
	// printf("INFINITY - INFINITY  : %f\n", c);
	// printf("NAN - NAN            : %f\n", d);
	// printf("std::signbit(-0.0/-INFINITY): %d\n", std::signbit(e));
	if (a && !b && std::isnan(c) && std::isnan(d) && std::signbit(e) == 0)
		return true;
	//if ((INFINITY == INFINITY) && !(NAN == NAN) && std::isnan(INFINITY - INFINITY) && std::isnan(NAN - NAN) && std::signbit(-0.0 / -INFINITY) == 0)
	//	return true;
	return 0;
}

/// @brief 打印稀疏模式，零元素用 * 表示，0-nnz表示非零元素的内存排序
/// @tparam T 整数类型: int/long
/// @param nnz 非零元素个数
/// @param iFun 行 T[nnz]
/// @param jVar 列 T[nnz]
/// @param idx_style 行列索引从idx_style开始
void aPrintSparityPatternCOO(int nnz, int* iFun, int* jVar, int idx_style)
{
	int row = 0, col = 0;
	for (int i = 0; i < nnz; i++) {
		if (iFun[i] + 1 - idx_style > row) {
			row = iFun[i] + 1 - idx_style;
		}
		if (jVar[i] + 1 - idx_style > col) {
			col = jVar[i] + 1 - idx_style;
		}
	}
	for (int i = 0; i < row; i++) {
		for (int j = 0; j < col; j++) {
			bool found = false;
			for (int idx = 0; idx < nnz; idx++) {
				if ((iFun[idx] == i - idx_style) && (jVar[idx] == j - idx_style)) {
					printf("%d\t", idx);
					found = true;
					break;
				}
			}
			if (!found) {
				printf("*\t");
			}
		}
		printf("\n");
	}
}

NLPProblem::NLPProblem(INLPProblem* problem)
	:m_problem{problem}
{
	assert(problem != nullptr);
	// 1. 获取非线性问题的维度
	this->getInfo(m_probInfo);
}

NLPProblem::~NLPProblem()
{
	if(m_problem)
		delete m_problem;
}

err_t NLPProblem::getInfo(NLPInfo& info) const
{
	return m_problem->getInfo(info);
}

err_t NLPProblem::getJacInfo(NLPJacInfo& info) const
{
	// 这里的默认实现简单认为雅可比矩阵为稠密矩阵

	NLPInfo probinfo;
	this->getInfo(probinfo);
	int numConstraint = probinfo.getNumConstraint();	// 约束维度
	int numVariable = probinfo.getNumVariable();		// 变量维度

	int len = numConstraint * numVariable;
	info.idxConstraint_.resize(len);
	info.idxVariable_.resize(len);
	int idx = 0;
	for (int j = 0; j < numConstraint; j++)
	{
		for (int i = 0; i < numVariable; i++)
		{
			info.idxConstraint_[idx] = j;
			info.idxVariable_[idx] = i;
			idx ++;
		}
	}
	return 0;
}

err_t NLPProblem::getBounds(NLPBounds& bounds) const
{
	// 1. 先设置默认值

	if (bounds.variable().size() > 0)
	{
		// 变量的默认值：-∞ <= variable <= +∞
		assert(bounds.variable_.lower_);
		assert(bounds.variable_.upper_);
		std::fill_n(bounds.variable_.lower_, bounds.variable_.size(), -INFBND);
		std::fill_n(bounds.variable_.upper_, bounds.variable_.size(), +INFBND);
	}
	if (bounds.constraintEq_.size() > 0)
	{
		// 等式约束的默认值： 0
		assert(bounds.constraintEq_.data_);
		std::fill_n(bounds.constraintEq_.data_, bounds.constraintEq_.size(), 0);
	}
	if (bounds.constraintIneq_.size() > 0)
	{
		// 等式约束的默认值： -∞ <= constrIneq <= 0
		assert(bounds.constraintIneq_.lower_);
		assert(bounds.constraintIneq_.upper_);
		std::fill_n(bounds.constraintIneq_.lower_, bounds.constraintIneq_.size(), -INFBND);
		std::fill_n(bounds.constraintIneq_.upper_, bounds.constraintIneq_.size(), 0);
	}

	// 2. 然后调用算法接口覆盖上面的默认值
	return m_problem->getBounds(bounds);
}

err_t NLPProblem::evalFitness(const NLPInput& input, NLPOutput& output) const
{
	return m_problem->evalFitness(input, output);
}

std::vector<double> NLPProblem::evalFitness(const std::vector<double>& x) const
{
	std::vector<double> f;
	err_t err = this->evalFitness(x, f);
	A_UNUSED(err);
	return f;
}

err_t NLPProblem::evalFitness(const std::vector<double>& x, std::vector<double>& f) const
{
	// @todo: check input

	f.resize(m_probInfo.getNumObjConstr());
	double* constraint = f.data();
	
	NLPInput input{};
	NLPOutput output{};
	input.variable_.size_ = x.size();
	input.variable_.data_ = (double*)x.data();

	output.objective_.size_ = m_probInfo.getNumObjective();
	output.objective_.data_ = constraint;
	constraint += output.objective_.size_;

	output.constraintEq_.size_ = m_probInfo.getNumConstraintEq();
	output.constraintEq_.data_ = constraint;
	constraint += output.constraintEq_.size_;

	output.constraintIneq_.size_ = m_probInfo.getNumConstraintIneq();
	output.constraintIneq_.data_ = constraint + output.constraintEq_.size_;

	return evalFitness(input, output);
}

std::vector<double> NLPProblem::evalConstraint(const std::vector<double>& variable) const
{
	std::vector<double> c;
	err_t err = this->evalConstraint(variable, c);
	A_UNUSED(err);
	return c;
	// moving a local object in a return statement prevents copy elision [-Werror=pessimizing-move]
	// return std::move(c);
}

err_t NLPProblem::evalConstraint(const std::vector<double>& variable, std::vector<double>& constraint) const
{
	int numConstraint = m_probInfo.getNumConstraint();
	constraint.resize(numConstraint);
	return this->evalConstraint((int)variable.size(), variable.data(), (int)numConstraint, constraint.data());
}

err_t NLPProblem::evalConstraint(int numVariable, const double* variable, int numConstraint, double* constraint) const
{
	// @todo check input and output

	NLPInput input{};
	NLPOutput output{};
	input.variable_.size_ = numVariable;
	input.variable_.data_ = (double*)variable;

	output.constraintEq_.size_ = m_probInfo.getNumConstraintEq();
	output.constraintEq_.data_ = constraint;

	output.constraintIneq_.size_ = m_probInfo.getNumConstraintIneq();
	output.constraintIneq_.data_ = constraint + output.constraintEq_.size_;

	return evalFitness(input, output);
}

err_t NLPProblem::evalObjective(int numVariable, const double* variable, int numObjective, double* objective) const
{
	// @todo check input and output

	NLPInput input{};
	NLPOutput output{};
	input.variable_.size_ = numVariable;
	input.variable_.data_ = (double*)variable;

	output.objective_.data_ = objective;
	output.objective_.size_ = numObjective;

	return evalFitness(input, output);
}


err_t NLPProblem::evalObjective(int numVariable, const double* variable, double& objective) const
{
	return evalObjective(numVariable, variable, 1, &objective);
}


err_t NLPProblem::evalFitness(int numVariable, double* variable, int numObjective, double* objective, int numConstrEq, double* constrEq, int numConstrIneq, double* constrIneq) const
{
	NLPInput input{};
	NLPOutput output{};
	input.variable_.size_ = numVariable;
	input.variable_.data_ = variable;

	output.objective_.data_ = objective;
	output.objective_.size_ = numObjective;
	output.constraintEq_.size_ = numConstrEq;
	output.constraintEq_.data_ = constrEq;
	output.constraintIneq_.size_ = numConstrIneq;
	output.constraintIneq_.data_ = constrIneq;

	return evalFitness(input, output);
}

err_t NLPProblem::evalJacobi(int ndim, const double* x_input, int numConstraint, int numJacobiElem, double* nzElemjacobi) const
{
	// printf("EvalJacobi(...) method is not overridden\n");
	return evalNLEJacobiCD(0.1, ndim, x_input, numConstraint, nzElemjacobi);
}

err_t NLPProblem::evalGradient(int numVariable, const double* variable, double* grad) const
{
	return evalGradientCD(0.1, numVariable, variable, grad);
}

err_t NLPProblem::getInitialGuess(int numVariable, double *variable) const
{
	// 这里的实现：简单取变量初值为变量上下界的平均值
	err_t err;
	
	// NLPInfo info;
	// err = this->getInfo(info);
	// if(err) return err;
	// int numVariable = info.getNumVariable();

	std::vector<double> lb(numVariable), ub(numVariable);
	NLPBounds bounds{};
	bounds.variable_.size() = numVariable;
	bounds.variable_.lower_ = lb.data();
	bounds.variable_.upper_ = ub.data();

	err = this->getBounds(bounds);
	if (err) return err;
	for (int i = 0; i < numVariable; i ++)
	{
		variable[i] = (lb[i] + ub[i]) / 2;
	}
    return err;
}

err_t NLPProblem::evalNLEJacobiCCSFD(double ustep, int ndim, const double* x_input, int m, const int* iFuncRow, const int* idxNNZElem, double* jacobi_sparse_value) const
{
	A_LOCAL_BUFFER(double, fvec, 3 * ndim);
	double* x = fvec + ndim;
	double* fvecnew = x + ndim;
	std::copy_n(x_input, ndim, x);
	if (ustep == 0) {
		ustep = sqrt(eps(1));
	}
	// 考虑数值舍入误差，最小的步长理论上是eps(x)约等于x*eps(1) > eps(x)
	// 在minpack，默认的步长为x*sqrt(eps(1)) > x*eps(1) > eps(x)
	double temp, h;
	err_t err;
	err = evalConstraint(ndim, x_input, m, fvec);
	if (err) {
		return err;
	}

	for (int jCol = 0; jCol < ndim; jCol++) {
		temp = x_input[jCol];
		h = ustep * (1 + fabs(temp));
		x[jCol] = temp + h;
		err = evalConstraint(ndim, x,  m, fvecnew);
		if (err) {
			return err;
		}
		for (int idx = idxNNZElem[jCol]; idx < idxNNZElem[jCol + 1]; idx++)
		{
			int iRow = iFuncRow[idx];
			jacobi_sparse_value[idx] = (fvecnew[iRow] - fvec[iRow]) / h;
		}
		x[jCol] = temp;
	}
	return 0;
}



err_t NLPProblem::evalGradientCD(double ustep, int n, const double* x_input, double* grad) const
{
	A_LOCAL_BUFFER(double, x1, 3 * n);
	double* x2 = x1 + n;

	std::copy_n(x_input, n, x1);
	std::copy_n(x_input, n, x2);
	if (ustep == 0) {
		ustep = sqrt(eps(1));
	}
	// 考虑数值舍入误差，最小的步长理论上是eps(x)约等于x*eps(1) > eps(x)
	// 在minpack，默认的步长为x*sqrt(eps(1)) > x*eps(1) > eps(x)
	double temp, h;
	err_t err;
	double f_new1;
	double f_new2;
	for (int j = 0; j < n; ++j) {
		temp = x1[j];
		h = ustep * (1 + fabs(temp));
		x1[j] = temp - h / 2;
		x2[j] = temp + h / 2;
		err = evalObjective(n, x1, f_new1);
		if (err) {
			return err;
		}
		err = evalObjective(n, x2, f_new2);
		if (err) {
			return err;
		}
		x1[j] = temp;
		x2[j] = temp;
		grad[j] = (f_new2 - f_new1) / h;
	}
	return 0;
}


err_t NLPProblem::evalGradientFD(double ustep, int n, const double* x_input, double* grad) const
{
	A_LOCAL_BUFFER(double, x, n);
	std::copy_n(x_input, n, x);
	if (ustep == 0) {
		ustep = sqrt(eps(1));
	}
	double f;
	double f_new;
	double temp, h;
	err_t err;
	err = evalObjective(n, x_input, f);
	if (err) {
		return err;
	}
	for (int j = 0; j < n; ++j) {
		temp = x[j];
		h = ustep * (1 + fabs(temp));

		x[j] = temp + h;
		err = evalObjective(n, x, f_new);
		if (err) {
			return err;
		}
		x[j] = temp;
		grad[j] = (f_new - f) / h;
	}
	return 0;
}


err_t NLPProblem::evalGradientBD(double ustep, int n, const double* x_input, double* grad) const
{
	A_LOCAL_BUFFER(double, x, n);
	std::copy_n(x_input, n, x);
	if (ustep == 0) {
		ustep = sqrt(eps(1));
	}
	double f;
	double f_new;
	double temp, h;
	err_t err;
	err = evalObjective(n, x_input, f);
	if (err) {
		return err;
	}
	for (int j = 0; j < n; ++j) {
		temp = x[j];
		h = -ustep * (1 + fabs(temp));

		x[j] = temp + h;
		err = evalObjective(n, x, f_new);
		if (err) {
			return err;
		}
		x[j] = temp;
		grad[j] = (f_new - f) / h;
	}
	return 0;
}

err_t NLPProblem::evalNLEJacobiFD( double ustep, int n, const double* x_input, int m, double* colmaj_jacobi) const
{
	A_LOCAL_BUFFER(double, fvec, 3*n);
	double* x       = fvec + n;
	double* fvecnew = x + n;
	std::copy_n(x_input, n, x);
	if (ustep == 0) {
		ustep = sqrt(eps(1));
	}
	// 考虑数值舍入误差，最小的步长理论上是eps(x)约等于x*eps(1) > eps(x)
	// 在minpack，默认的步长为x*sqrt(eps(1)) > x*eps(1) > eps(x)
	double temp, h;
	err_t err;
	err = evalConstraint(n, x_input, m, fvec);
	if (err) {
		return err;
	}
	for (int j = 0; j < n; ++j) {
		temp = x[j];
		h = ustep * (1 + fabs(temp));

		x[j] = temp + h;
		err = evalConstraint(n, x, m, fvecnew);
		if (err) {
			return err;
		}
		x[j] = temp;
		for (int i = 0; i < m; ++i) {
			colmaj_jacobi[i + j * m] = (fvecnew[i] - fvec[i]) / h;
		}
	}
	return 0;
}

err_t NLPProblem::evalNLEJacobiBD(double ustep, int n, const double* x_input, int m, double* colmaj_jacobi) const
{
	A_LOCAL_BUFFER(double, fvec, 3 * n);
	double* x = fvec + n;
	double* fvecnew = x + n;
	std::copy_n(x_input, n, x);
	if (ustep == 0) {
		ustep = sqrt(eps(1));
	}
	// 考虑数值舍入误差，最小的步长理论上是eps(x)约等于x*eps(1) > eps(x)
	// 在minpack，默认的步长为x*sqrt(eps(1)) > x*eps(1) > eps(x)
	double temp, h;
	err_t err;
	err = evalConstraint(n, x_input, m, fvec);
	if (err) {
		return err;
	}
	for (int j = 0; j < n; ++j) {
		temp = x[j];
		h = -ustep * (1 + fabs(temp));
		x[j] = temp + h;
		err = evalConstraint(n, x, m, fvecnew);
		if (err) {
			return err;
		}
		x[j] = temp;
		for (int i = 0; i < m; ++i) {
			colmaj_jacobi[i + j * m] = (fvecnew[i] - fvec[i]) / h;
		}
	}
	return 0;
}


err_t NLPProblem::evalNLEJacobiCD(double ustep, int n, const double* x_input, int m, double* colmaj_jacobi) const
{
	A_LOCAL_BUFFER(double, x1, 5 * n);
	double* x2 = x1 + n;
	double* fvecnew1 = x2 + n;
	double* fvecnew2 = fvecnew1 + n;
	std::copy_n(x_input, n, x1);
	std::copy_n(x_input, n, x2);
	if (ustep == 0) {
		ustep = sqrt(eps(1));
	}
	// 考虑数值舍入误差，最小的步长理论上是eps(x)约等于x*eps(1) > eps(x)
	// 在minpack，默认的步长为x*sqrt(eps(1)) > x*eps(1) > eps(x)
	double temp, h;
	err_t err;
	for (int j = 0; j < n; ++j) {
		temp = x1[j];
		h = ustep * (1 + fabs(temp));
		x1[j] = temp - h / 2;
		x2[j] = temp + h / 2;
		err = evalConstraint(n, x1, m, fvecnew1);
		if (err) {
			return err;
		}
		err = evalConstraint(n, x2, m, fvecnew2);
		if (err) {
			return err;
		}
		x1[j] = temp;
		x2[j] = temp;
		for (int i = 0; i < m; ++i) {
			colmaj_jacobi[i + j * m] = (fvecnew2[i] - fvecnew1[i]) / h;
		}
	}
	return 0;
}

err_t NLPProblem::evalNLENNZJacCCSNan(int ndim, const double* x_initguess, int m, std::vector<int>& iFunRow, std::vector<int>& idxNNZElem) const
{
	bool iflag;
	iflag = aCheckFloatingBehaviorIEEE754();
	if (!iflag) {
		printf("[error]: Compiling EvalNLENNZJacCCSNan method with floating-point behavior satisfying IEEE754 \n");
		return -1;
	}
	A_LOCAL_BUFFER(double, x, 2 * ndim);
	double* fvec = x + ndim;
	std::copy_n(x_initguess, ndim, x);
	double temp;

	iFunRow.clear();
	idxNNZElem.clear();
	for (int j = 0; j < ndim; j++)
	{
		temp = x[j];
		x[j] = NAN;
		err_t err = evalConstraint(ndim, x, m, fvec);
		if (err) {
			return err;
		}
		idxNNZElem.push_back((int)iFunRow.size());
		for (int i = 0; i < m; i++) {
			if (std::isnan(fvec[i])) {
				iFunRow.push_back(i);
			}
		}
		x[j] = temp;
	}
	idxNNZElem.push_back((int)iFunRow.size());
	return 0;
}

err_t NLPProblem::evalNLENNZJacNan(int ndim, const double* x_initguess, int m, int& nnz_jac) const
{
	std::vector<int> t1, t2;
	err_t rc =  evalNLENNZJacCOONan(ndim, x_initguess, m, t1, t2);
	nnz_jac = (int)t1.size();
	return rc;
}

err_t NLPProblem::evalNLENNZJacCOONan(int ndim, const double* x_initguess, int m, std::vector<int>& iFunRow, std::vector<int>& jVarCol) const
{
	A_LOCAL_BUFFER(double, x, 2 * ndim);
	double* fvec = x + ndim;
	std::copy_n(x_initguess, ndim, x);
	double temp;
	err_t err;
	iFunRow.clear();
	jVarCol.clear();
	for (int j = 0; j < ndim; j++)
	{
		temp = x[j];
		x[j] = NAN;
		err = evalConstraint(ndim, x, m, fvec);
		if (err) {
			return err;
		}
		for (int i = 0; i < ndim; i++) {
			if (std::isnan(fvec[i])) {
				iFunRow.push_back(i);
				jVarCol.push_back(j);
			}
		}
		x[j] = temp;
	}
	return 0;
}




void aParternCOOToCCS(
	const std::vector<int>& iRow, const std::vector<int>& jCol,
	std::vector<int>& ptr2fnz, std::vector<int>& indexRow)
{
	// 检查输入有效性
	if (iRow.size() != jCol.size()) {
		ptr2fnz = { 0 };
		// printf("COO格式的行列索引数组大小必须相同"); // COO格式的行列索引数组大小必须相同
	}

	if (iRow.empty()) {
		ptr2fnz.clear();
		// CCS格式的列指针数组至少有一个元素0
		ptr2fnz = { 0 };
		return;
	}

	// 获取矩阵的列数
	int maxCol = *std::max_element(jCol.begin(), jCol.end());
	int numCols = maxCol + 1;

	// 临时存储每列的非零元素信息
	std::vector<std::vector<int>> colData(numCols);

	// 按列分组存储行索引
	for (size_t i = 0; i < iRow.size(); ++i) {
		int col = jCol[i];
		int row = iRow[i];
		colData[col].push_back(row);
	}

	// 构建CCS格式的输出
	ptr2fnz.clear();
	indexRow.clear();
	ptr2fnz.push_back(0); // 第一列起始位置为0

	// 按列顺序填充数据
	for (int col = 0; col < numCols; ++col) {
		// 对每列的行索引排序（CCS通常按行索引排序）
		std::sort(colData[col].begin(), colData[col].end());

		// 添加到行索引数组
		indexRow.insert(indexRow.end(), colData[col].begin(), colData[col].end());

		// 更新列指针
		ptr2fnz.push_back(static_cast<int>(indexRow.size()));
	}
}

void aParternCCSToCOO(
	const std::vector<int>&ptr2fnz, const std::vector<int>&indexRow, 
	std::vector<int>&iFunRow, std::vector<int>&jVarCol)
{
	jVarCol.clear();
	iFunRow.clear();
	int ncols = (int)ptr2fnz.size() - 1;
	for (int _c = 0; _c < ncols; _c++) {
		for (int i = ptr2fnz[_c]; i < ptr2fnz[_c + 1]; i++)
		{
			int _r = indexRow[i];
			iFunRow.push_back(_r);
			jVarCol.push_back(_c);
		}
	}
}


AST_NAMESPACE_END
