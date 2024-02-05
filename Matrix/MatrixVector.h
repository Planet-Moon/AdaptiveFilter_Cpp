#pragma once
#include <algorithm>
#include <array>
#include <cstddef>
#include <cmath>
#include <type_traits>
#include <cassert>
#include <iostream>


namespace MatrixVector {

    template<std::size_t N>
    struct Vec{
        std::array<double,N> d;

        static constexpr std::size_t size() {
            return N;
        }

        double operator[](std::size_t i) const {
            assert(i < N);
            return d[i];
        }

        double& operator[](std::size_t i) {
            assert(i < N);
            return d[i];
        }

        typename std::array<double,N>::iterator begin() {
            return d.begin();
        }

        typename std::array<double,N>::iterator end() {
            return d.end();
        }

        typename std::array<double,N>::reverse_iterator rbegin() {
            return d.rbegin();
        }

        typename std::array<double,N>::reverse_iterator rend() {
            return d.rend();
        }


        Vec() = default;
        Vec(const std::array<double, N>& array) {
            d = array;
        }
        virtual ~Vec() = default;


        template <typename... Args>
        constexpr Vec(Args... args) : d{static_cast<double>(args)...} {
            static_assert(sizeof...(args) == N, "Initializer list size must be equal to N");
        }

    private:
        size_t _first = 0;
        size_t _last = N;
        double* _firstPtr = N > 0 ? &(d[0]) : nullptr;
    };

    template<std::size_t Z, std::size_t S>
    struct Mat{
        std::array<Vec<S>, Z> d;

        static std::size_t size() {
            return Z;
        }

        Vec<S> operator[](std::size_t i) const {
            assert(i < Z);
            return d[i];
        }

        Vec<S>& operator[](std::size_t i) {
            assert(i < Z);
            return d[i];
        }

        typename std::array<Vec<S>,Z>::iterator begin() {
            return d.begin();
        }

        typename std::array<Vec<S>,Z>::iterator end() {
            return d.end();
        }

    private:
        size_t _rowSize = Z;
        size_t _colSize = S;
        Vec<S>* _firstPtr = Z > 0 && S > 0 ? &(d[0]) : nullptr;
    };

    template<std::size_t N>
    Vec<N> vec_from_array(const std::array<double, N>& array){
        Vec<N> v;
        v.d = array;
        return v;
    }

    template<std::size_t Z, std::size_t S>
    Mat<Z,S> mat_from_array(const std::array<std::array<double,S>,Z>& array){
        Mat<Z,S> m;
        std::transform(array.begin(), array.end(), m.begin(), [](const std::array<double,S>& v){
            return vec_from_array(v);
        });
        return m;
    }

    template<std::size_t dim1, std::size_t dim2, std::size_t dim3>
    struct Mat3{
        std::array<Mat<dim2, dim3>, dim1> d;
        Mat<dim2, dim3> operator[](std::size_t i) const {
            return d[i];
        }

        Mat<dim2, dim3>& operator[](std::size_t i) {
            return d[i];
        }

        auto begin() {
            return d.begin();
        }

        auto end() {
            return d.end();
        }

        double get_element(std::size_t page, std::size_t row, std::size_t column) const {
            return d[page][row][column];
        }

        void set_element(std::size_t page, std::size_t row, std::size_t column, double value) {
            d[page][row][column] = value;
        }
    private:
        size_t _pageSize = dim1;
        size_t _rowSize = dim2;
        size_t _colSize = dim3;
        Mat<dim2, dim3>* _firstPtr = dim1 > 0 && dim2 > 0 && dim3 > 0 ? &(d[0]) : nullptr;
    };

    template<std::size_t N>
    void init_vector(Vec<N>& v, double value = 0.0){
        for(std::size_t i = 0; i < N; ++i){
            v[i] = value;
        }
    }

    template<std::size_t N>
    Vec<N> create_vector(double value = 0.0){
        Vec<N> v;
        init_vector(v,value);
        return v;
    }

    template<std::size_t Z, std::size_t S>
    void init_matrix(Mat<Z,S>& mat, double value = 0.0){
        for(std::size_t i = 0; i < Z; ++i){
            init_vector(mat[i], value);
        }
    }

