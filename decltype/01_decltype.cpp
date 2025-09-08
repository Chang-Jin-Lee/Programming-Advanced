// decltype_examples.cpp
#include <iostream>
#include <utility>
using namespace std;

struct A { double d; };

//---------------------------
// (1) decltype 기본
//---------------------------
void basicExample() {
	int a = 3;
	decltype(a) b = 2;   // int

	int& ra = a;
	decltype(ra) rb = a; // int&

	int&& x = 3;
	decltype(x) y = 2;   // int&&

	A* aa;
	decltype(aa->d) dd = 0.1; // double

	int c = 3;
	decltype(c) c1 = 2;   // int
	decltype((c)) c2 = c; // int&

}

//---------------------------
// (2) decltype 규칙 (value category)
//---------------------------
void valueCategory() {
	int a, b;
	decltype(a + b) c = 0; // int (prvalue)
	decltype(a = b) d = a; // int& (lvalue)
}

//---------------------------
// (3) auto vs decltype
//---------------------------
void autoVsDecltype() {
	const int i = 4;
	auto j = i;       // int
	decltype(i) k = i; // const int

	int arr[10];
	auto arr2 = arr;     // int*
	decltype(arr) arr3;  // int[10]
}

//---------------------------
// (4) declval + decltype
//---------------------------
template <typename T>
decltype(std::declval<T>().f()) call_f_and_return(T& t) {
	return t.f();
}

struct B1 { int f() { return 1; } };
struct B2 { B2(int) {} int f() { return 2; } };

//---------------------------
// (5) decltype(auto)
//---------------------------
template <typename T>
decltype(auto) call_f_and_return2(T& t) {
	return t.f();
}

//---------------------------
// main
//---------------------------
int main() {
	basicExample();
	valueCategory();
	autoVsDecltype();

	B1 b1; B2 b2(1);
	int n1 = call_f_and_return(b1);
	int n2 = call_f_and_return2(b2);
	cout << n1 << " " << n2 << endl; // 1 2


	//const A* aa = new A();
	//decltype((aa->d)) c2; // int&
}