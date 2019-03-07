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
	log.report() << "______shutdown______\n" << std::endl;
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
	else if (diff > 0) //surplus(add a column)
	{
		AddColumn(diff);
	}
	else //deficiency (add a line)
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
		balancedTable[i] = new Node[width + 1];//each line becomes longer by 1
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

	Node** balancedTable = new Node*[height+1];//add new line
	for (unsigned int i = 0; i < height; i++)
	{
		balancedTable[i] = new Node[width]; 
		for (unsigned int j = 0; j < width; j++)
		{
			balancedTable[i][j] = table[i][j];
		}
	}

	balancedTable[height] = new Node[width];//initialized new string
	balancedTable[height][0]._data.cargoAmount = addCargo;

	clearTable(table);
	table = balancedTable;

	height++;
}

void Table::northWestCornerMethod()
{
	if (err != ERR_OK)
		return;

	Node** saveTable = copyTable(); //here we will store our original table (after all, as we go through the algorithm, weТll mess up the load and needs

	auto start = std::chrono::system_clock::now();
	
	int check = 0;
	unsigned int i = 1, j = 1;



	while (check != width + height - 1)//one iteration of the algorithm
	{
		unsigned int min = std::min(table[0][j]._data.need, table[i][0]._data.cargoAmount);
		table[i][j].value = min; //we transport either everything that is, or we transport everything that is needed
		table[i][j].used = true;
		table[0][j]._data.need -= min; //need diminished
		table[i][0]._data.cargoAmount -= min; //load diminished

		//now this cell is the part of solution
		solution.push_back(min); 
		solution.push_back(i);
		solution.push_back(j);

		if (table[0][j]._data.need == 0 && table[i][0]._data.cargoAmount == 0)//if the demand is fully satisfied and there is no cargo left, then by default the fictitious zero is drawn to the left
		{
			if (j < width - 1) //is there still cell on the right
				table[i][j + 1].used = true;
			else if (i < height - 1) //is there another cell below
				table[i + 1][j].used = true;
			else //if none of this is done, then the algorithm has reached the bottom right edge and ended
				break;

			//diagonal jump
			i++;
			j++;
		}
		else if (table[0][j]._data.need == 0) //if the demand is satisfied, then move to the right
			if (j < width - 1) //move right if there is cell
				j++;
			else //if there is no place, and we have already satisfied all the demand, then something has gone wrong
				log.report() << "smth strange happens on [" << i << ", " << j << "] elem in table: " << std::endl << *this << std::endl;
		else if (table[i][0]._data.cargoAmount == 0) //the whole load is taken out, then we move down
			if (i < height - 1) //move down if there is cell
				i++;
			else //if there is no space below, and we have already taken out the entire load, then something has gone wrong
				log.report() << "smth strange happens on [" << i << ", " << j << "] elem in table: " << std::endl << *this << std::endl;
		
		if (profile)
			log.report() << "PROFILE_STEP " << check << std::endl << *this << std::endl;

		check++;
	}
	std::pair<int, int> tmp;
	while ((tmp = findNewStartPoint()).first != -1)
	{
		std::vector<std::pair<int, int>> chain = FindChainOfRecalc(tmp);
		//Now we have to check some optimizational shit
	}

	clearTable(saveTable);


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
		auto it1 = it;
		it1++;
		auto it2 = it1;
		it2++;
		out << "Send " << (*it) << " cargo from " << *(it1)<< " to " << *(it2) << "; ";
		it = it2;
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
	for (unsigned int i = 0; i < height; i++)
	{
		t[i] = new Node[width];
		for (unsigned int j = 0; j < width; j++)
			t[i][j] = table[i][j];
	}
	return t;
}

void Table::clearTable(Table::Node** t)
{
	if (t != nullptr)
	{
		for (unsigned int i = 0; i < height; i++)
		{
			delete[] t[i];
		}
		delete[] t;
	}
}

