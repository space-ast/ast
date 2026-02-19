The files in this directory were downloaded from this URL on 2021-03-25:
https://ccmc.gsfc.nasa.gov/pub/modelweb/geomagnetic/igrf/fortran_code/

The following changes were made:

1. Renamed `shellig.for` to `shellig.f`.
2. In `shellig.f`, update the array of filenames to match their names on disk
   (e.g., change `dgrf95.dat` to `dgrf1995.dat`). Update dimensions of filename
   variables in the `COMMON/MODEL/` section to match.
3. Update the list of filenames to include dates through the 2020s.


4. 使用[`f2c`工具](https://netlib.org/f2c)生成`shellig.c`，命令行为`f2c -r8 ./shellig.f`


参见：
- https://git.smce.nasa.gov/ccmc-share/modelwebarchive/-/blob/main/IGRF/fortran_code
- https://github.com/nasa/radbelt
