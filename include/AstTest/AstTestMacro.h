#define ASSERT_EQ(val1, val2)   \
if((val1) != (val2))            \
return -1;                      \


#define ASSERT_NEAR(val1, val2, eps)    \
if( fabs((val1) - (val2)) > fabs(eps))  \
return -1;                              \


