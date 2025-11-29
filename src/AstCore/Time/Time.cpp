#include "Time.hpp"
#include <math.h>       // for floor

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


AST_NAMESPACE_END
