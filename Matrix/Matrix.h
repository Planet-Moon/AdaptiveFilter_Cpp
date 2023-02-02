#pragma once
#include <vector>
#include <type_traits>


using Vec = std::vector<double>;
using Mat = std::vector<Vec>;
using Mat3 = std::vector<Mat>;

std::vector<size_t> dimension(const Vec& v);
std::vector<size_t> dimension(const Mat& m);

Vec operator*(const Mat &a, const Vec &x);
Mat operator*(const Mat &a, const Mat &b);

template<typename T, typename = typename std::enable_if<std::is_arithmetic<T>::value, T>::type>
Vec operator*(const T &scalar, const Vec &v){
    Vec result = v;
    for(int i = 0; i < result.size(); ++i){
        result[i] *= scalar;
    }
    return result;
};

template<typename T, typename = typename std::enable_if<std::is_arithmetic<T>::value, T>::type>
Vec operator*(const Vec &v, const T &scalar){
    return scalar * v;
}

Vec times(const Vec& v1, const Vec& v2);
Vec rdivide(const Vec& v1, const Vec& v2);
Vec ldivide(const Vec& v1, const Vec& v2);
Vec power(const Vec& v1, const Vec& v2);
Mat transpose(const Vec& v1);

Vec operator+(const Vec &v1, const Vec &v2);
Vec operator-(const Vec &v1, const Vec &v2);

template<typename T, typename = typename std::enable_if<std::is_arithmetic<T>::value, T>::type>
Vec operator+(const Vec &v, const T &scalar){
    Vec result = v;
    for(auto& i: result){
        i += scalar;
    }
    return result;
}

template<typename T, typename = typename std::enable_if<std::is_arithmetic<T>::value, T>::type>
Vec operator+(const T &scalar, const Vec &v){
    return v+scalar;
}

Mat convertToMat(const Vec& v);
Vec convertToVec(const Mat& m);

template<typename T, typename = typename std::enable_if<std::is_arithmetic<T>::value, T>::type>
Mat operator*(const T &scalar, const Mat &a){
    Mat result = a;
    for(int i = 0; i < result.size(); ++i){
        for(int j = 0; j < result[0].size(); ++j){
            result[i][j] *= scalar;
        }
    }
    return result;
};

template<typename T, typename = typename std::enable_if<std::is_arithmetic<T>::value, T>::type>
Mat operator*(const Mat &a, const T &scalar){
    return scalar * a;
}

Mat operator+(const Mat &m1, const Mat &m2);
Mat operator-(const Mat &m1, const Mat &m2);

Mat operator+(const Mat &m, const Vec& v);
Mat operator-(const Mat &m, const Vec& v);

namespace Matrix{
    Mat identity(int dim);
    Mat zeros(int dim);
    Mat zeros(int dim1, int dim2);
    Mat3 zeros(int dim1, int dim2, int dim3);
    Mat transpose(const Mat& m);
    Mat fromVector(const Vec& v);
    Mat mean(const Mat& m, int dim);
    Mat mean(const Mat3& m, int dim);
    int size(const Mat& m, int dim);
    Mat invert(const Mat& m);
    Mat square(const Mat& m);
    double determinant(const Mat& m);
    Mat ignoreRowCol(const Mat& m, int row, int col);
    bool hasFullRank(const Mat& m);
    Mat round(const Mat& m, double precision = 1.0);
};

namespace Vector{
    Vec zeros(int dim);
}
