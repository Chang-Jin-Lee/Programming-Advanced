// decltype_deep_dive.cpp
// 컴파일 타임 타입 추출: decltype / declval / decltype(auto) 종합 예제

#include <iostream>
#include <type_traits>
#include <utility>
#include <vector>

using std::cout;
using std::endl;

// 전방 선언: 정의 없이도 decltype 용도로만 사용 가능 (호출 X)
const int&& fx();

// 섹션 A: 변수/참조/우측값참조, 멤버 접근, 괄호 유무
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

// 섹션 B: 함수/함수식 decltype
namespace section_b {
	void run() {
		// fx는 "const int&& fx();" 로 선언되어 있음
		// 함수 호출 표현식의 타입
		decltype(fx())  rref_value = 42; // const int&&
		(void)rref_value;

		// 함수 자체의 타입
		decltype(fx) func_like; // const int&& ()

		// 주의: func_like를 호출하면 정의가 없으므로 링크 오류.
		(void)func_like;
	}
} // namespace section_b

// 섹션 C: auto vs decltype
namespace section_c {
	void run() {
		const int i = 4;
		auto j = i;        // int (const 탈락)
		decltype(i) k = i; // const int (보존)
		(void)j; (void)k;

		int arr[10]{};
		auto        arr2 = arr;   // int*
		decltype(arr) arr3{};     // int[10]
		(void)arr2; (void)arr3;
	}
} // namespace section_c

// 섹션 D: vector[] 반환, 조건 연산자 value category
namespace section_d {
	void run() {
		std::vector<int> vec{ 10, 20 };

		auto a = vec[0];       // int
		decltype(vec[0]) r = vec[0]; // int& (operator[]는 참조 반환)
		r = 99; // 참조로 바꾸면 원소가 바뀜
		(void)a;

		int x = 0, y = 0;
		const int cx = 40, cy = 50;
		double d1 = 3.14, d2 = 9.8;

		// prvalue → T
		decltype(x * y)   t_xy = 0; // int
		decltype(cx * cy) t_cxy = 0; // int

		// 동일 타입의 lvalue 3항 → T&
		decltype(d1 < d2 ? d1 : d2) t_dd = d1; // double&
		// 혼합 타입의 3항 → 공통 타입의 prvalue
		decltype(x < d2 ? x : d2) t_mixed = d2; // double

		(void)t_xy; (void)t_cxy; (void)t_dd; (void)t_mixed;
	}
} // namespace section_d

// 섹션 E: 참조 탈락 테크닉 (remove_reference)
namespace section_e {
	template<typename T1, typename T2>
	auto fpmin_raw(T1 x, T2 y) -> decltype(x < y ? x : y) {
		return x < y ? x : y; // 동일 타입이면 참조가 될 수 있음
	}

	template<typename T1, typename T2>
	auto fpmin_value(T1 x, T2 y)
		-> typename std::remove_reference<decltype(x < y ? x : y)>::type{
		   return x < y ? x : y; // 참조 제거해 일관된 값 타입으로
	}

		void run() {
		int i = 42; double d = 45.1; int& j = i;

		using R1 = decltype(fpmin_raw(i, d)); // double
		using R2 = decltype(fpmin_raw(j, d)); // double
		using R3 = decltype(fpmin_raw(d, d)); // double& (동일 타입 → 참조)
		using R4 = decltype(fpmin_value(d, d)); // double (참조 제거)

		(void)sizeof(R1);
		(void)sizeof(R2);
		(void)sizeof(R3);
		(void)sizeof(R4);
	}
} // namespace section_e

// 섹션 F: declval / decltype(auto)
namespace section_f {
	template <typename T>
	decltype(std::declval<T>().f()) call_f_and_return(T& t) {
		return t.f();
	}
	template <typename T>
	decltype(auto) call_f_and_return2(T& t) {
		return t.f(); // 참조/const 유지
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

// 섹션 G: 람다 타입 접근
namespace section_g {
	void run() {
		auto func = []() { return 42; };
		decltype(func)  func_copy = func; // 값 복사
		decltype((func)) func_ref = func; // 참조 바인딩

		cout << "func()=" << func()
			<< ", func_copy()=" << func_copy()
			<< ", func_ref()=" << func_ref() << endl;
	}
} // namespace section_g

// 섹션 H: 클래스 템플릿 내부 타입 접근
namespace section_h {
	template<typename R>
	class SomeFunctor {
	public:
		using result_type = R;
		result_type operator()() { return R(); }
	};

	void run() {
		SomeFunctor<int> f;
		// 객체 타입에서 내부 타입 별칭에 접근
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

	// 출력 예시 일부
	std::vector<int> v{ 10, 20 };
	cout << "v[0]=" << v[0] << endl; // section_d에서 r로 99로 바뀌었을 수 있음
	return 0;
}