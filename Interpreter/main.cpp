
#include <algorithm>
#include <cctype>
#include <cstdio>
#include<iostream>
#include <cstdlib>
#include <map>
#include <memory>
#include <string>
#include <vector>
#include"llvmsql.h"






int main()
{
	init_scanner();
	std::unique_ptr<ExprAST> x;
	init_parser();
	
	//auto z = ParseStatementAST();
	int qwexqy = 0;
	qwexqy++;
	while (qwexqy)
	{
		try
		{
			auto z = ParseStatementAST();
			qwexqy++;
		}
		catch (std::runtime_error& s)
		{
			std::cout << s.what() << std::endl;
		}
	}
		
	int ty = qwexqy;
	
	return 0;
}