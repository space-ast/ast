#pragma once
#include "AstMacro.h"
#include <math.h>       // for sqrt
#include <assert.h>     // for assert
#include <type_traits>  // for std::is_pointer
#include <array>        // for std::array


AST_NAMESPACE_BEGIN

using std::size;  // use function size in stdlib

// dot

template<typename Vector3D1, typename Vector3D2>
inline double dot(const Vector3D1& arr1, const Vector3D2& arr2)
{
    assert (size(arr1) == size(arr2));
    double retval = 0;
    size_t s = size(arr2);
    for (size_t i = 0; i < s; i++)
    {
        retval += arr1[i] * arr2[i];
    }
    return retval;
}


template<size_t N1, size_t N2>
inline double dot(const double(&arr1)[N1], const double(&arr2)[N2])
{
    static_assert(N1 == N2, "dot product requires two arrays of the same size");
    double retval = 0;
    for (size_t i = 0; i < N1; i++)
    {
        retval += arr1[i] * arr2[i];
    }
    return retval;
}


inline double dot(const double* arr1, const double* arr2, size_t N)
{
    double retval = 0;
    for (size_t i = 0; i < N; i++)
    {
        retval += arr1[i] * arr2[i];
    }
    return retval;
}

inline double dot3(const double* arr1, const double* arr2)
{
    return dot(arr1, arr2, 3);
}


// cross

template<typename Vector3D1, typename Vector3D2>
inline auto cross(const Vector3D1& arr1, const Vector3D2& arr2)
    -> std::enable_if_t<!std::is_pointer<Vector3D1>::value && !std::is_pointer<Vector3D2>::value, Vector3D1>
{
    assert (size(arr1) >= 3 && size(arr2) >= 3);

    return Vector3D1{
        arr1[1] * arr2[2] - arr1[2] * arr2[1],
        arr1[2] * arr2[0] - arr1[0] * arr2[2],
        arr1[0] * arr2[1] - arr1[1] * arr2[0]
    };
}

inline void cross(double* res, const double* arr1, const double* arr2)
{
    res[0] = arr1[1] * arr2[2] - arr1[2] * arr2[1];
    res[1] = arr1[2] * arr2[0] - arr1[0] * arr2[2];
    res[2] = arr1[0] * arr2[1] - arr1[1] * arr2[0];
}




template<size_t N1, size_t N2>
inline std::array<double, 3> cross(const double (&arr1)[N1], const double (&arr2)[N2])
{
    static_assert(N1 >= 3, "cross product requires arrays of at least size 3");
    static_assert(N2 >= 3, "cross product requires arrays of at least size 3");
    
    return std::array<double, 3>{
        arr1[1] * arr2[2] - arr1[2] * arr2[1],
        arr1[2] * arr2[0] - arr1[0] * arr2[2],
        arr1[0] * arr2[1] - arr1[1] * arr2[0]
    };
}


inline std::array<double, 3> cross3(const double* arr1, const double* arr2)
{
    return std::array<double, 3>{
        arr1[1] * arr2[2] - arr1[2] * arr2[1],
            arr1[2] * arr2[0] - arr1[0] * arr2[2],
            arr1[0] * arr2[1] - arr1[1] * arr2[0]
    };
}


// norm

inline double norm(double* arr, size_t N)
{
    double sum = 0;
    for (size_t i = 0; i < N; i++)
    {
        sum += arr[i] * arr[i];
    }
    return sqrt(sum);
}


template<typename Container>
inline auto norm(const Container& vec) 
    -> std::enable_if_t<!std::is_pointer<Container>::value, double>
{
    double sum = 0.0;
    for (const auto& val : vec) {
        sum += val * val;
    }
    return sqrt(sum);
}



template <size_t N>
inline double norm(const double (&arr)[N])
{
    double sum = 0;
    for (size_t i = 0; i < N; i++)
    {
        sum += arr[i] * arr[i];
    }
    return sqrt(sum);
}


// normalize


inline void normalize(double* arr, size_t N)
{
    double mag = norm(arr, N);
    if (mag == 0)
        return;
    for (size_t i = 0; i < N; i++)
    {
        arr[i] /= mag;
    }
}


template<typename Container>
inline auto normalize(const Container& vec)
-> std::enable_if_t<!std::is_pointer<Container>::value, void>
{
    double mag = norm(vec);
    if (mag == 0)
        return;
    for (auto& val : vec) {
        val /= mag;
    }
}



template <size_t N>
inline void normalize(double (&arr)[N])
{
    double mag = norm(arr);
    if (mag == 0)
        return;
    for (size_t i = 0; i < N; i++)
    {
        arr[i] /= mag;
    }
}


AST_NAMESPACE_END