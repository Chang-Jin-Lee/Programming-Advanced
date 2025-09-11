// typeid_rtti_examples.cpp
// typeid �����ڸ� ����ϸ� ��Ÿ�ӿ� ��ü�� ������ Ȯ���� �� �ִ�.
// RTTI(Run-Time Type Info)�� �����ϴ� Ű����: dynamic_cast, typeid

#include <iostream>
#include <typeinfo>
using namespace std;

class Base {
public:
	virtual void vvfunc() {} // ������ ������ ���� virtual �ʿ�
};

class Derived : public Base {};

int main()
{
	// ------------------------------
	// (1) �⺻ �ڷ���
	// ------------------------------
	cout << typeid(char).name() << endl;
	cout << typeid(int).name() << endl;
	cout << typeid(__int64).name() << endl;

	// ------------------------------
	// (2) const, volatile, ������ ���õ�
	// ------------------------------
	cout << typeid(char&).name() << endl;         // char
	cout << typeid(const char).name() << endl;    // char
	cout << typeid(volatile char).name() << endl; // char

	// ------------------------------
	// (3) �����ʹ� const, volatile ����
	// ------------------------------
	cout << typeid(char*).name() << endl;
	cout << typeid(const char*).name() << endl;
	cout << typeid(volatile char*).name() << endl;

	// ------------------------------
	// (4) Ŭ���� Ÿ��
	// ------------------------------
	cout << typeid(Base).name() << endl;     // class Base
	cout << typeid(Derived).name() << endl;  // class Derived

	// ------------------------------
	// (5) ������ Ȯ��
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