#include "AstUtil/Logger.hpp"



int main()
{
    aWarning() << "message";
    aWarning("number: %d", 1);
    return 0;
}