std::pair<int, int> Table::findNewStartPoint()
{
	std::vector<std::pair<int, bool>> u;
	std::vector<std::pair<int, bool>> v;
	std::vector<std::pair<int, int>> stack;

	for (int i = 1; i < height; i++)
	{
		u.push_back(std::pair<int, bool>(0,false));
	}
	for (int i = 1; i < width; i++)
	{
		v.push_back(std::pair<int, bool>(0, false));
	}
	u[0].first = 0;
	u[0].second = true;
	
	for (int j = 1; j < width; j++)
	{
		if (table[0][j].used)
		{
			v[j].first = u[0].first - table[0][j]._data.price;
			v[j].second = true;
			stack.push_back(std::pair<int, int>(0,j));
		}
	}

	while (stack.size() != 0)
	{
		std::pair<int, int> a = stack.back();
		stack.pop_back();
		for (int i = 1; i < height; i++)
		{
			if (!table[i][a.second].used) continue; //если клетка не юазалась то идЄм дальше
			if (u[i].second) continue; //если уже вычислили то погнали дальше
			u[i].first = table[i][a.second]._data.price + v[a.second].first;
			u[i].second = true;
			stack.push_back(std::pair<int, int>(i, a.second));
		}
		for (int j = 1; j < width; j++)
		{
			if (!table[a.first][j].used) continue; //если клетка не юазалась то идЄм дальше
			if (v[j].second) continue; //если уже вычислили то погнали дальше
			v[j].first = u[a.first].first - table[a.first][j]._data.price;
			v[j].second = true;
			stack.push_back(std::pair<int, int>(a.first, j));
		}
	}

	for (int i = 1; i < height; i++)
	{
		for (int j = 1; j < width; j++)
		{
			if (!table[i][j].used)
			{
				if (table[i][j]._data.price < u[i-1].first - v[j-1].first)
					return std::pair<int,int>(i,j);					
			}
		}
	}
	return std::pair<int, int>(-1, -1);
}

std::vector<std::pair<int, int>> Table::FindChainOfRecalc(std::pair<int, int> newPoint)
{
	std::vector<std::pair<int, int>> chain;
	std::vector<int> tmp;
	chain.push_back(newPoint);
	while (true)
	{
		int i = 0;
		tmp = solution;
		while (true)
		{
			i++;
			if (newPoint.first != solution[i])
			{
				i++;
				continue;
			}
			else
			{
				chain.push_back(std::pair<int, int>(solution[i], solution[i + 1]));
				solution.erase(solution.begin() + i - 1);
				solution.erase(solution.begin() + i);
				solution.erase(solution.begin() + i + 1);
				newPoint = chain.back();
				i--;
				break;
			}
			i++;
			if (newPoint.second != solution[i])
			{
				i++;
				continue;
			}
			else
			{
				chain.push_back(std::pair<int, int>(solution[i-1], solution[i]));
				solution.erase(solution.begin() + i - 2);
				solution.erase(solution.begin() + i - 1);
				solution.erase(solution.begin() + i);
				newPoint = chain.back();
				i -= 2;
				break;
			}
			
		}

		solution = tmp;

		if (newPoint.first = chain[0].first && newPoint.second == chain[0].second)
			break;
	}
	return chain;
}

//ADDED BY: Ivan Yangildin
//////////////////////////

//some useful functions

//find next var connected by equality with u[num]
std::list<equality>::iterator find_next_v(int num, std::list<equality>* equ_list)
{
	std::list<equality>::iterator it;
	for (it = equ_list->begin(); it != equ_list->end(); it++) {
		if (it->v_num == num)
			return it;
	}
	return it;
}
//find next var connected by equality with v[num]
std::list<equality>::iterator find_next_u(int num, std::list<equality>* equ_list)
{
	std::list<equality>::iterator it;
	for (it = equ_list->begin(); it != equ_list->end(); it++) {
		if (it->u_num == num)
			return it;

	}
	return it;
}
//find solution of system if v[num] is found
void find_solut_rec_v(int num, int* v, int* u, std::list<equality> equ_list) 
{
	std::list<equality>::iterator it;
	while (true)
	{
		it = find_next_v(num, &equ_list);
		if (it == equ_list.end())
			return;
		equality equ = *it;
		equ_list.erase(it);
		int u_num = equ.u_num;
		u[u_num] = v[num] - equ.result;
		find_solut_rec_u(u_num, v, u, equ_list);
	}
}
//find solution of system if u[num] is found
void find_solut_rec_u(int num, int* v, int* u, std::list<equality> equ_list)
{
	std::list<equality>::iterator it;
	while (true)
	{
		it = find_next_u(num, &equ_list);
		if (it == equ_list.end())
			return;
		equality equ = *it;
		equ_list.erase(it);
		int v_num = equ.v_num;
		v[v_num] = u[num] + equ.result;
		find_solut_rec_v(v_num, v, u, equ_list);

	}
}
//find the middle of the list
std::list<std::pair<int, int>>::iterator mean_iter(std::list<std::pair<int, int>>& chain) {
	std::list<std::pair<int, int>>::iterator it = chain.begin();
	for (int i = 0; i < chain.size()/2; i++)
		it++;
	return it;
}

