#include "gtest/gtest.h"
#include <Matrix.h>

TEST(Matrix, Build){
    EXPECT_TRUE(true);
    EXPECT_EQ(2, 1+1);
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
