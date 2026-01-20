///
/// @file      GravityCalculator3.cpp
/// @brief     ~
/// @details   ~
/// @author    axel
/// @date      2026-01-15
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

#include "GravityCalculator.hpp"
#include "AstUtil/Logger.hpp"
#include "AstUtil/StringView.hpp"
#include "AstMath/Vector.hpp"

AST_NAMESPACE_BEGIN

typedef int Integer;
typedef double Real;

static void AllocateArray(Real**& a, const Integer& nn, const Integer& excess)
{
    // Allocate out to full m, regardless of M_FileOrder
    a = new Real*[nn+1+excess];
    for (Integer n=0;  n<=nn+1+excess-1;  ++n)
    {
        a[n] = new Real[nn+1+excess];   // wcs 2011.06.02 n -> nn
        for (Integer m=0;  m<=nn+1+excess-1;  ++m)   // wcs 2011.06.02  n -> nn
            a[n][m] = 0.0;
    }
}

static void AllocateArray(Real*& a, const Integer& nn, const Integer& excess)
{
    a = new Real[nn+1+excess];
    for (Integer n=0;  n<=nn+1+excess-1;  ++n)
        a[n] = 0.0;
}
//------------------------------------------------------------------------------
static void DeallocateArray(Real**& a, const Integer& nn, const Integer& excess)
{
    if (a != NULL)
    {
        for (Integer n=0;  n<=nn+1+excess-1;  ++n)
        {
            if (a[n] != NULL)
            {
                delete[] a[n];
                a[n] = NULL;
            }
        }
        delete[] a;
        a = NULL;
    }
}
//------------------------------------------------------------------------------
static void DeallocateArray(Real*& a, const Integer& nn, const Integer& excess)
{
    if (a != NULL)
    {
        delete[] a;
        a = NULL;
    }
}

GravityCalculator3::GravityCalculator3()
    : GravityCalculator()
    , Factor(0.0)
{
    init();
}

GravityCalculator3::GravityCalculator3(const GravityField &gravityField, int degree, int order)
    : GravityCalculator(gravityField, degree, order)
    , Factor(0.0)
{
    init();
}

GravityCalculator3::GravityCalculator3(StringView gravityFilePath, int degree, int order)
    : GravityCalculator(gravityFilePath, degree, order)
    , Factor(0.0)
{
    init();
}

GravityCalculator3::~GravityCalculator3()
{
    deinit();
}

void GravityCalculator3::init()
{
    getGravityField().normalize();
    const int MM = getOrder();
    const int NN = getDegree();
    const int nn = NN;
    const int mm = MM;
    Factor = -getGravityField().getGM();

    AllocateArray(C,NN,0);
    AllocateArray(S,NN,0);
    AllocateArray(A,NN,3);
    AllocateArray(V,NN,3);
    AllocateArray(Re,NN,3);
    AllocateArray(Im,NN,3);
    AllocateArray(N1,NN,3);
    AllocateArray(N2,NN,3);
    AllocateArray(VR01,NN,0);
    AllocateArray(VR11,NN,0);
    AllocateArray(VR02,NN,0);
    AllocateArray(VR12,NN,0);
    AllocateArray(VR22,NN,0);

    // initialize the diagonal elements (not a function of the input)
    A[0][0] = 1.0;
    for (Integer n=1;  n<=NN+2;  ++n)
        A[n][n] = sqrt (Real(2*n+1)/Real(2*n)) * A[n-1][n-1];

    // Compute normalization coefficients V(n,m)     V(0..degree,0..order)
    //   V(n,0) = sqrt (2n+1)
    //   V(n,m) = sqrt (2(2n+1) * (n-m)! / (n+m)! )
    //   note that
    //       V(n,m) = V(n,m-1) / sqrt( (n+m) * (n-m+1) )
    for (Integer n=0;  n<=NN+2;  ++n)
    {
        V[n][0] = sqrt(Real(2*(2*n+1)));   // Temporary, to make following loop work
        for (Integer m=1;  m<=n+2 && m<=MM+2;  ++m)
        {
            V[n][m] = V[n][m-1] / sqrt(Real((n+m)*(n-m+1)));
        }
        V[n][0] = sqrt(Real(2*n+1));       // Now set true value
    }
    for (Integer n=0;  n<=NN;  ++n)
        for (Integer m=0;  m<=n && m<=MM;  ++m)
        {
            Real nn = n;
            VR01[n][m] = sqrt(Real((nn-m)*(nn+m+1)));
            VR11[n][m] = sqrt(Real((2*nn+1)*(nn+m+2)*(nn+m+1))/Real((2*nn+3)));
            VR02[n][m] = sqrt(Real((nn-m)*(nn-m-1)*(nn+m+1)*(nn+m+2))) ;
            VR12[n][m] = sqrt(Real(2*nn+1)/Real(2*nn+3)*Real((nn-m)*(nn+m+1)*(nn+m+2)*(nn+m+3)));
            VR22[n][m] = sqrt(Real(2*nn+1)/Real(2*nn+5)*Real((nn+m+1)*(nn+m+2)*(nn+m+3)*(nn+m+4)));
            if (m==0) 
            {
                VR01[n][m] /= sqrt(Real(2));
                VR11[n][m] /= sqrt(Real(2));
                VR02[n][m] /= sqrt(Real(2));
                VR12[n][m] /= sqrt(Real(2));
                VR22[n][m] /= sqrt(Real(2));
            }
        }

    for (Integer m=0;  m<=MM+2;  ++m)
    {
        for (Integer n=m+2;  n<=NN+2;  ++n)
        {
            N1[n][m] = sqrt (Real((2*n+1)*(2*n-1)) / Real((n-m)*(n+m)));
            N2[n][m] = sqrt (Real((2*n+1)*(n-m-1)*(n+m-1)) / 
                          Real((2*n-3)*(n+m)*(n-m)));
        }
    }
}