std::pair<int*, int*> Table::findPotentials() {
	int* v = new int[height - 1];
	int* u = new int[width - 1];
	std::list<equality> equ_list;
	for (int i = 1; i < height; i++)
		for (int j = 1; j < width; j++)
		{
			if (table[i][j].used)
			{
				equ_list.push_back({ i - 1, j - 1, table[i][j]._data.price });
			}
		}
	v[0] = 0;
	find_solut_rec_v(0, v, u, equ_list);
	//for (int i = 0; i < height - 1; i++) printf("%i ", v[i]); printf("\n");
	//for (int i = 0; i < width - 1; i++) printf("%i ", u[i]); printf("\n");
	return std::pair<int*, int*>(v, u);
}
std::pair<int, int> Table::findFirstPoint(int* v, int* u)
{
	for (int i = 1; i < height; i++)
		for (int j = 1; j < width; j++)
		{
			if (v[i-1] - u[j-1] > (int) table[i][j]._data.price)
				return std::pair<int, int>(i, j); 
		}
	return std::pair<int, int>(-1, -1);
}

bool Table::is_useable(std::pair<int, int> point, std::list<std::pair<int, int>>& chain) {
	if (!table[point.first][point.second].used)
		return false;
	std::list<std::pair<int, int>>::iterator it = std::find(chain.begin(), chain.end(), point);
	if (it != chain.end())
		return false;
	return true;
}

bool Table::FindChain(std::pair<int, int> point, std::list<std::pair<int, int>>& chain, bool parity) {
	int line = point.first;
	int column = point.second;
	if (table[line][column].used)
	{
		chain.insert(mean_iter(chain), point);
		return true;
	}

	for (int i = 1; i < height; i++)
	{
		if (i == line) continue;
		for (int j = 1; j < width; j++)
		{
			if (j == column) continue;
			std::pair<int, int> point_a(line, j);
			std::pair<int, int> point_x(i, j);
			std::pair<int, int> point_b(i, column);

			if (is_useable(point_a, chain) && is_useable(point_b, chain))
			{
				if (parity){
					chain.insert(mean_iter(chain), point_a);
					chain.insert(mean_iter(chain), point_b);
				}
				else{
					chain.insert(mean_iter(chain), point_b);
					chain.insert(mean_iter(chain), point_a);
				}
				if (FindChain(point_x, chain, !parity)) return true;
				else
				{
					chain.remove(point_a);
					chain.remove(point_b);
				}
			}
		}
	}

	return false;
}
void Table::Rebalance(std::list<std::pair<int, int>>& chain)
{
	std::list<std::pair<int, int>>::iterator it, it_min;
	int i, j, dzet;
	bool is_first = true, minus = false;
	for (it = chain.begin(); it != chain.end(); it++)
	{
		i = (*it).first;
		j = (*it).second;
		if (minus)
		{
			if (is_first)
			{
				is_first = false;
				dzet = table[i][j].value;
				it_min = it;
			}
			else
			{
				if (dzet > table[i][j].value)
				{
					dzet = table[i][j].value;
					it_min = it;
				}
			}
		}
		minus = !minus;
	}
	minus = false;
	for (it = chain.begin(); it != chain.end(); it++)
	{
		i = (*it).first;
		j = (*it).second;
		if (minus)
		{
			table[i][j].value -= dzet;
			if (it == it_min)
				table[i][j].used = false;
		}
		else
			table[i][j].value += dzet;
		minus = !minus;
	}
	i = (*chain.begin()).first;
	j = (*chain.begin()).second;
	table[i][j].used = true;
}
void Table::potentialsMethod()
{
	while (true)
	{
		std::pair<int*, int*> potent = findPotentials();
		std::pair<int, int> point = findFirstPoint(potent.first, potent.second);
		if (point.first == -1)
			return;
		std::list<std::pair<int, int>> list;
		bool b = FindChain(point, list, false);
		list.push_front(point);
		Rebalance(list);
		list.clear();

		std::cout << *this << std::endl;
	}
}