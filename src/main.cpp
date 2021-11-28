#include <iostream>
#include <assert.h>
#include "Matrix.h"

int main()
{

    Matrix<int, 0> matrix;    
  
    for (int i = 0; i <= 9; i++)
    {
        matrix[i][i] = i;
        matrix[9 - i][i] = 9 - i;
    }
    
    for (auto it : matrix)
    {
       int x; int y; int v;      
       std::tie(x, y, v) = it;         
       std::cout << x << y << v << std::endl;
    }
    
	
    //matrix.print(1, 8, 8);
    std::cout << "Matrix capacity " <<matrix.size() << std::endl;
}