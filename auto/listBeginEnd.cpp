#include "listBeginEnd.h"

#include <iostream>
#include <vector>
using namespace std;

struct Node
{
	int num;
	Node* next;
};

class Mylist
{
	Node* head;
	Node* tail;
	Node* now;
public:
	Node* begin() { return head; }
	Node* end() { return tail; }
	Node* operator++()
	{
		if (now != tail) now = now->next;
		return now;
	}
};

int main()
{
	Mylist ls;
	for (const auto& e : ls)
	{

	}
}