/// @file      NLPSnoptSolver.cpp
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
 

 

 
 
 


#include "NLPSnoptSolver.hpp"
#ifdef AST_WITH_SNOPT

#include "csnopt/snopt.h"
#include "f2c.h"

AST_NAMESPACE_BEGIN


class CSnoptWorkspace
{
public:


protected:

};

extern "C" {

	int FunSQP_SNOPT(long* Status, long* n, double x[], long* needF, long* neF, double F[], long* needG, long* neG,
		double G[], char* cu, long* lencu, long iu[], long* leniu, double ru[], long* lenru)
	{
		INLPProblem* prob = (INLPProblem*)cu;

		prob->evalFitness(*n, x, *neF, F);

		return 0;
	}

}

err_t NLPProblem::evalNLENNZJacCOOSnJac(int ndim, const double* x_initguess, int m, std::vector<int>& iFunRow, std::vector<int>& jVarCol)
{
	NLPBounds bounds;
	// bounds.initialize(ndim, m);
	this->getBounds(bounds);

	long info;
	long neF = m + 1;
	long n = ndim;

	long neA = -1;                  // A 中非零参数个数, -1表示还未指定
	long neG = -1;                  // G 中非零参数个数, -1表示还未指定

	long lenA = neF * n;               // 这样处理是否正确
	long* iAfun = new long[lenA];     // i=iAfun(k)
	long* jAvar = new long[lenA];     // j=jAvar(k)
	double* A = new double[lenA];   // A(i,j)=A(k)

	long lenG = neF * n;               // 这样处理是否正确
	long* iGfun = new long[lenG];      // i=iGfun(k)
	long* jGvar = new long[lenG];      // j=jGvar(k)
	long iprint = 0;
	long isumm = 6;//??
	long lencw = 500;                            //算法占用的字符串空间
	long leniw = 500;                            //算法占用的整数空间
	long lenrw = 500;                            //算法占用的实数空间
	long cw_len = 8 * 500;
	char* cw = new char[8 * lencw];
	long* iw = new long[leniw];
	double* rw = new double[lenrw];

	double* x = new double[n];       //设计变量，输入初值, 输出解得值
	double* xlow = new double[n];       //设计变量下界
	double* xupp = new double[n];       //设计变量上界

	long mincw;                             //返回求解问题最少需要分配多少字符内存，在问题求解失败时可据此判断是否内存分配不足
	long miniw;                             //返回求解问题最少需要分配多少整数内存
	long minrw;                             //返回求解问题最少需要分配多少实数内存
	char* cu = (char*)this;
	long lencu = lencw;
	long* iu = iw;
	long leniu = leniw;
	double* ru = rw;
	long lenru = lenrw;
	short cu_len = cw_len;

	long nxnames = 1;                      //变量名字的个数, 1表示不需要提供每列变量的名字，=n表示每一个变量的名字均需提供
	long nFnames = 1;                      //目标及约束名字的个数, 1表示不需要提供每列的名字, =neF表示每一个名字均需提供
	char* xnames = new char[nxnames * 8];    //xnames[j]存储第j个变量的名字
	char* Fnames = new char[nFnames * 8];    //Fnames[j]存储第j个目标或约束的名字

	////内部存储空间的一个自适应分配
	snmema_(&info, &neF, &n, &nxnames, &nFnames, &neA, &neG,
		&mincw, &miniw, &minrw, cw, &lencw, iw, &leniw,
		rw, &lenrw, 8 * 500);

	long iPrt = 0; // suppresses printing
	long iSum = 0;
	long stropt_len = 0;

	if (mincw > lencw | miniw > leniw | minrw > lenrw)
	{
		// Reallocate memory while retaining the values set in sninit_
		long tlencw = lencw;
		long tleniw = leniw;
		long tlenrw = lenrw;

		// Call to this->alloc will create new values for cw, iw, rw => must save.
		char* tcw = cw;
		long* tiw = iw;
		doublereal* trw = rw;

		// Allocate new memory
		// Reset work array lengths.
		lencw = mincw;
		leniw = miniw;
		lenrw = minrw;

		// Allocate new memory for work arrays.
		cw = new char[8 * lencw];
		iw = new integer[leniw];
		rw = new doublereal[lenrw];

		// Copy in old values, previously set
		long mlencw = lencw < tlencw ? lencw : tlencw;
		long mleniw = leniw < tleniw ? leniw : tleniw;
		long mlenrw = lenrw < tlenrw ? lenrw : tlenrw;

		memcpy(cw, tcw, 8 * mlencw * sizeof(char));
		memcpy(iw, tiw, mleniw * sizeof(long));
		memcpy(rw, trw, mlenrw * sizeof(double));

		// Save the lengths of the new work arrays.		
		stropt_len = sizeof("Total real workspace   ")-1;
		snseti_(const_cast<char*>("Total real workspace   "), &lenrw, &iPrt, &iSum, &info,
			cw, &lencw, iw, &leniw, rw, &lenrw, stropt_len, 8 * 500);

		stropt_len = sizeof("Total integer workspace")-1;
		snseti_(const_cast<char*>("Total integer workspace"), &leniw, &iPrt, &iSum, &info,
			cw, &lencw, iw, &leniw, rw, &lenrw, stropt_len, 8 * 500);

		// Delete temporary work arrays
		delete[] tcw;
		delete[] tiw;
		delete[] trw;
	}

	this->GetInitialGuess(ndim, x);    //设计变量初值 


	for (int ii = 0; ii < n; ii++)
	{
		xlow[ii] = bounds.lower.variable[ii];    //设计变量下界
		xupp[ii] = bounds.upper.variable[ii];    //设计变量上界
	}

	snjac_(&info, &neF, &n, (U_fp)FunSQP_SNOPT, iAfun, jAvar, &lenA,
		&neA, A, iGfun, jGvar, &lenG, &neG,
		x, xlow, xupp, &mincw, &miniw, &minrw, cu, &lencu,
		iu, &leniu, ru, &lenru, cw, &lencw, iw, &leniw, rw, &lenrw, cu_len, cw_len
	);

	iFunRow.resize(0);
	jVarCol.resize(0);
	for (int i = 0; i < neG; i++)
	{
		if(iGfun[i] == 1)
			continue;
		iFunRow.push_back(iGfun[i] - 1 - 1);
		jVarCol.push_back(jGvar[i] - 1);
	}

	delete[] cw;
	delete[] iw;
	delete[] rw;

	delete[]iAfun;  delete[]jAvar;  delete[]A;
	delete[]iGfun;  delete[]jGvar;

	delete[]x;      delete[]xlow;   delete[]xupp;
	return 0;
}


