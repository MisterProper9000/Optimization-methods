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

	bool profile; //���� �� �� ������ �������� ����������� �������� ��������� ������ ������

	unsigned int height; //���������� �����������+1
	unsigned int width; //���������� ������������+1

	void northWestCornerMethod(); //����� ������-��������� ����

    //TO-DO: int* minElemMethod();     //����� ������������ ��������
	//TO-DO: int* doublePreference();  //����� �������� ������������

	void saveSolution(); //��������� ������� � ����

	friend std::ostream& operator << (std::ostream &out, Table &table);   //������� ������� � �������
	friend std::ofstream& operator << (std::ofstream &out, Table &table); //��������� ������� � ����

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

		unsigned int value; //����� �������� (������ ������� �������)
		bool used;
		union data //������ �������������
		{
			unsigned int price; //�����
			unsigned int cargoAmount; //���������� ����� � ���������� (������� �������)
			unsigned int need; //������� ����� ����������� (������� ������)
		};

		data _data;
		

		friend std::ifstream& operator >> (std::ifstream &in, Node &node)
		{
			in >> node._data.price;
			return in;
		}
	};

	class ChainOfRecalc //���� ���������
	{

	};

	class Log //������
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

	Node** table; //���������� ������� ������ ��� ��������� ����� ���� (��� �������� ����), � ������ ����� ����� � ������

	std::list<int> solution; //������� � ������� ������� ������ ����

	int isBalancedTask(); //�������� �� ������ ����������������
	void toBalancedTask(int diff); //���������� � ����������������, in: �������� ����� ��������� ������ � ��������� ������

	void AddColumn(int addNeed); //��������� ���������� �����������
	//TO-DO: void AddColumn(int addNeed, unsigned int j); //��������� ���������� ����������� � ����������� ����� ������� (����� ��� 4�� ���������)

	void AddLine(int addCargo);  //��������� ���������� ����������
	//TO-DO: void AddLine(int addCargo, unsigned int i);  //��������� ���������� ���������� � ����������� ����� ������� (����� ��� 4�� ���������)

	Node** copyTable(); //����������� ������� (������ �������� height �� width!!!)

	void clearTable(Node**); //�������� ������� (������ �������� height �� width!!!)
};