    template<std::size_t Z, std::size_t S>
    Mat<Z,S> create_matrix(double value = 0.0){
        Mat<Z,S> result = {};
        init_matrix(result, value);
        return result;
    }

    template<std::size_t pages, std::size_t rows, std::size_t cols>
    void init_matrix3(Mat3<pages, rows, cols>& m3, double value = 0.0){
        for(std::size_t i = 0; i < pages; ++i){
            init_matrix(m3[i], value);
        }
    }

    template<std::size_t Z, std::size_t S>
    auto rowErase(const Mat<Z,S>& m, std::size_t row_idx) {
        assert(row_idx < Z);
        auto result = create_matrix<Z-1,S>();
        std::size_t result_idx = 0;
        for(std::size_t i = 0; i < Z; ++i){
            if(i == row_idx){
                continue;
            }
            result[result_idx] = m[i];
            ++result_idx;
        }
        return result;
    }

    template<std::size_t Z, std::size_t S>
    auto colErase(const Mat<Z,S>& m, std::size_t col_idx) {
        assert(col_idx < S);
        auto result = create_matrix<Z,S-1>();
        for(std::size_t j = 0; j<Z; ++j){
            std::size_t outer_idx = 0;
            for(std::size_t i = 0; i < S; ++i){
                if(i == col_idx){
                    continue;
                }
                result[j][outer_idx] = m[j][i];
                ++outer_idx;
            }
        }
        return result;
    }

    template<std::size_t Z, std::size_t S>
    auto rowColErase(const Mat<Z,S>& m, std::size_t row_idx, std::size_t col_idx){
        auto m1 = rowErase(m, row_idx);
        auto m2 = colErase(m1, col_idx);
        return m2;
    }

    template<std::size_t Z, std::size_t S>
    auto operator+(const Mat<Z,S>& m1, const Mat<Z,S>& m2){
        Mat<Z,S> m = create_matrix<Z, S>();
        for(std::size_t i = 0; i < Z; ++i){
            m[i] = m1[i] + m2[i];
        }
        return m;
    }

    template<std::size_t Z, std::size_t S>
    auto operator-(const Mat<Z,S>& m1, const Mat<Z,S>& m2){
        Mat<Z,S> m = create_matrix<Z, S>();
        for(std::size_t i = 0; i < Z; ++i){
            m[i] = m1[i] - m2[i];
        }
        return m;
    }

    template<std::size_t Z, std::size_t S>
    Vec<Z> operator*(const Mat<Z,S> &a, const Vec<S> &x)
    {
        Vec<Z> prod;

        #pragma omp parallel for
        for(int row = 0; row < Z; row++){
            double temp = 0.;
            #pragma omp parallel for
            for(int col = 0; col < S; col++){
                temp += a[row][col]*x[col];
            }
            prod[row] = temp;
        }
        return prod;
    }

    template<std::size_t Z, std::size_t S, std::size_t S2>
    Mat<Z,S2> operator*(const Mat<Z,S> &a, const Mat<S,S2> &b){
        Mat<Z,S2> result;
        init_matrix(result, 0.0);
        #pragma omp parallel for
        for(int a_row = 0; a_row < Z; ++a_row){ // Row
            #pragma omp parallel for
            for(int b_col = 0; b_col < S2; ++b_col){ // Column
                #pragma omp parallel for
                for(int k = 0; k < S; ++k){
                    result[a_row][b_col] += a[a_row][k] * b[k][b_col];
                }
            }
        }
        return result;
    }

    template<typename T, typename = typename std::enable_if<std::is_arithmetic<T>::value, T>::type, std::size_t N>
    Vec<N> operator*(const T &scalar, const Vec<N> &v){
        Vec<N> result = v;
        for(int i = 0; i < N; ++i){
            result[i] *= scalar;
        }
        return result;
    };

    template<typename T, typename = typename std::enable_if<std::is_arithmetic<T>::value, T>::type, std::size_t N>
    Vec<N> operator*(const Vec<N> &v, const T &scalar){
        return scalar * v;
    }

    template<typename T, typename = typename std::enable_if<std::is_arithmetic<T>::value, T>::type, std::size_t Z, std::size_t S>
    Mat<Z,S> operator*(const T &scalar, const Mat<Z,S> &m){
        auto result = m;
        for(int i = 0; i < S; ++i){
            result[i] = result[i] * scalar;
        }
        return result;
    };

