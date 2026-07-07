# nrlmsise-00
C source code for the NRLMSISE-00 empirical atmosphere model (Dominik Brodowski 2019-07-09)

代码来自：
https://git.linta.de/~brodo/nrlmsise-00.git/tree/
https://github.com/magnific0/NRLMSISE-00

nrlmsise-00-class.cpp 是在原代码的基础上进行的类封装，
用于支持在多线程下，每个线程使用单个类实例进行计算，
避免了原代码存在的全局变量冲突

