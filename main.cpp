#include"Table.h"
//#include "vld.h"
#include <chrono>
#include <type_traits>

int main()
{
	Table table("table.txt");
	table.profile = true;
	std::cout << table << std::endl;
	table.northWestCornerMethod();

	table.potentialsMethod();

	std::cout << table << std::endl;
	table.saveSolution();
	return 0;
}