#include "Table.h"

Table::Table(const char* fileName)
{
	log.report() << "______start_new_session______" << std::endl;
	err = ERR_OK;

	profile = false;

	table = nullptr;

	std::ifstream in(fileName);
	
	in >> height >> width;

	if (height <= 1 || width <= 1)
	{
		std::cout << "Invalid table size" << std::endl;
		log.report() << "Invalid table size" << std::endl;
		err = ERR_SIZE;
		return;
	}

	table = new Node*[height];

	for (unsigned int i = 0; i < height; i++)
	{
		table[i] = new Node[width];

		for (unsigned int j = (i==0)?1:0; j < width; j++)
		{
			in >> table[i][j];
		}
	}
	in.close();

	toBalancedTask(isBalancedTask());
}

Table::~Table()
{
	log.report() << "______shutdown______" << std::endl;
	clearTable(table);
}

int Table::isBalancedTask()
{
	if (err != ERR_OK)
		return 0;

	int totalCargo = 0;
	int totalNeed = 0;

	for (unsigned int i = 1; i < width; i++)
		totalNeed += table[0][i]._data.need;
	for (unsigned int i = 1; i < height; i++)
		totalCargo += table[i][0]._data.cargoAmount;

	return totalCargo - totalNeed;
}

void Table::toBalancedTask(int diff)
{
	if (err != ERR_OK)
		return;

	if (diff == 0)
		return;
	else if (diff > 0) //профицит (надо столбец добавить)
	{
		AddColumn(diff);
	}
	else //дефицит (надо строку добавить)
	{
		AddLine(-diff);
	}
}

void Table::AddColumn(int addNeed)
{
	if (err != ERR_OK)
		return;

	Node** balancedTable = new Node*[height];
	for (unsigned int i = 0; i < height; i++)
	{
		balancedTable[i] = new Node[width + 1];//каждая строка стала длиннее на 1
		for (unsigned int j = 0; j < width; j++)
		{
			balancedTable[i][j] = table[i][j];
		}		
	}

	balancedTable[0][width]._data.need = addNeed;

	clearTable(table);
	table = balancedTable;

	width++;
}

void Table::AddLine(int addCargo)
{
	if (err != ERR_OK)
		return;

	Node** balancedTable = new Node*[height+1];//добавили новую строку
	for (unsigned int i = 0; i < height; i++)
	{
		balancedTable[i] = new Node[width]; 
		for (unsigned int j = 0; j < width; j++)
		{
			balancedTable[i][j] = table[i][j];
		}
	}

	balancedTable[height] = new Node[width];//инициализировали новую строку
	balancedTable[height][0]._data.cargoAmount = addCargo;

	clearTable(table);
	table = balancedTable;

	height++;
}

void Table::northWestCornerMethod()
{
	if (err != ERR_OK)
		return;

	Node** saveTable = copyTable(); //схороним тут нашу оригинальную таблицу (мы ведь по мере прохождения алгоритма похерим число груза и потребности

	auto start = std::chrono::system_clock::now();
	
	int check = 0;
	unsigned int i = 1, j = 1;

	while (check != width + height - 1)//одна итерация алгоритма
	{
		table[i][j].value = std::min(table[0][j]._data.need, table[i][0]._data.cargoAmount); //перевозим либо всё что есть, либо перевозим всё что нужно
		table[i][j].used = true;

		if (table[i][j].value == table[0][j]._data.need == table[i][0]._data.cargoAmount)//если спрос полностью удовлетворён и груза не осталось, то по дефолту фиктивный ноль фигачим влево
		{
			if (j < width - 1) //есть ли ещё место справа
				table[i][j + 1].used = true;
			else if (i < height - 1) //есть ли ещё место снизу
				table[i + 1][j].used = true;
			else //если ни что из этого не выполнилось, значит алгоритм дошёл до нижнего правого края и завершился
				break;
		}
		else if (table[i][j].value == table[0][j]._data.need) //если спрос удовлетворён, то сдвигаемся вправо
			if (j < width - 1) //сдвигаемся вправо, если есть место
				j++;
			else //если места спрва нет, а мы уже удовлетворили весь спрос, значит что-то пошло не так
				log.report() << "smth strange happens on [" << i << ", " << j << "] elem in table: " << std::endl << *this << std::endl;
		else if (table[i][j].value == table[i][0]._data.need) //весь груз вывезен, то сдвигаемся вниз
			if (i < height - 1) //сдвигаемся вниз, если есть место
				i++;
			else //если места снизу нет, а мы уже вывези весь груз, значит что-то пошло не так
				log.report() << "smth strange happens on [" << i << ", " << j << "] elem in table: " << std::endl << *this << std::endl;
		
		if (profile)
			log.report() << "PROFILE_STEP " << check << *this << std::endl;

		check++;
	}




	auto end = std::chrono::system_clock::now();
	std::chrono::duration<double> elapsed_seconds = end - start;
	std::time_t start_time = std::chrono::system_clock::to_time_t(start);

	char str[26];
	ctime_s(str, sizeof(str), &start_time);

	log.report() << "started computation at " << str
		<< ">elapsed time: " << elapsed_seconds.count() << "s\n";
	
}

void Table::saveSolution()
{
	if (err != ERR_OK)
		return;

	std::ofstream out;
	out.open(saveDirDef, std::ios::app);
	out << "<" << std::chrono::system_clock::to_time_t(std::chrono::system_clock::now()) << ">" << std::endl;
	out << table << std::endl;
	out << "Solution: ";

	for (auto it = solution.begin(); it != solution.end(); it++)
	{
		out << "Send " << (*it)++ << " cargo from " << (*it)++ << " to " << *it;
	}
	out << std::endl << "_______end_solution_______" << std::endl;
	out.close();

	log.report() << "solution saved to " << saveDirDef << std::endl;
}

std::ostream& operator << (std::ostream &out, Table &table)
{
	if (table.isErr())
		return out;

	for (unsigned int i = 0; i < table.width; i++)
	{
		out << table.table[0][i]._data.price << "    ";
	}
	out << std::endl;

	for (unsigned int j = 1; j < table.height; j++)
	{
		for (unsigned int i = 0; i < table.width; i++)
		{
			out << table.table[j][i]._data.price;
			if (i != 0)
				out << "/" << table.table[j][i].value << "/" << ((table.table[j][i].used) ? "T" : "F");
			out << " ";
		}
		out << std::endl;
	}
	return out;
}

std::ofstream& operator << (std::ofstream &out, Table &table)
{
	if (table.isErr())
		return out;

	for (unsigned int i = 0; i < table.width; i++)
	{
		out << table.table[0][i]._data.price << "   ";
	}
	out << std::endl;

	for (unsigned int j = 1; j < table.height; j++)
	{
		for (unsigned int i = 0; i < table.width; i++)
		{
			out << table.table[j][i]._data.price;
			if (i != 0)
				out << "/" << table.table[j][i].value << "/" << ((table.table[j][i].used)?"T":"F");
			out << " ";
		}
		out << std::endl;
	}
	return out;
}

Table::Node** Table::copyTable()
{
	Node** t = new Node*[height];
	for (int i = 0; i < height; i++)
	{
		t[i] = new Node[width];
		for (int j = 0; j < width; j++)
			t[i][j] = table[i][j];
	}
}

void Table::clearTable(Table::Node** t)
{
	if (t != nullptr)
	{
		for (unsigned int i = 0; i < height; i++)
		{
			delete[] table[i];
		}
		delete[] table;
	}
}