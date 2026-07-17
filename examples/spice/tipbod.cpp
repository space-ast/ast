#ifdef AST_WITH_CSPICE
#include "SpiceUsr.h"
#include "ast/TestConfig.hpp"

AST_USING_NAMESPACE

int main()
{
    furnsh_c(aTestGetConfigValue("PCK_FILE").toString().c_str() );
    double tipm[3][3];
    enum {saturn = 699};
    tipbod_c("J2000", saturn, 0, tipm);
}
#else
int main(){}
#endif