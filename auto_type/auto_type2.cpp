// auto_usage_guide.cpp
// g++ -std=c++17 auto_usage_guide.cpp

#include <iostream>
#include <string>
#include <vector>
#include <array>
#include <unordered_map>
#include <type_traits>
#include <initializer_list>
#include <memory>
#include <algorithm>

using namespace std;

#define CHECK_SAME(T1, T2) static_assert(std::is_same_v<T1, T2>, "type mismatch: " #T1 " vs " #T2)

void init_and_exact_types() {
	// 1) 초기화 강제
	// int x1;      // 초기화 안 할 수 있음 (경고/버그 소지)
	// auto x2;     // 오류: auto는 반드시 초기화 필요
	auto x3 = 0;    // OK

	// 2) 정확한 타입 추종: size_type
	vector<int> v;
	auto sz = v.size(); // 정확히 vector<int>::size_type 로 추론
	CHECK_SAME<decltype(sz), vector<int>::size_type>();
	(void)x3; (void)sz;
}

void less_typing_readability() {
	unordered_map<string, int> m{ {"a",1}, {"b",2} };

	// 장황한 선언
	for (const pair<string, int>& p : m) {
		(void)p;
	}
	// 간결한 선언
	for (const auto& p : m) {
		(void)p;
	}
}

template <typename It>
void dwim_v1(It b, It e) {
	for (; b != e; ++b) {
		// value_type을 직접 쓰면 장황
		using VT = typename std::iterator_traits<It>::value_type;
		VT curr = *b;
		(void)curr;
	}
}

template <typename It>
void dwim_v2(It b, It e) {
	for (; b != e; ++b) {
		// auto로 간결
		auto curr = *b;
		(void)curr;
	}
}

void universal_reference_cases() {
	int x = 27;
	const int cx = x;

	auto&& u1 = x;   // lvalue → int&
	auto&& u2 = cx;  // lvalue → const int&
	auto&& u3 = 27;  // rvalue → int&&

	CHECK_SAME<decltype(u1), int&>();
	CHECK_SAME<decltype(u2), const int&>();
	CHECK_SAME<decltype(u3), int&&>();
	(void)u1; (void)u2; (void)u3;
}

void decay_examples() {
	const char name[] = "abc";
	auto  arr1 = name;  // const char*
	auto& arr2 = name;  // const char(&)[4]
	CHECK_SAME<decltype(arr1), const char*>();
	CHECK_SAME<decltype(arr2), const char(&)[4]>();

	void someFunc(int, double);
	auto  f1 = someFunc; // 함수 포인터
	auto& f2 = someFunc; // 함수 참조
	CHECK_SAME<decltype(f1), void(*)(int, double)>();
	CHECK_SAME<decltype(f2), void(&)(int, double)>();
	(void)f1; (void)f2;
}

void brace_init_rules() {
	auto x1 = 27;   CHECK_SAME<decltype(x1), int>();
	auto x2(27);    CHECK_SAME<decltype(x2), int>();
	auto x3 = { 27 }; CHECK_SAME<decltype(x3), std::initializer_list<int>>();

	auto x4{ 27 };
#if __cplusplus >= 201703L
	CHECK_SAME<decltype(x4), int>(); // C++17: int
#else
	CHECK_SAME<decltype(x4), std::initializer_list<int>>(); // C++11/14
#endif

	// 혼합 타입은 오류:
	// auto bad = {1, 2, 3.0}; // 컴파일 오류
	(void)x1; (void)x2; (void)x3; (void)x4;
}

void lambda_and_auto() {
	// 람다 타입은 무명 → auto로 받는 것이 자연스럽고 빠름
	auto cmp = [](const unique_ptr<int>& a, const unique_ptr<int>& b) {
		return *a < *b;
		};

	vector<unique_ptr<int>> vp;
	vp.emplace_back(make_unique<int>(3));
	vp.emplace_back(make_unique<int>(1));
	vp.emplace_back(make_unique<int>(2));
	sort(vp.begin(), vp.end(), cmp);

#if __cplusplus >= 201402L
	// 제네릭 람다(C++14~): 파라미터에도 auto
	auto cmp_any = [](const auto& a, const auto& b) {
		return *a < *b;
		};
	sort(vp.begin(), vp.end(), cmp_any);
#endif
}

void const_and_refs_rules() {
	const int ci = 42;
	auto a = ci;                 // top-level const 제거 → int
	const auto ac = ci;          // const int
	auto& ar = a;                // int&
	const auto& cr = ci;         // const int&
	CHECK_SAME<decltype(a), int>();
	CHECK_SAME<decltype(ac), const int>();
	CHECK_SAME<decltype(ar), int&>();
	CHECK_SAME<decltype(cr), const int&>();
	(void)a; (void)ac; (void)ar; (void)cr;
}

void vector_bool_note() {
	vector<bool> bits = { true, false, true };
	auto proxy = bits[0];   // proxy 타입 (vector<bool>::reference)
	bool val = bits[1];    // 값 복사
	auto& ref = bits[2];    // proxy 참조

	// 의도에 따라 선택:
	// - proxy 그대로 쓰면 비트 플립 등 참조 의미 유지
	// - bool 값이 필요하면 bool로 복사
	proxy = false;
	ref = false;
	(void)val;
}

template<typename T, typename U>
auto Plus(T&& lhs, U&& rhs) {
	return std::forward<T>(lhs) + std::forward<U>(rhs);
}

void forwarding_demo() {
	string A = "abc", B = "def";
	cout << "Plus(string,string) = " << Plus(A, B) << "\n"; // abcdef
	cout << "Plus(int,int)       = " << Plus(10, 20) << "\n"; // 30
}

int main() {
	init_and_exact_types();
	less_typing_readability();
	universal_reference_cases();
	decay_examples();
	brace_init_rules();
	lambda_and_auto();
	const_and_refs_rules();
	vector_bool_note();
	forwarding_demo();

	cout << "auto usage guide: OK\n";
	return 0;
}