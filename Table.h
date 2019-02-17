#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <list>
#include <chrono>
#include <ctime>
#include <algorithm>
#include <cmath>


class Table
{
public:
	Table(const char* fileName);
	~Table();

	bool profile; //надо ли на каждой итерации оптимизации выводить результат работы метода

	unsigned int height; //количество поставщиков+1
	unsigned int width; //количество потребителей+1

	void northWestCornerMethod(); //метод северо-западного угла

    //TO-DO: int* minElemMethod();     //метод минимального элемента
	//TO-DO: int* doublePreference();  //метод двойного предпочтения

	void saveSolution(); //сохраняем решение в файл

	friend std::ostream& operator << (std::ostream &out, Table &table);   //выводим таблицу в консоль
	friend std::ofstream& operator << (std::ofstream &out, Table &table); //сохраняем таблицу в файл

	bool isErr()
	{
		return err != ERR_OK;
	}

private:
	class Node {
	public:
		Node() :used(false)
		{
			this->value = 0;
			this->_data.price = 0;
		}

		unsigned int value; //объём поставки (ячейка матрицы тарифов)
		bool used;
		union data //данные инициализации
		{
			unsigned int price; //тариф
			unsigned int cargoAmount; //количество груза у поставщика (нулевой столбец)
			unsigned int need; //сколько нужно потребителю (нулевая строка)
		};

		data _data;
		

		friend std::ifstream& operator >> (std::ifstream &in, Node &node)
		{
			in >> node._data.price;
			return in;
		}
	};

	class ChainOfRecalc //цепь пересчёта
	{

	};

	class Log //логгер
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

	Node** table; //используем массивы потому что реаллоков будет мало (как максимум один), а доступ нужен часто и быстро

	std::list<int> solution; //решение в формате сколько откуда куда

	int isBalancedTask(); //является ли задача сбалансированной
	void toBalancedTask(int diff); //приведение к сбалансированной, in: разность между суммарным грузом и суммарной нуждой

	void AddColumn(int addNeed); //добавляем фиктивного потребетеля
	//TO-DO: void AddColumn(int addNeed, unsigned int j); //добавляем фиктивного потребетеля в определённое место таблицы (нужно для 4го усложения)

	void AddLine(int addCargo);  //добавляем фиктивного поставщика
	//TO-DO: void AddLine(int addCargo, unsigned int i);  //добавляем фиктивного потсавщика в определённое место таблицы (нужно для 4го усложения)

	Node** copyTable(); //копирование таблицы (ТОЛЬКО РАЗМЕРОВ height на width!!!)

	void clearTable(Node**); //удаление таблицы (ТОЛЬКО РАЗМЕРОВ height на width!!!)
};