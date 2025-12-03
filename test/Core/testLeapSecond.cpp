#include "AstCore/LeapSecond.hpp"
#include "AstTest/AstTestMacro.h"
#include "AstCore/DateTime.hpp"

AST_USING_NAMESPACE

TEST(LeapSecond, LoadDefault)
{
    LeapSecond leapSecond;
    err_t err = leapSecond.loadDefault();
    EXPECT_EQ(err, eNoError);
}

TEST(LeapSecond, normalize)
{
    DateTime dateTime{ 2023, 12, 31, 23, 59, 60 };
    dateTime.normalize();

    EXPECT_EQ(dateTime.year(), 2024);
    EXPECT_EQ(dateTime.month(), 1);
    EXPECT_EQ(dateTime.day(), 1);
    EXPECT_EQ(dateTime.hour(), 0);
    EXPECT_EQ(dateTime.minute(), 0);
    EXPECT_EQ(dateTime.second(), 0);
}

TEST(LeapSecond, normalizeUTC)
{
    {
        DateTime dateTime{ 2015, 6, 30, 23, 59, 60 };
        dateTime.normalizeUTC();

        EXPECT_EQ(dateTime.year(), 2015);
        EXPECT_EQ(dateTime.month(), 6);
        EXPECT_EQ(dateTime.day(), 30);
        EXPECT_EQ(dateTime.hour(), 23);
        EXPECT_EQ(dateTime.minute(), 59);
        EXPECT_EQ(dateTime.second(), 60);
    }
    {
        DateTime dateTime{ 2015, 6, 30, 23, 60, 0 };
        dateTime.normalizeUTC();

        EXPECT_EQ(dateTime.year(), 2015);
        EXPECT_EQ(dateTime.month(), 6);
        EXPECT_EQ(dateTime.day(), 30);
        EXPECT_EQ(dateTime.hour(), 23);
        EXPECT_EQ(dateTime.minute(), 59);
        EXPECT_EQ(dateTime.second(), 60);
    }
    {
        DateTime dateTime{ 2015, 6, 30, 23, 59, 62 };
        dateTime.normalizeUTC();

        EXPECT_EQ(dateTime.year(), 2015);
        EXPECT_EQ(dateTime.month(), 7);
        EXPECT_EQ(dateTime.day(), 1);
        EXPECT_EQ(dateTime.hour(), 0);
        EXPECT_EQ(dateTime.minute(), 0);
        EXPECT_EQ(dateTime.second(), 1);
    }
    {
        DateTime dateTime{ 2015, 6, 30, 23, 59, 60.12 };
        dateTime.normalizeUTC();

        EXPECT_EQ(dateTime.year(), 2015);
        EXPECT_EQ(dateTime.month(), 6);
        EXPECT_EQ(dateTime.day(), 30);
        EXPECT_EQ(dateTime.hour(), 23);
        EXPECT_EQ(dateTime.minute(), 59);
        EXPECT_EQ(dateTime.second(), 60.12);

    }
}

TEST(LeapSecond, normalizeLocal)
{
    {
        DateTime dateTime{ 2015, 7, 1, 7, 59, 60 };
        dateTime.normalizeLocal(8);

        EXPECT_EQ(dateTime.year(), 2015);
        EXPECT_EQ(dateTime.month(), 7);
        EXPECT_EQ(dateTime.day(), 1);
        EXPECT_EQ(dateTime.hour(), 7);
        EXPECT_EQ(dateTime.minute(), 59);
        EXPECT_EQ(dateTime.second(), 60);
    }
    {
        DateTime dateTime{ 2015, 7, 1, 7, 60, 0 };
        dateTime.normalizeLocal(8);

        EXPECT_EQ(dateTime.year(), 2015);
        EXPECT_EQ(dateTime.month(), 7);
        EXPECT_EQ(dateTime.day(), 1);
        EXPECT_EQ(dateTime.hour(), 7);
        EXPECT_EQ(dateTime.minute(), 59);
        EXPECT_EQ(dateTime.second(), 60);
    }
}


GTEST_MAIN()

