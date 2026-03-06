#ifndef AST_NO_CSPICE
#include "SpiceUsr.h"


int main()
{
    furnsh_c("data/Test/kernels/pck/pck00011.tpc");
    double tipm[3][3];
    enum {saturn = 699};
    tipbod_c("J2000", saturn, 0, tipm);
}
#else
int main(){}
#endif