err_t CNLPSnoptSolver::Solve(INLPProblem& prob, VectorXd& xopt)
{
	NLPInfo probinfo;
	prob.GetInfo(probinfo);
	NLPBounds bounds;
	bounds.initialize(probinfo.numVariable, probinfo.numConstraint);
	prob.GetBounds(bounds);


	//SNOPT初始化步骤1
	long iprint = 0;
	long isumm = 6;//??
	long lencw = 500;                            //算法占用的字符串空间
	long leniw = 500;                            //算法占用的整数空间
	long lenrw = 500;                            //算法占用的实数空间
	long cw_len = 8 * 500;
	char* cw = new char[8 * lencw];
	long* iw = new long[leniw];
	double* rw = new double[lenrw];

	sninit_(&iprint, &isumm, cw, &lencw, iw, &leniw, rw, &lenrw, cw_len);


	long start = 0;                   //  启动方式 0-Cold, 1-Basis, 2-warm (便于接口提供初值); 
	long n = probinfo.numVariable;      //  设计变量个数
	//long neF   = m_nEquaSubNum+m_nInequaSubNum+1;//  约束个数+1, 设计变量上下界约束不在此处理多出的1指目标函数
	long neF = probinfo.numConstraint + 1;//  约束个数+1, 设计变量上下界约束不在此处理多出的1指目标函数
	// A代表目标与约束中线性部分的系数-也即偏导数，G代表非线性部分的偏导数，维数均为neF*n, A与G的和为Jacobian

	long neA = -1;                  // A 中非零参数个数, -1表示还未指定
	long neG = -1;                  // G 中非零参数个数, -1表示还未指定

	long lenA = neF * n;               // 这样处理是否正确
	long* iAfun = new long[lenA];     // i=iAfun(k)
	long* jAvar = new long[lenA];     // j=jAvar(k)
	double* A = new double[lenA];   // A(i,j)=A(k)

	long lenG = neF * n;               // 这样处理是否正确
	long* iGfun = new long[lenG];      // i=iGfun(k)
	long* jGvar = new long[lenG];      // j=jGvar(k)

	double* x = new double[n];       //设计变量，输入初值, 输出解得值
	double* xlow = new double[n];       //设计变量下界
	double* xupp = new double[n];       //设计变量上界
	double* xmul = new double[n];       //输入及输出拉格朗日乘子值
	long* xstate = new   long[n];       //标识设计变量值情况

	double* F = new double[neF];     //输出目标函数及约束函数值
	double* Flow = new double[neF];     //目标及约束下界 
	double* Fupp = new double[neF];     //目标及约束上界
	double* Fmul = new double[neF];     //输入及输出拉格朗日乘子值
	long* Fstate = new   long[neF];     //标识目标及约束值情况

	long nxnames = 1;                      //变量名字的个数, 1表示不需要提供每列变量的名字，=n表示每一个变量的名字均需提供
	long nFnames = 1;                      //目标及约束名字的个数, 1表示不需要提供每列的名字, =neF表示每一个名字均需提供
	char* xnames = new char[nxnames * 8];    //xnames[j]存储第j个变量的名字
	char* Fnames = new char[nFnames * 8];    //Fnames[j]存储第j个目标或约束的名字

	prob.GetInitialGuess(probinfo.numVariable, x);    //设计变量初值 

	

	long   ObjRow = 1;                     //目标函数在F中所在的行，默认值为第1行, 算法优化的真正目标是 ObjAdd+F[ObjRow].=0意味着寻找可行解
	double ObjAdd = 0;                     //加在目标上的常数，默认0
	int ii;
	for (ii = 0; ii < n; ii++)
	{
		xlow[ii] = bounds.lower.variable[ii];    //设计变量下界
		xupp[ii] = bounds.upper.variable[ii];    //设计变量上界
		// x[ii] = OptiVari[ii];         //设计变量初值 
		xstate[ii] = 0;                    //1标识采用接口初值, 0标识文件初值或初值可忽略   
		xmul[ii] = 0;
	}

	Flow[0] = -1e20;                       //目标函数上下界不约束，设置为无穷大
	Fupp[0] = 1e20;
	F[0] = 0; //m_pPlan->m_Object;
	for (ii = 0; ii < probinfo.numConstraint; ii++)       //约束上下界
	{
		Flow[ii + 1] = bounds.lower.constraint[ii];
		Fupp[ii + 1] = bounds.upper.constraint[ii];
		F[ii + 1] = 0; //m_pPlan->m_EquaSub[ii];
	}

	for (ii = 0; ii < neF; ii++)
	{
		Fstate[ii] = 0;                    //标识目标及约束值情况
		Fmul[ii] = 0;                      //拉格朗日乘子初值，如果不知道设为零
	}


	long info;                              //返回求解成功失败标识0-成功, 1-满足最优性条件, 2-找到可行解, 3-达不到指定精度, 10-问题似乎不可行

	long mincw;                             //返回求解问题最少需要分配多少字符内存，在问题求解失败时可据此判断是否内存分配不足
	long miniw;                             //返回求解问题最少需要分配多少整数内存
	long minrw;                             //返回求解问题最少需要分配多少实数内存

	//下面这几个数组本来是用来给用户定义函数单独传设计变量之外的数据, 这里设置为算法自己的空间，用户函数不应使用这几个接口参数
	char* cu = (char*) &prob;
	long lencu = lencw;
	long* iu = iw;
	long leniu = leniw;
	double* ru = rw;
	long lenru = lenrw;
	short cu_len = cw_len;

	const char* Prob = "SQP_NOPT";
	long npname = sizeof(Prob) - 1;

	long nInf, nS;                   //nInf-不可行个数, nS-the final number of superbasics
	double sInf;                    //不可行的和



	////内部存储空间的一个自适应分配
	snmema_(&info, &neF, &n, &nxnames, &nFnames, &neA, &neG,
		&mincw, &miniw, &minrw, cw, &lencw, iw, &leniw,
		rw, &lenrw, 8 * 500);

	long iPrt = 0; // suppresses printing
	long iSum = 0;
	long stropt_len = 0;

	if (mincw > lencw | miniw > leniw | minrw > lenrw)
	{
		// Reallocate memory while retaining the values set in sninit_
		long tlencw = lencw;
		long tleniw = leniw;
		long tlenrw = lenrw;

		// Call to this->alloc will create new values for cw, iw, rw => must save.
		char* tcw = cw;
		long* tiw = iw;
		doublereal* trw = rw;

		// Allocate new memory
		// Reset work array lengths.
		lencw = mincw;
		leniw = miniw;
		lenrw = minrw;

		// Allocate new memory for work arrays.
		cw = new char[8 * lencw];
		iw = new integer[leniw];
		rw = new doublereal[lenrw];

		// Copy in old values, previously set
		long mlencw = lencw < tlencw ? lencw : tlencw;
		long mleniw = leniw < tleniw ? leniw : tleniw;
		long mlenrw = lenrw < tlenrw ? lenrw : tlenrw;

		memcpy(cw, tcw, 8 * mlencw * sizeof(char));
		memcpy(iw, tiw, mleniw * sizeof(long));
		memcpy(rw, trw, mlenrw * sizeof(double));

		// Save the lengths of the new work arrays.		
		stropt_len = sizeof("Total real workspace   ")-1;
		snseti_(const_cast<char*>("Total real workspace   "), &lenrw, &iPrt, &iSum, &info,
			cw, &lencw, iw, &leniw, rw, &lenrw, stropt_len, 8 * 500);

		stropt_len = sizeof("Total integer workspace") - 1;
		snseti_(const_cast<char*>("Total integer workspace"), &leniw, &iPrt, &iSum, &info,
			cw, &lencw, iw, &leniw, rw, &lenrw, stropt_len, 8 * 500);

		// Delete temporary work arrays
		delete[] tcw;
		delete[] tiw;
		delete[] trw;
	}


	long MajorIt = options.major_iteration_limit;
	stropt_len = sizeof("Major Iterations Limit") - 1;  //"Minor Iterations Limit" //Iteration Limit
	snseti_(const_cast<char*>("Major Iterations Limit"), &MajorIt, &iPrt, &iSum, &info,
		cw, &lencw, iw, &leniw, rw, &lenrw, stropt_len, 8 * 500);


	snjac_(&info, &neF, &n, (U_fp)FunSQP_SNOPT, iAfun, jAvar, &lenA,
		&neA, A, iGfun, jGvar, &lenG, &neG,
		x, xlow, xupp, &mincw, &miniw, &minrw, cu, &lencu,
		iu, &leniu, ru, &lenru, cw, &lencw, iw, &leniw, rw, &lenrw, cu_len, cw_len);


	snopta_(&start, &neF, &n, &nxnames, &nFnames, &ObjAdd, &ObjRow, const_cast<char*>(Prob), (U_fp)FunSQP_SNOPT,
		iAfun, jAvar, &lenA, &neA, A, iGfun, jGvar, &lenG, &neG,
		xlow, xupp, xnames, Flow, Fupp, Fnames, x, xstate, xmul, F, Fstate, Fmul,
		&info, &mincw, &miniw, &minrw, &nS, &nInf, &sInf,
		cu, &lencu, iu, &leniu, ru, &lenru,
		cw, &lencw, iw, &leniw, rw, &lenrw,
		npname, nxnames * 8, nFnames * 8, cu_len, cw_len);

	xopt.resize(probinfo.numVariable);
	for (int i = 0; i < probinfo.numVariable; i++)
	{
		xopt[i] = x[i];                  //反馈最优解
	}

	delete[] cw;
	delete[] iw;
	delete[] rw;

	delete[]iAfun;  delete[]jAvar;  delete[]A;
	delete[]iGfun;  delete[]jGvar;

	delete[]x;      delete[]xlow;   delete[]xupp;
	delete[]xmul;   delete[]xstate;

	delete[]F;      delete[]Flow;   delete[]Fupp;
	delete[]Fmul;   delete[]Fstate;

	delete[]xnames; delete[]Fnames;

    return err_t();
}

AST_NAMESPACE_END

#endif