    template<typename T, typename = typename std::enable_if<std::is_arithmetic<T>::value, T>::type, std::size_t Z, std::size_t S>
    Mat<Z,S> operator*(const Mat<Z,S> &m, const T &scalar){
        return scalar * m;
    }

    template<std::size_t N>
    Vec<N> times(const Vec<N>& v1, const Vec<N>& v2){
        Vec<N> result = create_vector<N>();
        #pragma omp parallel for
        for(int i = 0; i < N; i++){
            result[i] = v1[i] * v2[i];
        }
        return result;
    }

    template<std::size_t N>
    Vec<N> rdivide(const Vec<N>& v1, const Vec<N>& v2){
        Vec<N> result = create_vector<N>();
        #pragma omp parallel for
        for(int i = 0; i < N; i++){
            result[i] = v1[i] / v2[i];
        }
        return result;
    }

    template<std::size_t N>
    Vec<N> ldivide(const Vec<N>& v1, const Vec<N>& v2){
        return rdivide(v2, v1);
    }

    template<std::size_t N>
    Vec<N> power(const Vec<N>& v1, const Vec<N>& v2){
        Vec<N> result = create_vector<N>();
        #pragma omp parallel for
        for(int i = 0; i < N; i++){
            result[i] = std::pow(v1[i], v2[i]);
        }
        return result;
    }

    template<std::size_t N>
    Mat<N,1> transpose(const Vec<N>& v){
        Mat<N,1> result = create_matrix<N,1>();
        #pragma omp parallel for
        for(int i = 0; i < N; i++){
            result[i][0] = v[i];
        }
        return result;
    }

    template<std::size_t Z, std::size_t S>
    Vec<Z> get_column(const Mat<Z,S>& m, std::size_t c) {
        static_assert(c > 0 && c < S);
        Vec<Z> v = {};
        init_vector(v, 0.0);
        for(int i = 0; i < Z; ++i){
            v[i] = m[i][c];
        }
        return v;
    }

    template<std::size_t Pages, std::size_t Rows, std::size_t Columns>
    bool get_row(const Mat3<Pages, Rows, Columns>& m, std::size_t row, Mat<Pages, Columns>& out) {
        Mat<Pages, Columns> result;
        for(std::size_t page = 0; page < Pages; ++page){
            for(std::size_t column = 0; column < Columns; ++column){
                result[page][column] = m.get_element(page, row, column);
            }
        }
        return true;
    }

    template<std::size_t Pages, std::size_t Rows, std::size_t Columns>
    bool get_column(const Mat3<Pages, Rows, Columns>& m, std::size_t column, Mat<Pages, Rows>& out) {
        Mat<Pages, Rows> result;
        for(std::size_t page = 0; page < Pages; ++page){
            for(std::size_t row = 0; row < Rows; ++row){
                result[page][row] = m[page][row][column];
            }
        }
        return result;
    }

    template<std::size_t N>
    Vec<N> operator+(const Vec<N>& v1, const Vec<N>& v2){
        auto result = create_vector<N>();
        for(int i = 0; i < N; ++i){
            result[i] = v1[i] + v2[i];
        }
        return result;
    }

    template<std::size_t N>
    Vec<N> operator-(const Vec<N>& v1, const Vec<N>& v2){
        auto result = create_vector<N>();
        for(int i = 0; i < N; ++i){
            result[i] = v1[i] - v2[i];
        }
        return result;
    }

    template<typename T, typename = typename std::enable_if<std::is_arithmetic<T>::value, T>::type, std::size_t N>
    Vec<N> operator+(const Vec<N> &v, const T &scalar){
        Vec<N> result = v;
        for(auto& i: result){
            i += scalar;
        }
        return result;
    }

    template<typename T, typename = typename std::enable_if<std::is_arithmetic<T>::value, T>::type, std::size_t N>
    Vec<N> operator+(const T &scalar, const Vec<N> &v){
        return v+scalar;
    }

    template<std::size_t N>
    Mat<1,N> convertToMat(const Vec<N>& v){
        Mat<1,N> m = create_matrix<1, N>();
        m[0] = v;
        return m;
    }