void GravityCalculator3::deinit()
{
    const int MM = getOrder();
    const int NN = getDegree();
    
    DeallocateArray(C,NN,0);
    DeallocateArray(S,NN,0);
    DeallocateArray(A,NN,3);
    DeallocateArray(V,NN,3);
    DeallocateArray(Re,NN,3);
    DeallocateArray(Im,NN,3);
    DeallocateArray(N1,NN,3);
    DeallocateArray(N2,NN,3);
    DeallocateArray(VR01,NN,0);
    DeallocateArray(VR11,NN,0);
    DeallocateArray(VR02,NN,0);
    DeallocateArray(VR12,NN,0);
    DeallocateArray(VR22,NN,0);
}

void GravityCalculator3::calcPertAcceleration(const Vector3d &positionCBF, Vector3d &accelerationCBF)
{
    const bool fillgradient = false;
    auto& pos = positionCBF;
    auto& acc = accelerationCBF;
    const int MM = getOrder();
    const int NN = getDegree();
    const int nn = NN;
    const int mm = MM;
    const double FieldRadius = getGravityField().getRefDistance();

    Integer XS = fillgradient ? 2 : 1;
    // calculate vector components ----------------------------------
    Real r = sqrt (pos[0]*pos[0] + pos[1]*pos[1] + pos[2]*pos[2]);    // Naming scheme from ref [3]
    Real s = pos[0]/r;
    Real t = pos[1]/r;
    Real u = pos[2]/r; // sin(phi), phi = geocentric latitude

    // Calculate values for A -----------------------------------------
    // generate the off-diagonal elements
    A[1][0] = u*sqrt(Real(3.0));
    for (Integer n=1;  n<=NN+XS && n<=nn+XS;  ++n)
        A[n+1][n] = u*sqrt(Real(2*n+3))*A[n][n];

    // apply column-fill recursion formula (Table 2, Row I, Ref.[1])
    for (Integer m=0;  m<=MM+XS && m<=mm+XS;  ++m)
    {
        for (Integer n=m+2;  n<=NN+XS && n<=nn+XS;  ++n)
            A[n][m] = u * N1[n][m] * A[n-1][m] - N2[n][m] * A[n-2][m];
        // Ref.[3], Eq.(24)
        Re[m] = m==0 ? 1 : s*Re[m-1] - t*Im[m-1]; // real part of (s + i*t)^m
        Im[m] = m==0 ? 0 : s*Im[m-1] + t*Re[m-1]; // imaginary part of (s + i*t)^m
    }

    // Now do summation ------------------------------------------------
    // initialize recursion
    Real rho = FieldRadius/r;
    Real rho_np1 = -Factor/r * rho;   // rho(0) ,Ref[3], Eq 26 , factor = mu for gravity
    Real rho_np2 = rho_np1 * rho;
    Real a1 = 0;
    Real a2 = 0;
    Real a3 = 0;
    Real a4 = 0;
    Real a11 = 0;
    Real a12 = 0;
    Real a13 = 0;
    Real a14 = 0;
    Real a23 = 0;
    Real a24 = 0;
    Real a33 = 0;
    Real a34 = 0;
    Real a44 = 0;
    Real sqrt2 = sqrt (Real(2)); 
    for (Integer n=1;  n<=NN && n<=nn;  ++n)
    {
        rho_np1 *= rho;
        rho_np2 *= rho;
        Real sum1 = 0;
        Real sum2 = 0;
        Real sum3 = 0;
        Real sum4 = 0;
        Real sum11 = 0;
        Real sum12 = 0;
        Real sum13 = 0;
        Real sum14 = 0;
        Real sum23 = 0;
        Real sum24 = 0;
        Real sum33 = 0;
        Real sum34 = 0;
        Real sum44 = 0;

        for (Integer m=0;  m <= n && m<=MM && m<=mm;  ++m) // wcs - removed "m<=n"
        {
            Real Cval = getGravityField().getCnm (n,m);
            Real Sval = getGravityField().getSnm (n,m);
            // Pines Equation 27 (Part of)
            Real D =            (Cval*Re[m]   + Sval*Im[m]) * sqrt2;
            Real E = m==0 ? 0 : (Cval*Re[m-1] + Sval*Im[m-1]) * sqrt2;
            Real F = m==0 ? 0 : (Sval*Re[m-1] - Cval*Im[m-1]) * sqrt2;
            // Correct for normalization
            Real Avv00 = A[n][m];
            Real Avv01 = VR01[n][m] * A[n][m+1];
            Real Avv11 = VR11[n][m] * A[n+1][m+1];
            // Pines Equation 30 and 30b (Part of)
            sum1 += m * Avv00 * E;
            sum2 += m * Avv00 * F;
            sum3 +=     Avv01 * D;
            sum4 +=     Avv11 * D;

        }
        // Pines Equation 30 and 30b (Part of)
        Real rr = rho_np1/FieldRadius;
        a1 += rr*sum1;
        a2 += rr*sum2;
        a3 += rr*sum3;
        a4 -= rr*sum4;
    }

    // Pines Equation 31 
    acc[0] = a1+a4*s;
    acc[1] = a2+a4*t;
    acc[2] = a3+a4*u;
}   


AST_NAMESPACE_END
