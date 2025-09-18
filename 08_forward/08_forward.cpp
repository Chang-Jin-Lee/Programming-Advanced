// universal_reference_forwarding_copy_elision.cpp
// Build: g++ -std=gnu++17 -O2 universal_reference_forwarding_copy_elision.cpp && ./a.out

#include <iostream>
#include <string>
#include <utility>
#include <type_traits>
#include <vector>
#include <cstring>

using namespace std;

// =============================================================
// 1) 보편 참조, forward 기본 데모
// =============================================================
namespace demo_universal_ref {

	struct A {};

	void foo(A&) { cout << "foo(A&)\n"; }
	void foo(const A&) { cout << "foo(const A&)\n"; }
	void foo(A&&) { cout << "foo(A&&)\n"; }

	template<class T>
	void wrapper_no_forward(T&& u) {     // 보편 참조
		// 주의: u는 '이름이 있는 변수'이므로 표현식으로 쓰면 항상 lvalue
		foo(u); // 항상 lvalue 오버로드로 감
	}

	template<class T>
	void wrapper_forward(T&& u) {        // 보편 참조 + 완벽 전달
		foo(std::forward<T>(u));         // 원래의 값 범주 유지
	}

	void run() {
		cout << "\n=== [1] Universal Reference & forward ===\n";
		A a; const A ca{};

		cout << "원본 호출:\n";
		foo(a);
		foo(ca);
		foo(A());    // rvalue

		cout << "wrapper_no_forward:\n";
		wrapper_no_forward(a);    // A&
		wrapper_no_forward(ca);   // const A&
		wrapper_no_forward(A());  // A& (의도와 다르게)

		cout << "wrapper_forward:\n";
		wrapper_forward(a);       // A&
		wrapper_forward(ca);      // const A&
		wrapper_forward(A());     // A&&
	}

} // namespace demo_universal_ref

// =============================================================
// 2) std::forward 시그니처 감각 익히기
// =============================================================
namespace demo_forward_sig {

	void run() {
		cout << "\n=== [2] std::forward signature intuition ===\n";
		int val = 1;

		int& r1 = std::forward<int&>(val);   // lvalue 반환
		int&& r2 = std::forward<int&&>(val);  // rvalue(xvalue) 반환
		(void)r1; (void)r2;
		cout << "forward<int&>(val) -> lvalue, forward<int&&>(val) -> xvalue\n";
	}

} // namespace demo_forward_sig

// =============================================================
// 3) 완벽 전달 실전: 오버로드 유지
// =============================================================
namespace demo_perfect_forwarding {

	struct S {};

	void func(S&) { cout << "func(S&)      \n"; }
	void func(S&&) { cout << "func(S&&)     \n"; }
	void func(const S&) { cout << "func(const S&)\n"; }

	template<class T>
	void wrapper_bad(T t) {           // 값 전달 → 값 범주 정보 유실
		func(t);                      // 항상 S&
	}

	template<class T>
	void wrapper_good(T&& t) {        // 보편 참조 + forward
		func(std::forward<T>(t));
	}

	void run() {
		cout << "\n=== [3] Perfect forwarding ===\n";
		S s;
		cout << "직접 호출:\n";
		func(s);
		func(S());
		cout << "wrapper_bad (값 전달):\n";
		wrapper_bad(s);
		wrapper_bad(S());
		cout << "wrapper_good (forward):\n";
		wrapper_good(s);
		wrapper_good(S());
	}

} // namespace demo_perfect_forwarding

// =============================================================
// 4) CustomVector: 복사/이동/오버로드 + 템플릿 래퍼
// =============================================================
namespace demo_custom_vector {

	class CustomVector {
	public:
		unsigned m_size = 0;
		int* ptr = nullptr;

		CustomVector(const unsigned& n, const int& init = 0) {
			cout << "Constructor\n";
			init_mem(n, init);
		}
		CustomVector(const CustomVector& other) { // Lvalue만
			cout << "Copy constructor\n";
			init_mem(other.m_size);
			for (unsigned i = 0; i < m_size; ++i) ptr[i] = other.ptr[i];
		}
		CustomVector(CustomVector&& other) noexcept { // Rvalue만
			cout << "Move constructor\n";
			m_size = other.m_size;
			ptr = other.ptr;
			other.m_size = 0;
			other.ptr = nullptr;
		}
		~CustomVector() { delete[] ptr; }

		void init_mem(const unsigned& n, const int& init = 0) {
			m_size = n;
			ptr = new int[m_size];
			for (unsigned i = 0; i < m_size; ++i) ptr[i] = init;
		}
	};

	void doSomething(CustomVector& v) { cout << "Pass by L-ref\n"; CustomVector tmp(v); }
	void doSomething(CustomVector&& v) { cout << "Pass by R-ref\n"; CustomVector tmp(std::move(v)); }

	template<class T>
	void doSomethingTemplate_X(T v) {    // 값 전달 → Lvalue로만 처리
		doSomething(v);
	}

	template<class T>
	void doSomethingTemplate_O(T&& v) {  // 보편 참조 + forward
		doSomething(std::forward<T>(v));
	}

	void run() {
		cout << "\n=== [4] CustomVector copy/move & forwarding ===\n";
		{
			CustomVector my(10, 1024);
			CustomVector tmp(my);              // copy
			cout << my.m_size << "\n";
		}
		{
			CustomVector my(10, 1024);
			CustomVector tmp(std::move(my));   // move
			cout << my.m_size << "\n";         // 0
		}
		{
			CustomVector my(10, 1024);
			doSomething(my);                   // L-ref
			doSomething(CustomVector(10, 8));  // R-ref (임시)
		}
		{
			CustomVector my(10, 1024);
			doSomethingTemplate_X(my);                // L-ref로만
			doSomethingTemplate_X(CustomVector(10, 8));// 여기도 L-ref로만
		}
		{
			CustomVector my(10, 1024);
			doSomethingTemplate_O(my);                // L-ref
			doSomethingTemplate_O(CustomVector(10, 8));// R-ref
		}
	}

} // namespace demo_custom_vector

