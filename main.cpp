#include <iostream>
#include "Matrix.h"

int main(int argc, char **argv){
    // Mat a{{1,1},{1,1},{1,1}};
    // Mat b{{1,0,0},{0,1,0}};

    Mat a{
        {1,1},
        {2,2}};

    Mat b{
        {1,1},
        {2,2}};

    Mat c = a * b;
    return 0;
}