    template<std::size_t N>
    Vec<N> convertToVec(const Mat<1,N>& m){
        return m[0];
    }

    namespace Matrix{
        template<std::size_t N>
        Mat<N,N> identity(){
            Mat<N,N> m = create_matrix<N, N>();
            #pragma omp parallel for
            for(int i = 0; i < N; i++){
                #pragma omp parallel for
                for(int j = 0; j < N; j++){
                    if(i==j){
                        m[i][j] = 1;
                    }
                }
            }
            return m;
        }

        template<std::size_t N>
        Mat<N,N> zeros(){
            return create_matrix<N, N>();
        }

        template<std::size_t Z, std::size_t S>
        Mat<Z,S> zeros(){
            return create_matrix<Z, S>();
        }

        template<std::size_t dim1, std::size_t dim2, std::size_t dim3>
        Mat3<dim1, dim2, dim3> zeros(){
            Mat3<dim1, dim2, dim3> result;
            Mat<dim2, dim3> m = create_matrix<dim2, dim3>();
            for(size_t i = 0; i < dim1; ++i){
                result[i] = m;
            }
            return result;
        }

        template<std::size_t Z, std::size_t S>
        bool transpose(const Mat<Z,S>& m, Mat<S,Z>& m_out) {
            #pragma omp parallel for
            for(int i = 0; i < Z; i++){
                #pragma omp parallel for
                for(int j = 0; j < S; j++){
                    m_out[j][i] = m[i][j];
                }
            }
            return true;
        }

        template<std::size_t N>
        Mat<1,N> fromVector(const Vec<N>& v){
            Mat<1,N> result;
            result[0] = v;
            return result;
        }

        enum MeanDim {
            Rows,
            Cols
        };

        template<MeanDim dim, std::size_t Z, std::size_t S>
        auto mean(const Mat<Z,S>& m) {
            if constexpr(dim == MeanDim::Rows){
                auto result = create_matrix<1, S>();
                #pragma omp parallel for
                for(int i = 0; i < S; ++i){
                    #pragma omp parallel for
                    for(int j = 0; j < Z; ++j){
                        result[0][i] += m[j][i]/Z;
                    }
                }
                return result;
            }
            else if constexpr(dim == MeanDim::Cols){
                Mat<Z,1> result = create_matrix<Z, 1>();
                #pragma omp parallel for
                for(int i = 0; i < Z; ++i){
                    #pragma omp parallel for
                    for(int j = 0; j < S; ++j){
                        result[i][0] += m[i][j]/S;
                    }
                }
                return result;
            }
            else{
                return;
            }
        }

        enum MeanDim3{
            dim3Rows,
            dim3Cols,
            dim3Pages
        };

        template<MeanDim3 meanDim, std::size_t pages, std::size_t rows, std::size_t cols>
        auto mean(const Mat3<pages, rows, cols>& m) {
            if constexpr(meanDim == MeanDim3::dim3Rows){
                Mat<cols,pages> result = create_matrix<cols,pages>();
                for(int i = 0; i < cols; ++i){
                    for(int p = 0; p < pages; ++p){
                        for(int j = 0; j < rows; ++j){
                            result[i][p] += m[p][j][i]/rows;
                        }
                    }
                }
                return result;
            }
            else if constexpr(meanDim == MeanDim3::dim3Cols){
                Mat<rows, pages> result = create_matrix<rows, pages>();
                for(int p = 0; p < pages; ++p){
                    for(int i = 0; i < rows; ++i){
                        for(int j = 0; j < cols; ++j){
                            result[i][p] += m[p][i][j]/cols;
                        }
                    }
                }
                return result;
            }
            else if constexpr(meanDim == MeanDim3::dim3Pages){
                Mat<rows, cols> result = create_matrix<rows, cols>();
                for(int i = 0; i < rows; ++i){
                    for(int j = 0; j < cols; ++j){
                        for(int p = 0; p < pages; ++p){
                            result[i][j] += m[p][i][j]/pages;
                        }
                    }
                }
                return result;
            }
            else{
                return;
            }
        }

