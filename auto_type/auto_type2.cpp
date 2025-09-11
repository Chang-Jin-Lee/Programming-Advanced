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
	// 1) �ʱ�ȭ ����
	// int x1;      // �ʱ�ȭ �� �� �� ���� (���/���� ����)
	// auto x2;     // ����: auto�� �ݵ�� �ʱ�ȭ �ʿ�
	auto x3 = 0;    // OK

	// 2) ��Ȯ�� Ÿ�� ����: size_type
	vector<int> v;
	auto sz = v.size(); // ��Ȯ�� vector<int>::size_type �� �߷�
	CHECK_SAME<decltype(sz), vector<int>::size_type>();
	(void)x3; (void)sz;
}

void less_typing_readability() {
	unordered_map<string, int> m{ {"a",1}, {"b",2} };

	// ��Ȳ�� ����
	for (const pair<string, int>& p : m) {
		(void)p;
	}
	// ������ ����
	for (const auto& p : m) {
		(void)p;
	}
}

template <typename It>
void dwim_v1(It b, It e) {
	for (; b != e; ++b) {
		// value_type�� ���� ���� ��Ȳ
		using VT = typename std::iterator_traits<It>::value_type;
		VT curr = *b;
		(void)curr;
	}
}

template <typename It>
void dwim_v2(It b, It e) {
	for (; b != e; ++b) {
		// auto�� ����
		auto curr = *b;
		(void)curr;
	}
}

void universal_reference_cases() {
	int x = 27;
	const int cx = x;

	auto&& u1 = x;   // lvalue �� int&
	auto&& u2 = cx;  // lvalue �� const int&
	auto&& u3 = 27;  // rvalue �� int&&

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
	auto  f1 = someFunc; // �Լ� ������
	auto& f2 = someFunc; // �Լ� ����
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

	// ȥ�� Ÿ���� ����:
	// auto bad = {1, 2, 3.0}; // ������ ����
	(void)x1; (void)x2; (void)x3; (void)x4;
}

void lambda_and_auto() {
	// ���� Ÿ���� ���� �� auto�� �޴� ���� �ڿ������� ����
	auto cmp = [](const unique_ptr<int>& a, const unique_ptr<int>& b) {
		return *a < *b;
		};

	vector<unique_ptr<int>> vp;
	vp.emplace_back(make_unique<int>(3));
	vp.emplace_back(make_unique<int>(1));
	vp.emplace_back(make_unique<int>(2));
	sort(vp.begin(), vp.end(), cmp);

#if __cplusplus >= 201402L
	// ���׸� ����(C++14~): �Ķ���Ϳ��� auto
	auto cmp_any = [](const auto& a, const auto& b) {
		return *a < *b;
		};
	sort(vp.begin(), vp.end(), cmp_any);
#endif
}

void const_and_refs_rules() {
	const int ci = 42;
	auto a = ci;                 // top-level const ���� �� int
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
	auto proxy = bits[0];   // proxy Ÿ�� (vector<bool>::reference)
	bool val = bits[1];    // �� ����
	auto& ref = bits[2];    // proxy ����

	// �ǵ��� ���� ����:
	// - proxy �״�� ���� ��Ʈ �ø� �� ���� �ǹ� ����
	// - bool ���� �ʿ��ϸ� bool�� ����
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