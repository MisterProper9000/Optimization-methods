#include "Table.h"

Table::Table(const char* name)
{
	std::ifstream in(name);
	
	int a, b;
	in >> a >> b;

	numClosed = 0;
	numSuppliers = a - 1;
	numConsumers = b - 1;
	Node** table = new Node*[a];

	for (int i = 0; i < a; i++)
	{
		table[i] = new Node[b];

		for (int j = 0; j < b; j++)
		{
			in >> table[i][j];
		}
	}
	in.close();

	table[0][0].closed = true; //помещаем данные о потребности и количестве груза в нулевые столбцы и строки, чтоб индексы данных о тарифах начинались с единицы как в математике любят

	if (!isBalancedTask())
		toBalancedTask();
}

Table::~Table()
{
	for (int i = 0; i < numSuppliers; i++)
	{
		delete table[i];
	}
	delete table;
}

void Table::FindOptimalStrategy()
{
	northWestCornerMethod();
}

bool Table::isBalancedTask()
{
	int totalCargo = 0;
	int totalNeed = 0;

	for (int i = 0; i < numConsumers; i++)
		totalNeed += table[i][0]._data.need;
	for (int i = 0; i < numConsumers; i++)
		totalCargo += table[i][0]._data.cargoAmount;

	//if(totalCargo != )
}

void Table::toBalancedTask()
{

}

int* Table::northWestCornerMethod(unsigned int complication = 0)
{
	int check = 0;
	while (check != numConsumers + numSuppliers - 1)
	{

	}
}

void Table::printSolution()
{
	std::ofstream out("out.txt");

	for (auto it = solution.begin(); it != solution.end(); it++)
	{
		out << "Send " << (*it)++ << " cargo from " << (*it)++ << " to " << *it;
	}
}