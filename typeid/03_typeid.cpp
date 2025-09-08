// rtti_traits_examples.cpp
// RTTI (typeid, dynamic_cast) & 타입 Traits (is_base_of_v, is_same_v)

#include <iostream>
#include <typeinfo>
#include <type_traits>
using namespace std;

//---------------------------
// (1) RTTI with typeid & dynamic_cast
//---------------------------
class Base {
public:
	virtual ~Base() {}
};

class Derived1 : public Base {};
class Derived2 : public Base {};

void F(Base* pB) {
	// typeid 이용
	if (typeid(*pB) == typeid(Derived2)) {
		cout << typeid(*pB).name() << endl;
	}
	else {
		cout << "No!" << endl;
	}

	// dynamic_cast 이용
	if (dynamic_cast<Derived2*>(pB)) {
		cout << typeid(*pB).name() << endl;
	}
	else {
		cout << "No!" << endl;
	}
}

int main() {
	Base* pB = new Derived2;
	Base* pD = new Derived1;

	// Derived2 가 아니면 "No" 출력
	F(pB); // Derived2
	F(pD); // No!

	delete pB;
	delete pD;
	return 0;
}

//---------------------------
// (2) is_base_of_v
//---------------------------
class A {};
class B : public A {};
class C : private A {};

static_assert(std::is_base_of_v<B, B> == true);      // 동일 클래스
static_assert(std::is_base_of_v<int, int> == false); // 기본 자료형
static_assert(std::is_base_of_v<A, B> == true);      // public 상속
static_assert(std::is_base_of_v<A, C> == true);      // private 상속

//---------------------------
// (3) is_same_v
//---------------------------
class P1 {};
class P2 : public P1 {};

int main_is_same() {
	cout << is_same_v<P1, P1> << endl; // true
	cout << is_same_v<P1, P2> << endl; // false

	P1 p1; P2 p2;
	cout << is_same_v<decltype(p1), decltype(p1)> << endl; // true
	cout << is_same_v<decltype(p1), decltype(p2)> << endl; // false
	return 0;
}