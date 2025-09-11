// typeid_rtti_examples.cpp
// typeid 연산자를 사용하면 런타임에 객체의 형식을 확인할 수 있다.
// RTTI(Run-Time Type Info)를 제공하는 키워드: dynamic_cast, typeid

#include <iostream>
#include <typeinfo>
using namespace std;

class Base {
public:
	virtual void vvfunc() {} // 다형성 구분을 위해 virtual 필요
};

class Derived : public Base {};

int main()
{
	// ------------------------------
	// (1) 기본 자료형
	// ------------------------------
	cout << typeid(char).name() << endl;
	cout << typeid(int).name() << endl;
	cout << typeid(__int64).name() << endl;

	// ------------------------------
	// (2) const, volatile, 참조는 무시됨
	// ------------------------------
	cout << typeid(char&).name() << endl;         // char
	cout << typeid(const char).name() << endl;    // char
	cout << typeid(volatile char).name() << endl; // char

	// ------------------------------
	// (3) 포인터는 const, volatile 구분
	// ------------------------------
	cout << typeid(char*).name() << endl;
	cout << typeid(const char*).name() << endl;
	cout << typeid(volatile char*).name() << endl;

	// ------------------------------
	// (4) 클래스 타입
	// ------------------------------
	cout << typeid(Base).name() << endl;     // class Base
	cout << typeid(Derived).name() << endl;  // class Derived

	// ------------------------------
	// (5) 다형성 확인
	// ------------------------------
	Derived* pd = new Derived;
	Base* pb = pd;

	cout << typeid(pb).name() << endl;   // class Base*
	cout << typeid(*pb).name() << endl;  // class Derived
	cout << typeid(pd).name() << endl;   // class Derived*
	cout << typeid(*pd).name() << endl;  // class Derived

	delete pd;
	return 0;
}