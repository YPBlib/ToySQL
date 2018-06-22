#include"llvmsql.h"
#include<iostream>
int main()
{
	init_scanner();
	init_parser();
	init_cata();
	int qwexqy = 0;
	qwexqy++;
	while (qwexqy)
	{
		try
		{
			auto z = ParseStatementAST();
			qwexqy++;
			if (z == nullptr)
				return 0;
		}
		catch (std::runtime_error& s)
		{
			std::cout << s.what() << std::endl;
			skip_exp();
		}
	}
	int ty = qwexqy;
	return 0;
}