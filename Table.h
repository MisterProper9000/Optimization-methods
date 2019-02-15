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

	unsigned int numSuppliers; //���������� �����������
	unsigned int numConsumers; //���������� ������������
	unsigned int numClosed;    //����� �������� �����

	void FindOptimalStrategy(); //������� ������ �����������

	bool isBalancedTask(); //�������� �� ������ ����������������
	void toBalancedTask(); //���������� � ����������������

	int* northWestCornerMethod(unsigned int complication = 0); //����� ������-��������� ����
	int* minElemMethod() { return nullptr; }    //����� ������������ ��������
	int* doublePreference() { return nullptr; } //����� �������� ������������

	void printSolution();

private:
	class Node {
	public:
		Node() :closed(false)
		{
			this->_data.value = 0;
		}

		unsigned int price; //�����

		union data //� �� ������ ��� �� �����, �� ���� ���������� ����������� ����� ��� �������� ���� � ��� �� ����� ������ ��-�������
		{
			unsigned int value; //����� �������� (������ ������� �������)
			unsigned int cargoAmount; //����� �������� (������� �������)
			unsigned int need; //����� �������� (������� ������)
		};

		data _data; 
		bool closed; //������� ������ ��� ���

		friend std::ifstream& operator >> (std::ifstream &in, Node &node)
		{
			in >> node.price;
		}
	};

	Node** table; //���������� ������� ������ ��� ��������� ����� ���� (��� �������� ����), � ������ ����� ����� � ������

	std::list<int> solution; //������� � ������� ������� ������ ����
};