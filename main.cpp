#include"Table.h"
#include "vld.h"
#include <chrono>
#include <type_traits>

int main()
{
	Table table("table.txt");
	table.profile = true;
	std::cout << table << std::endl;
	table.northWestCornerMethod();
	table.saveSolution();
	return 0;
}