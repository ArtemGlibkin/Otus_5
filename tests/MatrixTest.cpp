#include <gtest/gtest.h>
#include "Matrix.h"
#include <sstream>

TEST(MatrixTest, BaseTest) {
	Matrix<int, -1> matrix;     
	ASSERT_EQ(matrix.size(), 0); 
	auto a = matrix[0][0];     
	ASSERT_TRUE(a == -1);     
	ASSERT_EQ(matrix.size(), 0);
	matrix[100][100] = 314;     
	ASSERT_TRUE(matrix[100][100] == 314);     
	ASSERT_EQ(matrix.size(), 1);       // выведется одна строка     // 100100314     
	std::stringstream ss;
	for(auto c: matrix)     
	{         
		int x;	int y;	int v;         
		std::tie(x, y, v) = c;         
		ss << x << y << v;     
	}  
	ASSERT_EQ(ss.str(), std::string("100100314"));
}