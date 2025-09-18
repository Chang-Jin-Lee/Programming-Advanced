// temporary_and_value_categories.cpp
// g++ -std=c++17 temporary_and_value_categories.cpp

#include <iostream>
#include <string>
#include <utility>
#include <type_traits>
#include <vector>
using namespace std;

//-------------------------------------
// 1) 임시(익명) 객체 데모
//-------------------------------------
struct A {
	void print() const { cout << "Hello\n"; }
};

void demo_temporaries() {
	int n(1);          // 이름 있는 객체
	(void)n;

	int(1);            // 임시 int
	A a; a.print();    // 이름 있는 A
	A();               // 익명 A (즉시 소멸)
	A().print();       // 익명 A로 멤버 호출

	// decltype / auto 와 임시
	using T1 = decltype(A()); // prvalue → A
	T1 obj{};                 // A
	auto obj1 = A();          // A
	auto&& obj2 = A();        // A&& (임시에 바인딩)

	// 주의: 아래는 타입은 A&& 지만 변수 선언에 사용할 수 없음(바인딩 대상 없음) → 주석
	// decltype(std::move(obj)) obj3; // A&& → 불가(초기화 필요)
	(void)obj; (void)obj1; (void)obj2;
}

//-------------------------------------
// 2) Cents: 익명 객체 반환/사용
//-------------------------------------
struct Cents {
	int m{};
	explicit Cents(int c) : m(c) {}
	int get() const { return m; }
};

Cents add(const Cents& c1, const Cents& c2) {
	return Cents(c1.get() + c2.get()); // 익명 객체 반환
}

void demo_cents() {
	cout << add(Cents(6), Cents(8)).get() << "\n"; // 익명 객체 즉시 사용
}

//-------------------------------------
// 3) 값 카테고리 기초
//-------------------------------------
void demo_value_categories() {
	int i = 0, j = 0;
	int* p = &i;

	i = 7;      // OK: i는 lvalue, 7은 prvalue

	// 잘못된 사례(컴파일 오류) → 주석
	// 7 = i;       // 좌변은 lvalue여야 함
	// j * 4 = 7;   // j*4 는 prvalue

	*p = i;      // 역참조는 lvalue

	((i < 3) ? i : j) = 7; // 조건연산자: 두 피연산자가 같은 lvalue 타입 → lvalue

	const int ci = 7;
	// ci = 9;     // 오류: const lvalue는 수정 불가
}

//-------------------------------------
// 4) 포인터 반환과 lvalue
//-------------------------------------
int  NextVal_1(int* p) { return *(p + 1); } // 값 반환 → prvalue
int* NextVal_2(int* p) { return (p + 1); }  // 포인터 반환

void demo_nextval() {
	int a[] = { 1,2,3,4,5 };
	// NextVal_1(a) = 9; // 오류: prvalue를 좌변에 둘 수 없음
	*NextVal_2(a) = 9;   // OK: 역참조는 lvalue → a[1]=9
	cout << "a[1]=" << a[1] << "\n";
}

//-------------------------------------
// 5) rvalue 참조, move
//-------------------------------------
void demo_rvalue_and_move() {
	int a = 0;
	int& lref = a;         (void)lref;   // lvalue 참조
	int&& rref = 0;         (void)rref;   // rvalue 참조

	const int& cr1 = a;     (void)cr1;    // const lvalue 참조
	const int& cr2 = 0;     (void)cr2;    // 임시에도 바인딩 가능

	// int&& bad = a;      // 오류: lvalue에 직접 rvalue 참조 불가
	int&& good = std::move(a); (void)good; // lvalue → xvalue
}

//-------------------------------------
// 6) 오버로딩: &, &&, const&
//-------------------------------------
struct B {};

void foo(B&) { cout << "&\n"; }
void foo(B&&) { cout << "&&\n"; }
void foo(const B&) { cout << "const &\n"; }
// void foo(const B&&) { cout << "const &&\n"; } // 보통 불필요/지양

void demo_overload_cat() {
	B o;
	foo(o);        // &
	foo(B());      // &&
	const B co{};
	foo(co);       // const &
	foo(std::move(co)); // const &가 매칭(상수는 이동 불가)
}

//-------------------------------------
// 7) 멤버 함수 ref-qualifier
//-------------------------------------
struct RefQ {
	std::string s = "abc";
	std::string& get()& { cout << "&\n";        return s; }
	std::string        get()&& { cout << "&&\n";       return std::move(s); }
	const std::string& get() const& { cout << "const &\n";  return s; }
	std::string        get() const&& { cout << "const &&\n"; return s; }
};

void demo_ref_qualifiers() {
	RefQ a1; a1.get();
	const RefQ a2{}; a2.get();
	RefQ().get();
	const RefQ a3{}; std::move(a3).get();
}

//-------------------------------------
// 8) C++17 임시 실체화(prvalue 변화)
//-------------------------------------
struct NonCopyNonMove {
	NonCopyNonMove() = default;
	NonCopyNonMove(const NonCopyNonMove&) = delete;
	NonCopyNonMove(NonCopyNonMove&&) = delete;
};

NonCopyNonMove make_obj() {
	return NonCopyNonMove{}; // C++17: 여기서 개념적 임시 없음(문맥에서 직접 초기화)
}

void demo_temp_materialization() {
	auto x = make_obj(); // C++17 이전: 오류 가능(복사/이동 필요)
	// C++17 이후: OK (직접 초기화)
	(void)x;
}

//-------------------------------------
// 9) 호출 대상에 따른 오버로드 선택 예
//-------------------------------------
struct X {};
void f(const X&) { cout << "f(const&)\n"; }
void f(X&&) { cout << "f(&&)\n"; }

void demo_value_category_calls() {
	X v;       f(v);            // const&
	const X c; f(c);            // const&
	f(X());                     // (C++17 materialized as xvalue) → && 오버로드가 더 적합
	f(std::move(v));            // && (xvalue)
}

//-------------------------------------
int main() {
	demo_temporaries();
	demo_cents();
	demo_value_categories();
	demo_nextval();
	demo_rvalue_and_move();
	demo_overload_cat();
	demo_ref_qualifiers();
	demo_temp_materialization();
	demo_value_category_calls();
	return 0;
}