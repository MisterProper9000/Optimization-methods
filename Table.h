#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <list>

class Table
{
public:
	Table(const char* name);
	~Table();

	unsigned int numSuppliers; //количество поставщиков
	unsigned int numConsumers; //количество потребителей
	unsigned int numClosed;    //число закрытых ячеек

	void FindOptimalStrategy(); //решение задачи оптимизации

	bool isBalancedTask(); //является ли задача сбалансированной
	void toBalancedTask(); //приведение к сбалансированной

	int* northWestCornerMethod(unsigned int complication = 0); //метод северо-западного угла
	int* minElemMethod() { return nullptr; }    //метод минимального элемента
	int* doublePreference() { return nullptr; } //метод двойного предпочтения

	void printSolution();

private:
	class Node {
	public:
		Node() :closed(false)
		{
			this->_data.value = 0;
		}

		unsigned int price; //тариф

		union data //я бы сказал это не нужно, но ради понятности обозначений дадим нам называть один и тот же кусок памяти по-разному
		{
			unsigned int value; //объём поставки (ячейка матрицы тарифов)
			unsigned int cargoAmount; //сколько груза у поставщика (нулевой столбец)
			unsigned int need; //сколько груза нужно потребителю (нулевая строка)
		};

		data _data; 
		bool closed; //закрыта ячейка или нет

		friend std::ifstream& operator >> (std::ifstream &in, Node &node)
		{
			in >> node.price;
		}
	};

	Node** table; //используем массивы потому что реаллоков будет мало (как максимум один), а доступ нужен часто и быстро

	std::list<int> solution; //решение в формате сколько откуда куда
};
