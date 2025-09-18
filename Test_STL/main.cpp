//---------------------------------------------------------------------------------
// B 클래스인지 검사해서 맞다면 맴버 값을 출력하는 코드를 작성하세요.
//#include <iostream>
//
//struct A { virtual void vf() {} };
//struct B : public A { int m = 100; };
//
//void memberPrint(A* a)
//{
//	if (B* b = dynamic_cast<B*>(a))
//	{
//		std::cout << b->m << "\n";
//	}
//	else
//	{
//		std::cout << "Not B class\n";
//	}
//}
//
//int main() {
//	A a;
//	B b;
//	memberPrint(&a);
//	memberPrint(&b);
//}


//#include <iostream>
//
//class A {
//public:
//	A() { std::cout << "A()\n"; }
//	A(const A& a) { std::cout << "A() copy\n"; }
//	A(A&& a) { std::cout << "A() move\n"; }
//};
//
//class B {
//public:
//	B(A a) : m_a(std::move(a)) {  }
//	A m_a;
//};
//
//int main() {
//	A a;
//	std::cout << "create B \n";
//	B b(std::move(a));							//???
//}
//


//#include <iostream>
//
//class A {};
//
//void foo(A& a) { std::cout << "A&" << std::endl; }
//void foo(const A& a) { std::cout << "const A&" << std::endl; }
//void foo(A&& a) { std::cout << "A&&" << std::endl; }
//
//template <typename T>
//void wrapper(T&& u) {		
//
//	//foo(u);
//	foo(std::forward<T>( u ));	
//}
//
//template <typename T>
//void wrapper2(T&& u) {
//
//	//foo(u);
//	foo(std::move<T>(u));
//}
//
//int main() {
//	A a;
//	const A ca;
//
//	//std::cout << "원본 --------" << std::endl;
//	//foo(a);
//	//foo(ca);
//	//foo(A());		//
//
//	std::cout << "Wrapper -----" << std::endl;
//	wrapper(a);
//	wrapper(ca);
//	wrapper(A());	//
//
//	std::cout << "move -----" << std::endl;
//	wrapper2(a);
//	wrapper2(ca);
//	//wrapper2(A());	//
//}


// 20250918
//#include <iostream>
//using namespace std;
//
//class Test
//{
//	int num;
//public:
//	Test() : num(0) { cout << "Test(): " << num << endl; }
//	Test(int num) : num(num) { cout << "Test(int): " << num << endl; }
//	void Print() { cout << num << endl; }
//};
//
//int main()
//{
//	// 동적 배열
//	Test* pTest = new Test[3]{ Test(1), Test(2), Test(3) };
//	for (int i = 0; i < 3; i++) pTest[i].Print();
//	delete[] pTest;
//
//	// 정적 배열
//	Test* pTest2[3] = {new Test(1), new Test(2), new Test(3)};
//	for (int i = 0; i < 3; i++) pTest2[i]->Print();
//	for (int i = 0; i < 3; i++) pTest2[i];
//
//
//
//	Test* ptest1 = new Test;
//	Test* ptest2 = new Test(10);
//	delete ptest1;
//	delete ptest2;
//
//	Test* plist = new Test[3]{ Test(1), Test(2),Test(3) };
//	for (size_t i = 0; i < 3; i++)
//	{
//		plist[i].Print();
//	}
//	delete[] plist;
//
//
//	Test** plist2 = new Test * [3] { new Test(4), new Test(5), new Test(6) };
//	for (size_t i = 0; i < 3; i++)
//	{
//		plist2[i]->Print();
//	}
//	delete plist2[0];
//	delete plist2[1];
//	delete plist2[2];
//	delete[] plist2;
//
//	return 0;
//}

//#include <iostream>
//#include <memory>
//
//class A 
//{
//	int* data;
//public:
//	A() { data = new int[100]; }
//	~A() { delete[] data; }
//	void some() { std::cout << "some()" << std::endl; }
//};
//
//template<typename T>
//class SPtr
//{
//	T* data;
//public:
//	SPtr(T* d) : data(d) {}
//	~SPtr() { delete data; }
//	SPtr(const SPtr& d) = delete;
//	T& operator*() const { return *data; }
//	T* operator->() const { return data; }
//};
//
//
//void ex()
//{
//	SPtr<A> sp(new A());
//	sp->some();
//
//	// 복사를 막아야함
//	//SPtr<A> sp2(sp);
//}
//
//int main() { ex(); }




// #include <iostream>
// #include <memory>                       // #include <memory> 
// #include <vector>
// #include <algorithm>
// using namespace std;
// 
// 
// class Player {
// public:
// 	Player() : name("") {}
// 	Player(string n) : name(n) {}
// 	string name;
// };
// 
// int main()
// {
// 	// Player* [3] // p1,p2,p3;
// 	vector<unique_ptr<Player>> players;
// 	players.emplace_back(make_unique<Player>("1"));
// 	players.emplace_back(make_unique<Player>("2"));
// 	players.emplace_back(make_unique<Player>("3"));
// 
// 	for (auto& player : players) cout << player->name << endl;
// 
// 	for_each(players.begin(), players.end(), [](const unique_ptr<Player>& player) { cout << player->name << endl; });
// }





//#include <iostream>
//#include <memory>
//#include <vector>
//using namespace std;
//
//class Player {
//public:
//	Player() : name("") {}
//	Player(string n) : name(n) {}
//	virtual ~Player() { cout << "~Player()" << '\n'; }  //가상함수
//	string name;
//};
//
//class Hero : public Player {
//public:
//	Hero(string n) : Player(n) {}
//};
//
//class Enemy : public Player {
//public:
//	Enemy(string n) : Player(n) {}
//};
//
//template<typename Ts>
//auto makePlayer(Ts&& params)             //팩토리 함수
//{
//	auto myDeleter = [](Player* pPlayer) { delete pPlayer; };
//
//	std::unique_ptr<Player, decltype(myDeleter)> pPlayer(nullptr, myDeleter);
//
//	if (params == "hero") {
//		pPlayer.reset(new Hero(std::forward<Ts>(params)));
//	}
//	else {
//		pPlayer.reset(new Enemy(std::forward<Ts>(params)));
//	}
//	return pPlayer;
//}
//
//int main() {
//
//	//std::unique_ptr<Player> pPlayer(nullptr);
//	//pPlayer.reset(new Hero("hero") );
//
//	auto hero = makePlayer<string>("hero");
//	cout << hero->name << '\n';
//
//	auto enemy = makePlayer<string>("enemy");
//	cout << enemy->name << '\n';
//
//}


#include <iostream>
#include <memory>
#include <vector>
using namespace std;
class A
{

};

int main()
{
	shared_ptr<A> pA = make_shared<A>();
	shared_ptr<A> pB = pA;

}