        template<MeanDim3 meanDim, std::size_t pages, std::size_t rows, std::size_t cols, std::size_t Z, std::size_t S>
        bool mean(const Mat3<pages, rows, cols>& m, Mat<Z, S>& m_out) {
            if constexpr(meanDim == MeanDim3::dim3Rows){
                static_assert(cols == Z && pages == S);
                for(int c = 0; c < cols; ++c){
                    for(int p = 0; p < pages; ++p){
                        for(int r = 0; r < rows; ++r){
                            m_out[c][p] += m[p][r][c]/static_cast<double>(rows);
                        }
                    }
                }
                return true;
            }
            else if constexpr(meanDim == MeanDim3::dim3Cols){
                static_assert(rows == Z && pages == S);
                for(int p = 0; p < pages; ++p){
                    for(int r = 0; r < rows; ++r){
                        for(int c = 0; c < cols; ++c){
                            m_out[r][p] += m[p][r][c]/static_cast<double>(cols);
                        }
                    }
                }
                return true;
            }
            else if constexpr(meanDim == MeanDim3::dim3Pages){
                static_assert(rows == Z && cols == S);
                for(int r = 0; r < rows; ++r){
                    // Mat<pages, cols> m_;
                    // get_row(m, r, m_);
                    for(int c = 0; c < cols; ++c){
                        for(int p = 0; p < pages; ++p){
                            m_out[r][c] += m.get_element(p, r, c)/static_cast<double>(pages);
                        }
                    }
                }
                return true;
            }
            else{
                return false;
            }
        }

        template<std::size_t dim, std::size_t Z, std::size_t S>
        std::size_t size(const Mat<Z,S>& m){
            static_assert(dim < 2, "invalid dimension");
            if constexpr(dim == 0){
                return Z;
            }
            else if constexpr(dim == 1){
                return S;
            }
        }

        template<std::size_t rows, std::size_t cols>
        Mat<rows,cols> square(const Mat<rows,cols>& m){
            auto result = cols > rows ? create_matrix<cols, cols>() : create_matrix<rows, rows>();
            for(size_t row = 0; row < rows; ++row){
                for(size_t col = 0; col < cols; ++col){
                    result[row][col] = m[row][col];
                }
            }
            return result;
        }

        using Dimension = std::pair<std::size_t, std::size_t>;

        template<std::size_t N>
        Dimension dimension(const Vec<N>& v){
            return {1, N};
        }

        template<std::size_t rows, std::size_t cols>
        Dimension dimension(const Mat<rows, cols>& m){
            return {rows, cols};
        }

        template<std::size_t rows, std::size_t cols>
        auto ignoreRowCol(const Mat<rows,cols>& m, int row, int col){
            return rowColErase(m, row, col);
        }

        template<std::size_t rows, std::size_t cols>
        auto determinant_Mat(const std::pair<double, Mat<rows, cols>>& in){
            Mat<rows,cols> m = in.second;
            const auto dim = dimension(m);
            auto m_r = Matrix::zeros<0>();
            if constexpr (rows == cols && rows == 2){
                double det = m[0][0]*m[1][1]-m[0][1]*m[1][0];
                std::pair<double, Mat<0,0>> result = {det, m_r};
                return result;
            }
            else{
                double sum = 0.0;
                for(int row = 0; row < rows; ++row){
                    if(m[row][0] == 0) continue;
                    auto m_ = ignoreRowCol(m, row, 0);
                    const auto det = determinant_Mat<rows-1,cols-1>({0, m_});
                    sum += std::pow(-1, row+2) * m[row][0] * det.first;
                }
                std::pair<double, Mat<0,0>> result = {sum, m_r};
                return result;
            }
        }

        template<std::size_t rows, std::size_t cols>
        double determinant(const Mat<rows, cols>& m){
            std::pair<double, Mat<rows, cols>> p = {0, m};
            return determinant_Mat(p).first;
        }

        template<std::size_t rows, std::size_t cols>
        bool hasFullRank(const Mat<rows,cols>& m){
            return 0 != determinant(m);
        }

        template<std::size_t rows, std::size_t cols>
        Mat<rows,cols> invert(const Mat<rows,cols>& m){
            static_assert(rows == cols, "Matrix is not square");
            // needs at least rank of two for determinate
            auto tmp = Matrix::square(m);
            if(!Matrix::hasFullRank(tmp)){
                return {};
            }

            Mat result = Matrix::identity<rows>();

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
    }
};
