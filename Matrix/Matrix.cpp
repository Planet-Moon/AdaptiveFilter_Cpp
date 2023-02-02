#include "Matrix.h"
#include <cassert>
#include <iostream>

std::vector<size_t> dimension(const Vec& v){
    return {1, v.size()};
}

std::vector<size_t> dimension(const Mat& m){
    for(int i=0; i<m.size(); i++){
        assert(m[i].size() == m[0].size());
    }
    return {m.size(), m[0].size()};
}

Vec operator*(const Mat &a, const Vec &x)
{
    int row,col;
    const int rows = a.size();
    const int cols = x.size();
    assert(a[0].size() == x.size());

    Vec prod(rows);

    #pragma omp parallel for
    for(row = 0; row < rows; row++){
        double temp = 0.;
        #pragma omp parallel for
        for(col = 0; col < cols; col++){
            temp += a[row][col]*x[col];
        }
        prod[row] = temp;
    }
    return prod;
}

Mat operator*(const Mat &a, const Mat &b)
{
    const int a_rows = a.size();
    const int a_cols = a[0].size();
    const int b_rows = b.size();
    const int b_cols = b[0].size();
    assert(a_cols == b_rows);

    Vec v_result(b_cols);
    Mat result(a_rows, v_result);
    #pragma omp parallel for
    for(int a_row = 0; a_row < a_rows; ++a_row){ // Row
        #pragma omp parallel for
        for(int b_col = 0; b_col < b_cols; ++b_col){ // Column
            #pragma omp parallel for
            for(int k = 0; k < a_cols; ++k){
                result[a_row][b_col] += a[a_row][k] * b[k][b_col];
            }
        }
    }
    return result;
}

Vec times(const Vec& v1, const Vec& v2){
    assert(v1.size() == v2.size());
    Vec result(v1.size());
    #pragma omp parallel for
    for(int i = 0; i < v1.size(); i++){
        result[i] = v1[i] * v2[i];
    }
    return result;
}

Vec rdivide(const Vec& v1, const Vec& v2){
    assert(v1.size() == v2.size());
    Vec result(v1.size());
    #pragma omp parallel for
    for(int i = 0; i < v1.size(); i++){
        result[i] = v1[i] / v2[i];
    }
    return result;
}

Vec ldivide(const Vec& v1, const Vec& v2){
    return rdivide(v2, v1);
}

Vec power(const Vec& v1, const Vec& v2){
    assert(v1.size() == v2.size());
    Vec result(v1.size());
    #pragma omp parallel for
    for(int i = 0; i < v1.size(); i++){
        result[i] = std::pow(v1[i], v2[i]);
    }
    return result;
}

Mat transpose(const Vec& v){
    Vec v_result(1);
    Mat result(v.size(), v_result);
    for(int col = 0; col < v.size(); col++){
        result[col][0] = v[col];
    }
    return result;
}

Mat operator+(const Mat &m1, const Mat &m2)
{
    const size_t m1_rows = m1.size();
    const size_t m1_cols = m1[0].size();
    const size_t m2_rows = m2.size();
    const size_t m2_cols = m2[0].size();
    assert(m1_rows == m2_rows && m1_cols == m2_cols);
    Vec v(m1_cols);
    Mat result(m1_rows, v);
    #pragma omp parallel for
    for(int row = 0; row < m1_rows; ++row){
        #pragma omp parallel for
        for(int col = 0; col < m1_cols; ++col){
            result[row][col] = m1[row][col] + m2[row][col];
        }
    }
    return result;
}

Mat operator-(const Mat &m1, const Mat &m2)
{
    const size_t m1_rows = m1.size();
    const size_t m1_cols = m1[0].size();
    const size_t m2_rows = m2.size();
    const size_t m2_cols = m2[0].size();
    Vec v(m1_cols);
    Mat result(m1_rows, v);
    #pragma omp parallel for
    for(int row = 0; row < m1_rows; ++row){
        #pragma omp parallel for
        for(int col = 0; col < m1_cols; ++col){
            result[row][col] = m1[row][col] - m2[row][col];
        }
    }
    return result;
}

