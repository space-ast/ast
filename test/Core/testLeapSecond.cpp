#include "AstCore/LeapSecond.hpp"
#include "AstTest/AstTestMacro.h"

AST_USING_NAMESPACE

TEST(LeapSecond, LoadDefault)
{
    LeapSecond leapSecond;
    err_t err = leapSecond.loadDefault();
    EXPECT_EQ(err, eNoError);
}


GTEST_MAIN()

