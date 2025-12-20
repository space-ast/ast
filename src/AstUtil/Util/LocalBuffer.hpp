#pragma once
#ifdef __cplusplus 
#   include <memory>           // for std::unique_ptr
#   include <algorithm>        // for std::fill_n


#if __cplusplus >= 201402L

#define A_LOCAL_BUFFER(T, buf, size)\
  auto __a_local_buffer_ ## buf = std::make_unique<T []> (size);\
  T *buf = __a_local_buffer_ ## buf.get ();            

#else

#define A_LOCAL_BUFFER(T, buf, size)\
  std::unique_ptr<T []> __a_local_buffer_ ## buf { new T [size] };\
  T *buf = __a_local_buffer_ ## buf.get ();

#endif

#define A_LOCAL_BUFFER_INIT(T, buf, size, value)\
  A_LOCAL_BUFFER (T, buf, size);\
  std::fill_n (buf, size, value);


#endif