Mat operator+(const Mat &m, const Vec& v)
{
    const size_t m_rows = m.size();
    const size_t m_cols = m[0].size();
    assert(m_cols == v.size());
    Vec v_result(m_cols);
    Mat result(m_rows, v_result);
    #pragma omp parallel for
    for(int row = 0; row < m_rows; ++row){
        #pragma omp parallel for
        for(int col = 0; col < m_cols; ++col){
            result[row][col] = m[row][col] + v[col];
        }
    }
    return result;
}

Mat operator-(const Mat &m, const Vec& v){
    return m + (v * -1);
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

Mat convertToMat(const Vec& v){
    return Mat(1, v);
}

Vec convertToVec(const Mat& m){
    assert(m.size() == 1);
    return m[0];
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
        const size_t m_rows = m.size();
        const size_t m_cols = m[0].size();
        Vec v(m_rows);
        Mat result(m_cols, v);

        #pragma omp parallel for
        for(int i = 0; i < m_rows; i++){
            #pragma omp parallel for
            for(int j = 0; j < m_cols; j++){
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

    Mat invert(const Mat& m){
        // needs at least rank of two for determinate
        Mat tmp = Matrix::square(m);
        if(!Matrix::hasFullRank(tmp)){
            return {};
        }

        Mat result = Matrix::identity(tmp.size());

        const size_t rows = tmp.size();
        const size_t cols = tmp[0].size();

        for(int col = 0; col < cols; ++col){
            const int step = col;

            const double f = 1/tmp[step][step];
            tmp[step] = tmp[step] * f;
            result[step] = result[step] * f;

            const double t = 1/tmp[step][step];
            #pragma omp parallel for
            for(int row = 0; row < rows; ++row){
                if(row == step) continue;
                const double v = tmp[row][step]*t;
                if(v == 0) continue;
                tmp[row] = tmp[row] - v * tmp[step];
                result[row] = result[row] - v * result[step];
            }
        }
        return result;
    }

    Mat square(const Mat& m){
        auto dim = dimension(m);
        size_t max_dim = std::max(dim[0], dim[1]);

        Vec v(max_dim);
        Mat result(max_dim, v);

        for(size_t row = 0; row < dim[0]; ++row){
            for(size_t col = 0; col < dim[1]; ++col){
                result[row][col] = m[row][col];
            }
        }
        return result;
    }

    std::pair<double, Mat> determinant_Mat(const std::pair<double, Mat>& in){
        Mat m = in.second;
        const auto dim = dimension(m);
        if(dim[0] == dim[1] && dim[0] == 2){
            return {m[0][0]*m[1][1]-m[0][1]*m[1][0], Matrix::zeros(0)};
        }
        double sum = 0.0;
        for(int row = 0; row < dim[0]; ++row){
            if(m[row][0] == 0) continue;
            Mat m_ = ignoreRowCol(m, row, 0);
            const auto det = determinant_Mat({0, m_});
            sum += std::pow(-1, row+2) * m[row][0] * det.first;
        }
        return {sum, Matrix::zeros(0)};
    }

    double determinant(const Mat& m){
        return determinant_Mat({0,m}).first;
    }

    Mat ignoreRowCol(const Mat& m, int row, int col){
        const auto dim = dimension(m);
        assert(dim[0] > 1 && dim[1] > 1);
        assert(row >= 0 && row < dim[0]);
        assert(col >= 0 && col < dim[1]);
        Mat result = m;
        result.erase(result.begin()+row);
        for(size_t i = 0; i < result[0].size(); ++i){
            result[i].erase(result[i].begin()+col);
        }
        return result;
    }

    bool hasFullRank(const Mat& m){
        return 0 != determinant(m);
    }

    double round_to(double value, double precision = 1.0)
    {
        return std::round(value / precision) * precision;
    }

    Mat round(const Mat &m, double precision){
        const int rows = m.size();
        const int cols = m[0].size();
        Mat result = m;
        #pragma omp parallel for
        for(int i=0; i < rows; i++){
            #pragma omp parallel for
            for(int j=0; j < cols; j++){
                result[i][j] = round_to(m[i][j], precision);
            }
        }
        return result;
    }
};

namespace Vector{
    Vec zeros(int dim){
        Vec v(dim);
        return v;
    }
};
