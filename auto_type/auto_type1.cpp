// auto_type_deduction_rules.cpp
// C++17 ����. (C++11/14���� brace-init ���̸� ��ó����� ǥ��)

#include <iostream>
#include <type_traits>
#include <initializer_list>
#include <array>
#include <string>
#include <utility>

using std::cout;
using std::endl;

// �����: Ÿ�� ���ϼ� �˻� ��ũ�� (���� �� �޽����� ����)
#define STATIC_SAME(T1, T2) static_assert(std::is_same<T1, T2>::value, "type mismatch: " #T1 " vs " #T2)

void arrays_and_functions() {
	// �迭
	const char name[] = "abc"; // ���� 4 ('a','b','c','\0')

	auto  arr1 = name;   // const char*
	auto& arr2 = name;   // const char (&)[4]

	STATIC_SAME<decltype(arr1), const char*>();
	STATIC_SAME<decltype(arr2), const char(&)[4]>();

	// �Լ�
	void someFunc(int, double);
	auto  func1 = someFunc; // �Լ� ������
	auto& func2 = someFunc; // �Լ� ����

	STATIC_SAME<decltype(func1), void(*)(int, double)>();
	STATIC_SAME<decltype(func2), void(&)(int, double)>();
}

void three_cases_and_refs() {
	int x = 27;
	const auto cx = x;     // const int
	const auto& rx = x;    // const int&

	STATIC_SAME<decltype(cx), const int>();
	STATIC_SAME<decltype(rx), const int&>();

	// ���� ����
	auto&& uref1 = x;   // lvalue �� int&
	auto&& uref2 = cx;  // lvalue �� const int&
	auto&& uref3 = 27;  // rvalue �� int&&

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
	// C++17: ���� ���� brace-init �� ��Į��� �߷�
	STATIC_SAME<decltype(x4), int>();
#else
	// C++11/14: std::initializer_list<int>
	STATIC_SAME<decltype(x4), std::initializer_list<int>>();
#endif

	// ȥ�� Ÿ���� �߷� ���� �� ������ ����
	// auto bad = {1, 2, 3.0}; // ERROR
}

template <typename T>
void accept_init_list(std::initializer_list<T> il) {
	// il: std::initializer_list<T>
	(void)il;
}

void init_list_with_template() {
	accept_init_list({ 11, 23, 9 }); // T=int
	// �Ϲ� ���ø� �Ű����� T�δ� �߰�ȣ �ʱ�ġ�����δ� �߷� �Ұ�:
	// template <typename T> void f(T);
	// f({11, 23, 9}); // ERROR
}

// ���� ���� + forward�� ����
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