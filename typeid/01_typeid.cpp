// typeid_examples.cpp

#include <iostream>
#include <typeinfo>
using namespace std;

//----------------------
// (1) 기본 사용 예제
//----------------------
void basicExample() {
	cout << typeid(int).name() << endl;
	cout << typeid(type_info).name() << endl;
	cout << typeid(basicExample).name() << endl;
	cout << typeid(cout).name() << endl;
}

//----------------------
// (2) 다형성 구분
//----------------------
class A { public: virtual void vf() {} };
				class B : public A {};
				class C : public A {};
				class D : public A {};

void polymorphismExample() {
	B b; C c; D d;
	A* arr[] = { &b, &c, &d };
	cout << typeid(*arr[0]).name() << endl;
	cout << typeid(*arr[1]).name() << endl;
	cout << typeid(*arr[2]).name() << endl;
}

//----------------------
// (3) 안전한 형변환
//----------------------
class B2 : public A { public: bool b; };
class C2 : public A { public: int c; };

void memberPrint(A* a) {
	if (typeid(B2) == typeid(*a)) {
		B2* b = (B2*)a;
		cout << typeid(b->b).name() << endl;
	}
	else if (typeid(C2) == typeid(*a)) {
		C2* c = (C2*)a;
		cout << typeid(c->c).name() << endl;
	}
}

//----------------------
// (4) 템플릿 타입 확인
//----------------------
template <typename T>
T mymax(T a, T b) {
	cout << typeid(T).name() << endl;
	return (a > b ? a : b);
}

//----------------------
// (5) 참조 타입 비교
//----------------------
void refCheck() {
	bool rt = typeid(int) == typeid(int&);
	cout << rt << endl;              // true
	cout << typeid(int&).name() << endl;
}

class Base
{
public:
	virtual ~Base() {}
};

class Derived1 : public Base {};
class Derived2 : public Base {};

void F(Base* pB)
{
	cout << endl << typeid(pB).name() << endl;
	if (dynamic_cast<Derived2*>(pB)) cout << "Yes" << endl;
	else cout << "No" << endl;
}

//----------------------
// main
//----------------------
int main() {
	basicExample();
	polymorphismExample();

	B2 b; C2 c;
	A* arr[] = { &b, &c };
	memberPrint(arr[0]);
	memberPrint(arr[1]);

	mymax(3, 5);
	mymax(3.14, 2.71);

	refCheck();


	Base* pB = new Derived2;
	Base* pD = new Derived1;

	F(pB);
	F(pD);

	return 0;
}
