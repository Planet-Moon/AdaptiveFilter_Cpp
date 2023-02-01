#include "gtest/gtest.h"
#include <Matrix.h>

TEST(Matrix, Build){
    EXPECT_TRUE(true);
    EXPECT_EQ(2, 1+1);
}

TEST(Vector, Transpose){
    Vec v = {1,2,3};
    auto dim_v = dimension(v);
    Mat m = transpose(v);
    auto dim_m = dimension(m);
    Mat expected = {{1},{2},{3}};
    EXPECT_EQ(expected, m);
}

TEST(Vector, ToMatrixTranspose){
    Vec v = {1,2,3};
    Mat m = convertToMat(v);
    auto dim_m = dimension(m);
    Mat m_t = Matrix::transpose(m);
    auto dim_m_t = dimension(m_t);
    Mat expected = {{1},{2},{3}};
    EXPECT_EQ(expected, m_t);
}

TEST(Vector, times){
    Vec v1 = {1,2,3};
    Vec v2 = {4,5,6};
    Vec v3 = times(v1, v2);
    Vec expected = {4,10,18};
    EXPECT_EQ(v3, expected);
}

TEST(Vector, rdivide){
    Vec v1 = {12,32,36};
    Vec v2 = {6,8,12};
    Vec v3 = rdivide(v1, v2);
    Vec expected = {2,4,3};
    EXPECT_EQ(v3, expected);
}

TEST(Vector, ldivide){
    Vec v1 = {6,8,12};
    Vec v2 = {12,32,36};
    Vec v3 = ldivide(v1, v2);
    Vec expected = {2,4,3};
    EXPECT_EQ(v3, expected);
}

TEST(Vector, power){
    Vec v1 = {12,32,36};
    Vec v2 = {0,1,2};
    Vec v3 = power(v1, v2);
    Vec expected = {1, 32, std::pow(36, 2)};
    EXPECT_EQ(v3, expected);
}

TEST(Matrix, MatVec){
    Mat a = {{0,1,2},{3,4,5}};
    Vec v = {2,2,2};
    Vec v2 = a * v;
    Vec expected = {6,24};
    EXPECT_EQ(v2, expected);
}

TEST(Matrix, MatVec2){
    Mat a = {{0},{1},{2},{3}};
    Vec v = {2};
    Vec v2 = a * v;
    Vec expected = {{0},{2},{4},{6}};
    EXPECT_EQ(v2, expected);
}

TEST(Matrix, MatMat){
    Mat a = {{0,1,2},{3,4,5}};
    Mat b = {{1,1,1},{2,2,2},{3,3,3}};
    Mat r = a * b;
    Mat expected = {{8,8,8},{26,26,26}};
    EXPECT_EQ(r, expected);
}

TEST(Matrix, ScalarMat){
    Mat a = {{0,1,2},{3,4,5}};
    {
        Mat r = 2 * a;
        Mat expected = {{0,2,4},{6,8,10}};
        EXPECT_EQ(r, expected);
    }
    {
        Mat r = a * 2;
        Mat expected = {{0,2,4},{6,8,10}};
        EXPECT_EQ(r, expected);
    }
}

TEST(Matrix, Matrix_add_Vec){
    Mat a = {{0,1,2},{3,4,5}};
    Vec b = {1,2,3};
    Mat c = a + b;
    Mat expected = {{1,3,5},{4,6,8}};
    EXPECT_EQ(c, expected);
}

TEST(Matrix, Matrix_sub_Vec){
    Mat a = {{0,1,2},{3,4,5}};
    Vec b = {1,2,3};
    Mat c = a - b;
    Mat expected = {{-1,-1,-1},{2,2,2}};
    EXPECT_EQ(c, expected);
}

TEST(Matrix, Identity) {
    Mat m = Matrix::identity(3);
    Mat expected = {{1,0,0},{0,1,0},{0,0,1}};
    EXPECT_EQ(m, expected);
}

TEST(Matrix, Transpose){
    Mat m = Matrix::identity(3);
    Mat m2 = Matrix::transpose(m);
    Mat expected = {{1,0,0},{0,1,0},{0,0,1}};
    EXPECT_EQ(m2, expected);
}

