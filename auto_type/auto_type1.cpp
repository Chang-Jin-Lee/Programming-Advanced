// auto_type_deduction_rules.cpp
// C++17 권장. (C++11/14와의 brace-init 차이를 전처리기로 표시)

#include <iostream>
#include <type_traits>
#include <initializer_list>
#include <array>
#include <string>
#include <utility>

using std::cout;
using std::endl;

// 도우미: 타입 동일성 검사 매크로 (에러 시 메시지로 구분)
#define STATIC_SAME(T1, T2) static_assert(std::is_same<T1, T2>::value, "type mismatch: " #T1 " vs " #T2)

void arrays_and_functions() {
	// 배열
	const char name[] = "abc"; // 길이 4 ('a','b','c','\0')

	auto  arr1 = name;   // const char*
	auto& arr2 = name;   // const char (&)[4]

	STATIC_SAME<decltype(arr1), const char*>();
	STATIC_SAME<decltype(arr2), const char(&)[4]>();

	// 함수
	void someFunc(int, double);
	auto  func1 = someFunc; // 함수 포인터
	auto& func2 = someFunc; // 함수 참조

	STATIC_SAME<decltype(func1), void(*)(int, double)>();
	STATIC_SAME<decltype(func2), void(&)(int, double)>();
}

void three_cases_and_refs() {
	int x = 27;
	const auto cx = x;     // const int
	const auto& rx = x;    // const int&

	STATIC_SAME<decltype(cx), const int>();
	STATIC_SAME<decltype(rx), const int&>();

	// 보편 참조
	auto&& uref1 = x;   // lvalue → int&
	auto&& uref2 = cx;  // lvalue → const int&
	auto&& uref3 = 27;  // rvalue → int&&

	STATIC_SAME<decltype(uref1), int&>();
	STATIC_SAME<decltype(uref2), const int&>();
	STATIC_SAME<decltype(uref3), int&&>();
}

void brace_init_rules() {
	auto x1 = 27;         STATIC_SAME<decltype(x1), int>();
	auto x2(27);          STATIC_SAME<decltype(x2), int>();

	auto x3 = { 27 };       STATIC_SAME<decltype(x3), std::initializer_list<int>>();

	auto x4{ 27 };
#if __cplusplus >= 201703L
	// C++17: 단일 원소 brace-init 은 스칼라로 추론
	STATIC_SAME<decltype(x4), int>();
#else
	// C++11/14: std::initializer_list<int>
	STATIC_SAME<decltype(x4), std::initializer_list<int>>();
#endif

	// 혼합 타입은 추론 실패 → 컴파일 에러
	// auto bad = {1, 2, 3.0}; // ERROR
}

template <typename T>
void accept_init_list(std::initializer_list<T> il) {
	// il: std::initializer_list<T>
	(void)il;
}

void init_list_with_template() {
	accept_init_list({ 11, 23, 9 }); // T=int
	// 일반 템플릿 매개변수 T로는 중괄호 초기치만으로는 추론 불가:
	// template <typename T> void f(T);
	// f({11, 23, 9}); // ERROR
}

// 보편 참조 + forward의 예시
template<typename T, typename U>
auto Plus(T&& lhs, U&& rhs) {
	return std::forward<T>(lhs) + std::forward<U>(rhs);
}

void forwarding_demo() {
	std::string A = "abc", B = "def";
	cout << "Plus(string,string) = " << Plus(A, B) << "\n"; // abcdef
	cout << "Plus(int,int)       = " << Plus(10, 20) << "\n"; // 30
}

int main() {
	three_cases_and_refs();
	arrays_and_functions();
	brace_init_rules();
	init_list_with_template();
	forwarding_demo();

	cout << "All static checks passed.\n";
	return 0;
}