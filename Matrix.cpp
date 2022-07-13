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

    #pragma omp parallel for
    for(i = 0; i < m; i++){
        prod[i] = 0.;
        #pragma omp parallel for
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
    #pragma omp parallel for
    for(int n_i = 0; n_i < n; ++n_i){ // Row
        #pragma omp parallel for
        for(int j_i = 0; j_i < j; ++j_i){ // Column
            #pragma omp parallel for
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
    #pragma omp parallel for
    for(int i = 0; i < m1.size(); ++i){
        #pragma omp parallel for
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
    #pragma omp parallel for
    for(int i = 0; i < m1.size(); ++i){
        #pragma omp parallel for
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
    #pragma omp parallel for
    for(int i = 0; i < v1.size(); i++){
        result[i] = v1[i] + v2[i];
    }
    return result;
}

Vec operator-(const Vec &v1, const Vec &v2)
{
    assert(v1.size() == v2.size());
    Vec result(v1.size());
    #pragma omp parallel for
    for(int i = 0; i < v1.size(); i++){
        result[i] = v1[i] - v2[i];
    }
    return result;
}

namespace Matrix{
    Mat identity(int dim){
        Mat m = zeros(dim);

        #pragma omp parallel for
        for(int i = 0; i < dim; i++){
            #pragma omp parallel for
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

    Mat zeros(int dim1, int dim2)
    {
        Vec v(dim2);
        Mat m(dim1, v);
        return m;
    }

    Mat3 zeros(int dim1, int dim2, int dim3)
    {
        Vec v(dim3);
        Mat m(dim2, v);
        Mat3 m3(dim1, m);
        return m3;
    }

    Mat transpose(const Mat& m){
        Vec v(m.size());
        Mat result(m[0].size(), v);

        #pragma omp parallel for
        for(int i = 0; i < m.size(); i++){
            #pragma omp parallel for
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

    Mat mean(const Mat& m, int dim){
        const int rows = m.size();
        const int cols = m[0].size();
        if(dim == 1){
            Vec v(cols);
            Mat result(1,v);
            #pragma omp parallel for
            for(int i = 0; i < cols; ++i){
                #pragma omp parallel for
                for(int j = 0; j < rows; ++j){
                    result[0][i] += m[j][i]/rows;
                }
            }
            return result;
        }
        else if(dim == 2){
            Vec v(1);
            Mat result(rows,v);
            #pragma omp parallel for
            for(int i = 0; i < rows; ++i){
                #pragma omp parallel for
                for(int j = 0; j < cols; ++j){
                    result[i][0] += m[i][j]/cols;
                }
            }
            return result;
        }
        return{};
    }

    Mat mean(const Mat3& m, int dim){
        const int pages = m.size();
        const int rows = m[0].size();
        const int cols = m[0][0].size();

        if(dim == 1){
            Mat result = Matrix::zeros(cols, pages);
            for(int i = 0; i < cols; ++i){
                for(int p = 0; p < pages; ++p){
                    for(int j = 0; j < rows; ++j){
                        result[i][p] += m[p][j][i]/rows;
                    }
                }
            }
            return result;
        }
        else if(dim == 2){
            Mat result = Matrix::zeros(rows, pages);
            for(int p = 0; p < pages; ++p){
                for(int i = 0; i < rows; ++i){
                    for(int j = 0; j < cols; ++j){
                        result[i][p] += m[p][i][j]/cols;
                    }
                }
            }
            return result;
        }
        else if(dim == 3){
            Mat result = Matrix::zeros(rows, cols);
            for(int i = 0; i < rows; ++i){
                for(int j = 0; j < cols; ++j){
                    for(int p = 0; p < pages; ++p){
                        result[i][j] += m[p][i][j]/pages;
                    }
                }
            }
            return result;
        }
        return{};
    }

    int size(const Mat& m, int dim){
        if(dim == 0){
            return m.size();
        }
        else if(dim == 1){
            return m[0].size();
        }
        return 0;
    }
};

namespace Vector{
    Vec zeros(int dim){
        Vec v(dim);
        return v;
    }
};
