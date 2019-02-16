#include "parser.h"
int main()
{
	init_cata(); 
	initbuff();
	init_scanner();
	init_parser();
	

	while (1)
	{
		try
		{
			auto z = ParseStatementAST();
			if (z == nullptr)
				break;
		}
		catch (std::runtime_error& s)
		{
			std::cout << s.what() << std::endl;
			skip_exp();
		}
	}
	return 0;
}