// =============================================================
// 5) Copy Elision: RVO/NRVO/Passing Temporary
// =============================================================
namespace demo_copy_elision {

	struct Foo {
		Foo() { cout << "Constructed\n"; }
		Foo(const Foo&) { cout << "Copy-constructed\n"; }
		Foo(Foo&&) { cout << "Move-constructed\n"; }
		~Foo() { cout << "Destructed\n"; }
	};

	Foo RVO_F() {             // Return Value Optimization
		return Foo();         // 무명 임시 반환
	}
	Foo NRVO_F() {            // Named Return Value Optimization
		Foo f;
		return f;             // 명명된 지역변수 반환 (최적화 기대)
	}

	void f(Foo x) {           // 값으로 받기: 임시 전달 시 elision 가능성
		cout << "f(Foo)\n";
		(void)x;
	}

	void run() {
		cout << "\n=== [5] Copy elision (RVO/NRVO) ===\n";
		{
			cout << "- RVO_F():\n";
			Foo r = RVO_F();
		}
		{
			cout << "- NRVO_F():\n";
			Foo r = NRVO_F();
		}
		{
			cout << "- Passing temporary by value:\n";
			f(Foo());
			f(std::move(Foo()));
		}
	}

} // namespace demo_copy_elision

// =============================================================
// 6) 대용량 리소스 타입(Coo)로 RVO/NRVO/Move 체감
// =============================================================
namespace demo_coo {

	class Coo {
		char* data{};
	public:
		explicit Coo(const char* s) {
			data = new char[1000000];
			std::strcpy(data, s);
			cout << "Coo(const char*)    [data:" << (void*)data << "] this:[" << this << "]\n";
		}
		Coo(const Coo& rhs) {
			data = new char[1000000];
			std::strcpy(data, rhs.data);
			cout << "Coo(const Coo&)     [data:" << (void*)data << "] this:[" << this << "]\n";
		}
		Coo(Coo&& rhs) noexcept : data(rhs.data) {
			rhs.data = nullptr;
			cout << "Coo(Coo&&)          [data:" << (void*)data << "] this:[" << this << "]\n";
		}
		~Coo() {
			if (data) {
				cout << "~Coo()              [data:" << (void*)data << "] this:[" << this << "]\n";
				delete[] data;
			}
			else {
				cout << "~Coo()              this:[" << this << "] (null)\n";
			}
		}
		void ShowData() const {
			cout << "data: " << (data ? data : "(null)") << "\n";
		}
	};

	Coo FuncNrvo() {
		Coo s("sssss");
		return s;     // NRVO 시도
	}
	Coo FuncNrvoS(int a) {
		Coo s("sssss");
		Coo z("zzzzz");
		if (a == 100) return s; else return z;  // NRVO를 방해할 수 있는 분기
	}
	Coo FuncRvo() {
		return Coo("sssss"); // RVO
	}
	Coo FuncRvoS(int a) {
		if (a == 100) return Coo("sssss");
		else          return Coo("zzzzz");
	}

	void run() {
		cout << "\n=== [6] Coo: RVO/NRVO/move with heavy resource ===\n";

		{
			cout << "1) 일반 생성자\n";
			Coo a("aaaaa");
			Coo b("bbbbb");
		}
		{
			cout << "2) 복사 생성자\n";
			Coo a("aaaaa");
			Coo b(a);
		}
		{
			cout << "3) 이동 생성자\n";
			Coo a("aaaaa");
			Coo b(std::move(a));
		}
		{
			cout << "4) 임시 rvalue 생성\n";
			Coo("ccccc");
		}
		{
			cout << "5) 임시로부터 직접 초기화\n";
			Coo e(Coo("ddddd"));
			cout << "&e: " << &e << "\n";
		}
		{
			cout << "6) const&로 임시 참조 연장\n";
			const Coo& f(Coo("eeeee"));
			cout << "&f: " << &f << "\n";
		}
		{
			cout << "7) lvalue를 move\n";
			Coo a("aaaaa");
			Coo i(std::move(a));
			cout << "&i: " << &i << "\n";
			i.ShowData();
			a.ShowData();
		}
		{
			cout << "8) FuncNrvo() 호출만\n";
			FuncNrvo();
		}
		{
			cout << "9) FuncNrvoS() 호출만\n";
			FuncNrvoS(19);
		}
		{
			cout << "10) FuncRvo() 호출만\n";
			FuncRvo();
		}
		{
			cout << "11) FuncRvoS() 호출만\n";
			FuncRvoS(19);
		}
		{
			cout << "12) NRVO로 초기화\n";
			Coo g(FuncNrvo());
			cout << "&g: " << &g << "\n";
		}
		{
			cout << "13) NRVO(분기)로 초기화\n";
			Coo h(FuncNrvoS(11));
			cout << "&h: " << &h << "\n";
		}
		{
			cout << "14) RVO로 초기화\n";
			Coo j(FuncRvo());
			cout << "&j: " << &j << "\n";
		}
		{
			cout << "15) RVO(분기)로 초기화\n";
			Coo k(FuncRvoS(19));
			cout << "&k: " << &k << "\n";
		}
	}

} // namespace demo_coo

// =============================================================
// main: 모든 데모 실행
// =============================================================
int main() {
	demo_universal_ref::run();
	demo_forward_sig::run();
	demo_perfect_forwarding::run();
	demo_custom_vector::run();
	demo_copy_elision::run();
	demo_coo::run();
	return 0;
}