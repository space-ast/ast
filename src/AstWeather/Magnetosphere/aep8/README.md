The files in this directory were downloaded from this URL on 2021-03-25:
https://ccmc.gsfc.nasa.gov/pub/modelweb/radiation_belt/radbelt/fortran_code/

The following changes were made:

1. Renamed `trmfun.for` to `trmfun.f`.

参见：https://github.com/nasa/radbelt
参见：https://git.smce.nasa.gov/ccmc-share/modelwebarchive/-/tree/main/RADBELT/fortran_code?ref_type=heads

2. 使用[`f2c`工具](https://netlib.org/f2c)生成`trmfun.c`，命令行为`f2c -r8 ./trmfunc.f`

