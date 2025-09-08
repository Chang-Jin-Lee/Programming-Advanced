// template_return_examples.cpp
// ���ø� �Լ� ���� Ÿ��: decltype, trailing return type, auto, decltype(auto), forwarding

#include <iostream>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

using namespace std;

//------------------------------------------------------------------------------
// 1) ��� �Ķ����(out parameter) Ÿ�� ����: decltype(t+u)*
//------------------------------------------------------------------------------
template <typename T, typename U>
void add_out(T t, U u, decltype(t + u)* result) {
	*result = t + u;
}

//------------------------------------------------------------------------------
// 2) ��ȯ Ÿ�� ����
//   - C++11: ���� ���� Ÿ�� (t,u �� �Ķ���� �ڿ��� ����)
//------------------------------------------------------------------------------
template <typename T, typename U>
auto add_trailing(T t, U u) -> decltype(t + u) {
	return t + u;
}

//   - C++14: auto �� �� Ÿ�� ��ȯ
template <typename T, typename U>
auto add_auto_value(T t, U u) {
	return t + u; // ���� ������ ����� ��
}

//   - C++14: decltype(auto) �� ����/const ����
template <typename T, typename U>
decltype(auto) add_decltype_auto(T&& t, U&& u) {
	// ����: (t+u)�� ������ ��ȯ�ϴ� �����̶�� ���� ����
	return (t + u);
}

//------------------------------------------------------------------------------
// 3) ���� ���� + forward
//------------------------------------------------------------------------------
template<typename T, typename U>
auto Plus(T&& lhs, U&& rhs) {
	return std::forward<T>(lhs) + std::forward<U>(rhs);
}

//------------------------------------------------------------------------------
// 4) auto ��ȯ�� ���� vs decltype(auto)
//------------------------------------------------------------------------------
class C {
	int arr_[10] = { 1,2,3,4,5,6,7,8,9,10 };
public:
	int& operator[](int i) { return arr_[i]; }
};

template<typename T>
auto changearr(T& c, int index) {      // auto: �� ����
	return c[index];                   // int& -> int �� ����Ǿ� ���� �ҽ�
}

template<typename T>
decltype(auto) GetArr(T&& c, int index) { // decltype(auto): ���� ����
	return std::forward<T>(c)[index];     // int& ����
}

//------------------------------------------------------------------------------
// 5) ����
//------------------------------------------------------------------------------
int main() {
	// (1) ��� �Ķ����
	{
		int    i = 10;
		double d = 2.5;
		auto   r = 0.0; // double
		add_out(i, d, &r); // decltype(i+d) == double
		cout << "[add_out] " << r << "\n";
	}

	// (2) ��ȯ Ÿ�� (���� ���� Ÿ�� / auto / decltype(auto))
	{
		cout << "[add_trailing] " << add_trailing(10, 20.5) << "\n"; // double
		cout << "[add_auto_value] " << add_auto_value(10, 20) << "\n";  // int
		// add_decltype_auto �� ������ ������ �� �ǹ̰� ũ��.
		// ���⼭�� (t+u)�� prvalue�� ������ �߷е�.
		cout << "[add_decltype_auto] " << add_decltype_auto(1, 2) << "\n"; // int
	}

	// (3) ���� ���� + forward
	{
		string A("abc"), B("def");
		cout << "[Plus string] " << Plus(A, B) << "\n"; // "abcdef"
		cout << "[Plus int]    " << Plus(10, 20) << "\n"; // 30
	}

	// (4) auto vs decltype(auto) ��ȯ ����
	{
		C cls;
		auto i = changearr(cls, 3); // �� ����(int)
		i = 100;                    // cls[3] �� �ٲ��� ����
		cout << "[changearr] cls[3] = " << cls[3] << " (unchanged)\n";

		GetArr(cls, 3) = 100;       // decltype(auto): int& �� ���� ���� ����
		cout << "[GetArr]   cls[3] = " << cls[3] << " (modified)\n";
	}

	// (5) vector ����: operator[] �� ������ ��ȯ
	{
		vector<int> v{ 10, 20 };
		auto a = v[0];          // int (����)
		decltype(v[0]) r = v[0]; // int& (����)
		r = 77;                 // v[0] ���� ����
		cout << "[vector[]] a=" << a << ", v[0]=" << v[0] << "\n";
	}

	return 0;
}