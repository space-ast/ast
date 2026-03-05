#ifndef AST_NO_CSPICE
#include "SpiceUsr.h"
#include <cstdio>

int main()
{
    furnsh_c("data/Test/kernels/pck/pck00011.tpc");
    double mtx_c[3][3];
    pxform_c("J2000", "IAU_MARS", 0, mtx_c);
    for(int i = 0; i < 3; i++)
    {
        for(int j = 0; j < 3; j++)
        {
            printf("%12.6f ", mtx_c[i][j]);
        }
        printf("\n");
    }
}
#else
int main(){}
#endif


