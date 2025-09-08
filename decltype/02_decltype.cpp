// decltype_deep_dive.cpp
// ������ Ÿ�� Ÿ�� ����: decltype / declval / decltype(auto) ���� ����

#include <iostream>
#include <type_traits>
#include <utility>
#include <vector>

using std::cout;
using std::endl;

// ���� ����: ���� ���̵� decltype �뵵�θ� ��� ���� (ȣ�� X)
const int&& fx();

// ���� A: ����/����/����������, ��� ����, ��ȣ ����
namespace section_a {
	struct A { double d; };

	void run() {
		int a = 3;
		decltype(a)   b = 2;   // int
		int& ra = a;
		decltype(ra)  rb = a;  // int&
		int&& x = 3;
		decltype(x)   y = 2;   // int&&

		A* aa = nullptr;
		decltype(aa->d) dd = 0.1; // double

		int c = 3;
		decltype(c)   c1 = 2;   // int
		decltype((c)) c2 = c;   // int&

		(void)b; (void)rb; (void)y; (void)dd; (void)c1; (void)c2;
	}
} // namespace section_a

// ���� B: �Լ�/�Լ��� decltype
namespace section_b {
	void run() {
		// fx�� "const int&& fx();" �� ����Ǿ� ����
		// �Լ� ȣ�� ǥ������ Ÿ��
		decltype(fx())  rref_value = 42; // const int&&
		(void)rref_value;

		// �Լ� ��ü�� Ÿ��
		decltype(fx) func_like; // const int&& ()

		// ����: func_like�� ȣ���ϸ� ���ǰ� �����Ƿ� ��ũ ����.
		(void)func_like;
	}
} // namespace section_b

// ���� C: auto vs decltype
namespace section_c {
	void run() {
		const int i = 4;
		auto j = i;        // int (const Ż��)
		decltype(i) k = i; // const int (����)
		(void)j; (void)k;

		int arr[10]{};
		auto        arr2 = arr;   // int*
		decltype(arr) arr3{};     // int[10]
		(void)arr2; (void)arr3;
	}
} // namespace section_c

// ���� D: vector[] ��ȯ, ���� ������ value category
namespace section_d {
	void run() {
		std::vector<int> vec{ 10, 20 };

		auto a = vec[0];       // int
		decltype(vec[0]) r = vec[0]; // int& (operator[]�� ���� ��ȯ)
		r = 99; // ������ �ٲٸ� ���Ұ� �ٲ�
		(void)a;

		int x = 0, y = 0;
		const int cx = 40, cy = 50;
		double d1 = 3.14, d2 = 9.8;

		// prvalue �� T
		decltype(x * y)   t_xy = 0; // int
		decltype(cx * cy) t_cxy = 0; // int

		// ���� Ÿ���� lvalue 3�� �� T&
		decltype(d1 < d2 ? d1 : d2) t_dd = d1; // double&
		// ȥ�� Ÿ���� 3�� �� ���� Ÿ���� prvalue
		decltype(x < d2 ? x : d2) t_mixed = d2; // double

		(void)t_xy; (void)t_cxy; (void)t_dd; (void)t_mixed;
	}
} // namespace section_d

// ���� E: ���� Ż�� ��ũ�� (remove_reference)
namespace section_e {
	template<typename T1, typename T2>
	auto fpmin_raw(T1 x, T2 y) -> decltype(x < y ? x : y) {
		return x < y ? x : y; // ���� Ÿ���̸� ������ �� �� ����
	}

	template<typename T1, typename T2>
	auto fpmin_value(T1 x, T2 y)
		-> typename std::remove_reference<decltype(x < y ? x : y)>::type{
		   return x < y ? x : y; // ���� ������ �ϰ��� �� Ÿ������
	}

		void run() {
		int i = 42; double d = 45.1; int& j = i;

		using R1 = decltype(fpmin_raw(i, d)); // double
		using R2 = decltype(fpmin_raw(j, d)); // double
		using R3 = decltype(fpmin_raw(d, d)); // double& (���� Ÿ�� �� ����)
		using R4 = decltype(fpmin_value(d, d)); // double (���� ����)

		(void)sizeof(R1);
		(void)sizeof(R2);
		(void)sizeof(R3);
		(void)sizeof(R4);
	}
} // namespace section_e

// ���� F: declval / decltype(auto)
namespace section_f {
	template <typename T>
	decltype(std::declval<T>().f()) call_f_and_return(T& t) {
		return t.f();
	}
	template <typename T>
	decltype(auto) call_f_and_return2(T& t) {
		return t.f(); // ����/const ����
	}

	struct B1 { int f() { return 1; } };
	struct B2 { B2(int) {} int f() { return 2; } };

	void run() {
		B1 b1; B2 b2(1);
		int n1 = call_f_and_return(b1);
		int n2 = call_f_and_return2(b2);
		cout << "[declval/decltype(auto)] " << n1 << " " << n2 << endl; // 1 2
	}
} // namespace section_f

// ���� G: ���� Ÿ�� ����
namespace section_g {
	void run() {
		auto func = []() { return 42; };
		decltype(func)  func_copy = func; // �� ����
		decltype((func)) func_ref = func; // ���� ���ε�

		cout << "func()=" << func()
			<< ", func_copy()=" << func_copy()
			<< ", func_ref()=" << func_ref() << endl;
	}
} // namespace section_g

// ���� H: Ŭ���� ���ø� ���� Ÿ�� ����
namespace section_h {
	template<typename R>
	class SomeFunctor {
	public:
		using result_type = R;
		result_type operator()() { return R(); }
	};

	void run() {
		SomeFunctor<int> f;
		// ��ü Ÿ�Կ��� ���� Ÿ�� ��Ī�� ����
		using inner_t = decltype(f)::result_type; // int
		(void)sizeof(inner_t);
	}
} // namespace section_h

int main() {
	section_a::run();
	section_b::run();
	section_c::run();
	section_d::run();
	section_e::run();
	section_f::run();
	section_g::run();
	section_h::run();

	// ��� ���� �Ϻ�
	std::vector<int> v{ 10, 20 };
	cout << "v[0]=" << v[0] << endl; // section_d���� r�� 99�� �ٲ���� �� ����
	return 0;
}