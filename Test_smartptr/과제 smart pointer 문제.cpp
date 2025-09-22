//메니저 클래스, STL, 스마트포인터 활용하기 ***
 
// Weapon, Armor, Ring 등 카테고리 별로 구분되는 아이템을 관리하려고 합니다.  	//상속, 다형성
// 추가, 삭제, 검색, 출력, 정렬, 합성 기능을 구현하세요.


//Item class 를 만들고 
//		id, name, level, grade 등 데이타 맴버를 추가하세요.

#include <iostream>
#include <vector>
#include <algorithm>
#include <functional>
using namespace std;

class Item : enable_shared_from_this<Item> {
public:
	int		id = 0;
	string	name = "";
	int		level = 0;
	char	grade = 'A';
	Item(int id, string name, int level, char grade) : id(id), name(name), level(level), grade(grade) {    }
	virtual ~Item() { }
};
class Weapon : public Item {
public:
	int attack = 0;
	Weapon(int id, string name, int level, char grade) : Item(id, name, level, grade) {    }
};
class Armor : public Item {
public:
	int defence = 0;
	Armor(int id, string name, int level, char grade) : Item(id, name, level, grade) {    }
};

class ItemManager
{
	vector<shared_ptr<Item>> itemlist;

public:
	void AddItem(const shared_ptr<Item>& item) { itemlist.push_back(item); }
	void RemoveItemByName(const string& name)
	{
		itemlist.erase(remove_if(begin(itemlist), end(itemlist), [&](auto& a) { return a->name == name; }), end(itemlist));
	}
	void RemoveItemById(int id)
	{
		itemlist.erase(remove_if(begin(itemlist), end(itemlist), [&](auto& a) { return a->id == id; }), end(itemlist));
	}
	void MergeItems(int id1, int id2, int newId)
	{
		auto it1 = find_if(begin(itemlist), end(itemlist), [&](auto& a) { return a->id == id1; });
		auto it2 = find_if(begin(itemlist), end(itemlist), [&](auto& a) { return a->id == id2; });
		if (it1 != end(itemlist) && it2 != end(itemlist) && (*it1)->grade == (*it2)->grade)
		{
			char newGrade = ((*it1)->grade == 'S') ? 'S' : (*it1)->grade - 1;
			auto newItem = make_shared<Item>(newId, (*it1)->name, 1, newGrade);
			cout << newId << ' ' << (*it1)->name << ' ' << 1 << ' ' << newGrade << ' ' << endl;
			itemlist.push_back(newItem);
			RemoveItemById(id1);
			RemoveItemById(id2);
		}
	}
	void PrintItems()
	{
		std::for_each(begin(itemlist), end(itemlist), [](auto& a) {  cout << a->id << " " << a->name << " " << a->grade << endl; });
		cout << endl;
	}
	void SortByName()
	{
		std::sort(begin(itemlist), end(itemlist), [](auto& a, auto& b) { return a->name < b->name;   });
	}
	void SortByLevel()
	{
		std::sort(begin(itemlist), end(itemlist), [](auto& a, auto& b) { return a->level < b->level;   });
	}
};

int main() {

	//Item 목록을 만들고, 동적할당 하세요.	
	//  포인터 배열		// Item* ItemList[N] = { ~~~ };
	//	포인터 동적할당	// Item** ItemList = new Item*[N];
	//	동적배열 		// vector<Item*> ItemList;
	//	스마트포인터 		// vector<unique_ptr<Item>> ItemList;

	// 아이템을 추가하고 아이템 목록을 출력하세요.
	//	1, 단검, 1, 'A'
	//	2, 단검, 2, 'A'
	//	3, 갑옷, 1, 'B'
	//	4, 반지, 2, 'B'
	//	5, 반지, 3, 'S'

	ItemManager itemManager;
	itemManager.AddItem(make_shared<Weapon>(1, "단검", 1, 'A'));
	itemManager.AddItem(make_shared<Weapon>(2, "단검", 2, 'A'));
	itemManager.AddItem(make_shared<Armor>(3, "갑옷", 1, 'B'));
	itemManager.AddItem(make_shared<Armor>(4, "반지", 2, 'B'));
	itemManager.AddItem(make_shared<Armor>(5, "반지", 3, 'S'));

	// 아이템 목록 출력
	itemManager.PrintItems();

	// 아이템에서 단검을 모두 제거하세요.
	itemManager.RemoveItemByName("단검");
	itemManager.PrintItems();

	// 아이템에서 단검을 다시 추가 하세요. 
	itemManager.AddItem(make_shared<Weapon>(6, "단검", 1, 'A'));
	itemManager.AddItem(make_shared<Weapon>(7, "단검", 2, 'B'));
	itemManager.AddItem(make_shared<Weapon>(8, "단검", 2, 'B'));

	// 아이템 목록 출력
	itemManager.PrintItems();

	// 이름순으로 정렬하여 출력하세요.
	itemManager.SortByName();
	itemManager.SortByLevel();
	itemManager.PrintItems();

	//같은 등급의 아이템 2개를 합성하여 상위등급의 아이템 하나로 만드는 시스템을 만드세요.
	//합성후 네임은 재료1을 따르며, 레벨은 1로 초기화되고, 재료가 되는 아이템은 삭제 됩니다.
	//합성 재료1 : 7
	//합성 재료2 : 8
	//합성 결과  : 9, 단검, 1, 'A'
	//합성 후 아이템 목록 출력
	itemManager.MergeItems(7, 8, 9);
	itemManager.PrintItems();
}

//ItemManager class 를 만들어 코드를 정리하세요.
//	STL, 스마트포인터 사용하세요.