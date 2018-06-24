#include"llvmsql.h"
#include<iostream>
int main()
{
	init_cata(); 
	initbuff(buff);
	init_scanner();
	init_parser();
	
	int qwexqy = 0;
	qwexqy++;
	while (qwexqy)
	{
		try
		{
			auto z = ParseStatementAST();
			qwexqy++;
			if (z == nullptr)
				break;
		}
		catch (std::runtime_error& s)
		{
			std::cout << s.what() << std::endl;
			skip_exp();
		}
	}
	int ty = qwexqy;
	cata_wb();
	return 0;
}