TEST(Matrix, Transpose2){
    Mat m = {{1,1,1},{2,2,2}};
    Mat m2 = Matrix::transpose(m);
    Mat expected = {{1,2},{1,2},{1,2}};
    EXPECT_EQ(m2, expected);
}

TEST(Matrix, Mean1){
    Mat m = {{0,1,1},{2,3,2},{1,3,2},{4,2,2}};
    Mat mean = Matrix::mean(m,1);
    Mat expected = {{1.75, 2.25, 1.75}};
    EXPECT_EQ(mean, expected);
}

TEST(Matrix, Mean2){
    Mat m = {{0,1,2},{1,4,4},{4,0,-1},{-2,2,3}};
    Mat mean = Matrix::mean(m,2);
    Mat expected = {{1}, {3}, {1}, {1}};
    EXPECT_EQ(mean, expected);
}

TEST(Matrix, determinant){
    Mat m = {{0,1,2},{3,2,1},{1,1,0}};
    double det = Matrix::determinant(m);
    double expected = 3;
    EXPECT_EQ(det, expected);
}

TEST(Matrix, determinant2){
    Mat m = {{3,4,2},{2,1,1},{4,6,2}};
    double det = Matrix::determinant(m);
    double expected = 4;
    EXPECT_EQ(det, expected);
}

TEST(Matrix, determinant3){
    Mat m = {{3,4,2,12},{2,1,1,4},{4,6,2,8},{1,3,5,7}};
    double det = Matrix::determinant(m);
    double expected = -196;
    EXPECT_EQ(det, expected);
}

TEST(Matrix, determinant4){
    Mat m = {{3,4,2,12,33},{2,1,1,4,2},{4,6,2,8,6},{1,3,5,7,3},{1,4,7,3,2}};
    double det = Matrix::determinant(m);
    double expected = -6062;
    EXPECT_EQ(det, expected);
}

TEST(Matrix, determinant5){
    Mat m = {{3,4,2,12,33,53,57},{2,1,1,4,2,0,1},{4,6,2,8,6,3,6},{1,3,5,7,3,3,2},{1,4,7,3,2,8,4},{4,6,2,8,6,3,6},{1,3,2,3,6,3,9}};
    double det = Matrix::determinant(m);
    double expected = 0;
    EXPECT_EQ(det, expected);
}

TEST(Matrix, determinant6){
    Mat m = {{3,4,2,12,33,53,57},{2,1,1,4,2,0,1},{4,6,2,8,6,3,6},{1,3,5,7,3,3,2},{1,4,7,3,2,8,4},{4,6,2,5,6,3,6},{1,3,2,3,6,3,9}};
    double det = Matrix::determinant(m);
    double expected = 78804;
    EXPECT_EQ(det, expected);
}

TEST(Matrix, determinant7){
    Mat m = {{1,0,0,0,0,0,0},{0,1,0,0,0,0,0},{0,0,1,0,0,0,0},{0,0,0,1,0,0,0},{0,0,0,0,1,0,0},{0,0,0,0,0,1,0},{0,0,0,0,0,0,1}};
    double det = Matrix::determinant(m);
    double expected = 1;
    EXPECT_EQ(det, expected);
}

TEST(Matrix, hasFullRank){
    {
        Mat m = {{1,1},{2,2}};
        bool full_rank = Matrix::hasFullRank(m);
        EXPECT_FALSE(full_rank);
    }
    {
        Mat m = {{1,1},{-2,2}};
        bool full_rank = Matrix::hasFullRank(m);
        EXPECT_TRUE(full_rank);
    }
}

TEST(Matrix, invert){
    Mat m = {{-1, 1.5}, {2, -1}};
    Mat m_inv = Matrix::invert(m);
    Mat expected = {{0.5, 0.75}, {1, 0.5}};
    EXPECT_EQ(m_inv, expected);
}

TEST(Matrix, invert1){
    Mat m = {{-1, 1.5}, {1, -1}};
    Mat m_inv = Matrix::invert(m);
    Mat expected = {{2, 3}, {2, 2}};
    EXPECT_EQ(m_inv, expected);
}
