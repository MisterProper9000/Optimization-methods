#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <list>
#include <vector>
#include <chrono>
#include <ctime>
#include <algorithm>
#include <cmath>


class Table
{
public:
	Table(const char* fileName);
	~Table();

	bool profile; //is there need to output the result of the method operation at each optimization iteration

	unsigned int height; //number of suppliers+1
	unsigned int width; //number of consumers+1

	void northWestCornerMethod(); //northwest corner method

    //TO-DO: int* minElemMethod();     //minimum element method
	//TO-DO: int* dualPreference();  //dual preference method

	void saveSolution(); //save the solution to a file

	friend std::ostream& operator << (std::ostream &out, Table &table);   //display table in console
	friend std::ofstream& operator << (std::ofstream &out, Table &table); //save the table to a file

	bool isErr()
	{
		return err != ERR_OK;
	}
	
	//ADDED BY: Ivan Yangildin
	//////////////////////////
	void potentialsMethod();
private:
	class Node {
	public:
		Node() :used(false)
		{
			this->value = 0;
			this->_data.price = 0;
		}

		unsigned int value; //scope of delivery (tariff matrix cell)
		bool used;
		union data //initialization data
		{
			unsigned int price; //tariff
			unsigned int cargoAmount; //the amount of cargo at the supplier (zero column)
			unsigned int need; //how much the consumer needs (zero line)
		};

		data _data;
		

		friend std::ifstream& operator >> (std::ifstream &in, Node &node)
		{
			in >> node._data.price;
			return in;
		}
	};

	class Log //logger
	{
		const char* logDirDef = "log.txt";
	public:

		std::ofstream report()
		{
			std::ofstream out;
			out.open(logDirDef, std::ios::app);

			time_t now = time(NULL);
			char str[26];
			ctime_s(str, sizeof(str), &now);
			out << "---" << str << ">";
			return out;
		}
	};

	enum ERROR
	{
		ERR_OK,
		ERR_SIZE,
		ERR_PARAM
	};

	ERROR err;

	Log log;

	const char* saveDirDef = "out.txt";

	Node** table; //use arrays because there will be a few reallocs (at most one), and access is needed often and quickly

	std::vector<int> solution; //solution in the format how much/from where/to where

	int isBalancedTask(); //is the task balanced
	void toBalancedTask(int diff); //reduction to balanced, in: difference between total load and total need

	void AddColumn(int addNeed); //add fictitious consumer
	//TO-DO: void AddColumn(int addNeed, unsigned int j); //add a fictitious consumer to a specific place in the table (needed for the 4th complication)

	void AddLine(int addCargo);  //add fictitious supplier
	//TO-DO: void AddLine(int addCargo, unsigned int i);  //add a fictitious supplier to a certain place in the table (needed for the 4th complication)

	Node** copyTable(); //copying the table (size ONLY heightXwidth !!!)

	void clearTable(Node**); //deleting the table (size ONLY heightXwidth !!!)

	std::pair<int, int> findNewStartPoint();

	std::vector<std::pair<int,int>> FindChainOfRecalc(std::pair<int, int> newPoint);

	//ADDED BY: Ivan Yangildin
	//////////////////////////

	//solve the system of equalities for potentials
	std::pair<int*, int*> findPotentials();

	//find a point wich brings to false inequality
	std::pair<int, int> findFirstPoint(int* u, int* v);

	//find chain of recalculation
	bool FindChain(std::pair<int, int> point, std::list<std::pair<int, int>>& chain, bool parity);

	//make new solution
	void Rebalance(std::list<std::pair<int, int>>& chain);

	//true if we can use cell for chain of recalculation
	bool is_useable(std::pair<int, int> point, std::list<std::pair<int, int>>& chain);

};



struct equality
{
	int v_num;
	int u_num;
	unsigned int result;
};
void find_solut_rec_v(int num, int* v, int* u, std::list<equality> equ_list);
void find_solut_rec_u(int num, int* v, int* u, std::list<equality> equ_list);