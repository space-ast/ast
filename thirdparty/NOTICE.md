# thirdparty libraries notice

ast project use the following thirdparty libraries:

click the link to view the library's homepage and license.

- [cminpack](https://github.com/devernay/cminpack.git)
- [libf2c](https://netlib.org/f2c/)
- [Qt5](https://www.qt.io/)
- [*snopt](http://www.sbsi-sol-optimize.com/asp/sol_product_snopt.htm)
- [*matplotplusplus](https://alandefreitas.github.io/matplotplusplus/)


*snopt: ast project only provide the interface to use it, 
and the snopt library itself is not included in this project.
the snopt interface(file snopt_ast.h) is from [snopt_PAGMO.h in pagmo project](https://github.com/esa/pagmo/blob/master/src/algorithm/snopt_cpp_wrapper/snopt_PAGMO.h).
you must obtain the snopt library and it's license by yourself.

*matplotplusplus: ast project fork and modify orginal matplotplusplus project to support c++11 and xmake.

