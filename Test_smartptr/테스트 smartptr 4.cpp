//상속과 스마트 포인터

#include <iostream>
#include <memory>
#include <vector>
#include <algorithm>
using namespace std;

class Item : enable_shared_from_this<Item> {
public:
    string name;
    Item(string s) : name(s) { }
    virtual ~Item() {}
};

class Weapon : public Item {
public:
    Weapon(string s) : Item(s) { }
};

class Armor : public Item {
public:
    Armor(string s) : Item(s) { }
};

int main()
{
    // 벡터를 선언하고, Weapon, Weapon, Armor 객체의 포인터를 관리해보자.
    vector<Item*> assets;
    assets.push_back(new Weapon{ "Weapon1" });
    assets.push_back(new Weapon{ "Weapon2" });
    assets.push_back(new Armor{ "Armor1" });

    //delete assets[i]


    //-------------------------------------------------------------------------------
    // 스마트 포인터로 표현하세요. (shared_ptr)

    vector< shared_ptr<Item> > items;
	items.push_back(make_shared<Weapon>("Weapon1"));
	items.push_back(make_shared<Weapon>("Weapon2"));
	items.push_back(make_shared<Armor>("Armor1"));


    //-------------------------------------------------------------------------------
    // 아래 선언된 weapons 에, 'Weapon 형식'만 담아서 name 을 출력하세요.

    vector< shared_ptr<Item> > weapons;


    copy_if(items.begin(), items.end(), back_inserter(weapons), [](shared_ptr<Item> p) -> bool
		{
			shared_ptr<Weapon> temp = dynamic_pointer_cast<Weapon>(p);
			return temp.get() != nullptr;
		});

    for (const auto& p : weapons)
    {
        cout << p << endl;
        cout << (dynamic_pointer_cast<Weapon>(p))->name << endl;
    }

}
