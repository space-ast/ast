#include "AstGlobal.h"
#include "AstUtil/StringView.hpp"
#include <string>
#if _HAS_CXX17
#include <string_view>
#endif

AST_USING_NAMESPACE
int main()
{
	using namespace std;

	{
		StringView sv("124\0az123");
		std::string s("124\0az123");

		std::string s2 (sv);
		StringView sv2 = s;

		nothing();
	}

#if _HAS_CXX17
	{
		std::string_view sv("124\0az123");
		std::string s("124\0az123");

		std::string s2 (sv);
		std::string_view sv2 = s;

		nothing();
	}
#endif
}