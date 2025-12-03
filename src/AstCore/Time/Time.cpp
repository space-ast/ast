#include "Time.hpp"
#include <math.h>       // for floor
#include <algorithm>	// for std::min

AST_NAMESPACE_BEGIN

void aTimeNormalize(Time &time)
{
	int rmin = (int)floor(time.second() / 60);
	time.second() -= rmin * 60;
	time.minute() += rmin;

	int rhour = (int)floor(time.minute() / 60.);
	time.minute() -= rhour * 60;
	time.hour() += rhour;
}

void aTimeFromTotalSecond(double totalSecond, Time &time)
{
	using std::min;

	time.hour() = min((int)floor(totalSecond / 3600.), 23);
	totalSecond -= time.hour() * 3600;
	time.minute() = min((int)floor(totalSecond / 60.), 59);
	time.second() = totalSecond - time.minute() * 60;
}

AST_NAMESPACE_END
