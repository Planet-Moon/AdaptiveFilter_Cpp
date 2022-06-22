#include "Matrix.h"
#include <cassert>
#include <iostream>

Vec operator*(const Mat &a, const Vec &x)
{
    int i,j;
    int m = a.size();
    int n = x.size();
    assert(a[0].size() == x.size());

    Vec prod(m);

    for(i = 0; i < m; i++){
        prod[i] = 0.;
        for(j = 0; j < n; j++)
            prod[i] += a[i][j]*x[j];
    }
    return prod;
}

Mat operator*(const Mat &a, const Mat &b)
{
    const int n = a.size();
    const int m_a = a[0].size();
    const int m_b = b.size();
    const int j = b[0].size();
    assert(m_a == m_b);
    const int m = m_a;

    Vec col(j);
    Mat prod(n, col);

    for(int n_i = 0; n_i < n; ++n_i){ // Row
        for(int j_i = 0; j_i < j; ++j_i){ // Column
            for(int k = 0; k < m; ++k){
                const double toAdd = a[n_i][k] * b[k][j_i];
                prod[n_i][j_i] += toAdd;
            }
        }
    }
    return prod;
}

Mat operator+(const Mat &m1, const Mat &m2)
{
    assert(m1.size() == m2.size() && m1[0].size() == m2[0].size());
    Vec v(m1[0].size());
    Mat result(m1.size(), v);
    for(int i = 0; i < m1.size(); ++i){
        for(int j = 0; j < m1[i].size(); ++j){
            result[i][j] = m1[i][j] + m2[i][j];
        }
    }
    return result;
}

Mat operator-(const Mat &m1, const Mat &m2)
{
    assert(m1.size() == m2.size() && m1[0].size() == m2[0].size());
    Vec v(m1[0].size());
    Mat result(m1.size(), v);
    for(int i = 0; i < m1.size(); ++i){
        for(int j = 0; j < m1[i].size(); ++j){
            result[i][j] = m1[i][j] - m2[i][j];
        }
    }
    return result;
}

Vec operator+(const Vec &v1, const Vec &v2)
{
    assert(v1.size() == v2.size());
    Vec result(v1.size());
    for(int i = 0; i < v1.size(); i++){
        result[i] = v1[i] + v2[i];
    }
    return result;
}

Vec operator-(const Vec &v1, const Vec &v2)
{
    assert(v1.size() == v2.size());
    Vec result(v1.size());
    for(int i = 0; i < v1.size(); i++){
        result[i] = v1[i] - v2[i];
    }
    return result;
}

namespace Matrix{
    Mat identity(int dim){
        Mat m = zeros(dim);

        for(int i = 0; i < dim; i++){
            for(int j = 0; j < dim; j++){
                if(i==j){
                    m[i][j] = 1;
                }
            }
        }
        return m;
    }

    Mat zeros(int dim){
        Vec v(dim);
        Mat m(dim, v);
        return m;
    }

    Mat transpose(const Mat& m){
        Vec v(m.size());
        Mat result(m[0].size(), v);

        for(int i = 0; i < m.size(); i++){
            for(int j = 0; j < m[0].size(); j++){
                result[j][i] = m[i][j];
            }
        }
        return result;
    }

    Mat fromVector(const Vec& v){
        Mat result(1, v);
        return result;
    }
};

namespace Vector{
    Vec zeros(int dim){
        Vec v(dim);
        return v;
    }
};
