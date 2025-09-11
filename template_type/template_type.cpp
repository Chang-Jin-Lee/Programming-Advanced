// template_type_deduction_rules.cpp
// 템플릿 타입 추론 3대 경우, 배열/함수 decay, 포인터 const, 보편 참조 데모

#include <iostream>
#include <type_traits>
#include <typeinfo>
#include <array>
#include <vector>

using std::cout;
using std::endl;

// ----------------------------------------
// 출력 유틸: T / param 의 특성 확인용
// (typeid().name()은 구현마다 다를 수 있습니다. 의미 확인용 보조 출력)
// ----------------------------------------
template <typename T>
void show_T(const char* label) {
	cout << label
		<< "  name=" << typeid(T).name()
		<< "  is_const=" << std::is_const<T>::value
		<< "  is_volatile=" << std::is_volatile<T>::value
		<< "  is_ref=" << std::is_reference<T>::value
		<< "\n";
}

template <typename T>
void show_expr(const char* label, T&&) {
	using U = T;
	cout << label
		<< "  name=" << typeid(U).name()
		<< "  is_const=" << std::is_const<U>::value
		<< "  is_volatile=" << std::is_volatile<U>::value
		<< "  is_ref=" << std::is_reference<U>::value
		<< "\n";
}

// ----------------------------------------
// 경우 1) 값 전달
// ----------------------------------------
template <typename T>
void case_value(T param) {
	show_T<T>("[case_value] T      ");
	show_expr("[case_value] param  ", param);
}

// ----------------------------------------
// 경우 2) 참조/포인터 (보편참조 아님)
// ----------------------------------------
template <typename T>
void case_lref(T& param) {
	show_T<T>("[case_lref] T       ");
	show_expr("[case_lref] param   ", param);
}

template <typename T>
void case_clref(const T& param) {
	show_T<T>("[case_clref] T      ");
	show_expr("[case_clref] param  ", param);
}

template <typename T>
void case_ptr(T* param) {
	show_T<T>("[case_ptr] T        ");
	show_expr("[case_ptr] param    ", param);
}

// ----------------------------------------
// 경우 3) 보편 참조
// ----------------------------------------
template <typename T>
void case_uref(T&& param) {
	show_T<T>("[case_uref] T       ");
	show_expr("[case_uref] param   ", param);
}

// ----------------------------------------
// 포인터 const 두 겹
// ----------------------------------------
template <typename T>
void ptr_value_param(T param) { // 값 전달
	show_T<T>("[ptr_value] T       ");
	show_expr("[ptr_value] param   ", param);
}

// ----------------------------------------
// 배열/함수 decay
// ----------------------------------------
template <typename T>
void value_param(T param) { // 값 전달 → decay
	show_T<T>("[value_param] T     ");
	show_expr("[value_param] param ", param);
}

template <typename T>
void ref_param(T& param) { // 참조 전달 → 원형 유지
	show_T<T>("[ref_param] T       ");
	show_expr("[ref_param] param   ", param);
}

template <typename T, std::size_t N>
constexpr std::size_t arraySize(T(&)[N]) noexcept {
	return N;
}

void someFunc(int, double) {} // 타입: void(int,double)

template <typename T>
void f1(T param) {            // 값 전달 → 함수 포인터
	show_T<T>("[f1] T         ");
}

template <typename T>
void f2(T& param) {           // 참조 전달 → 함수 참조
	show_T<T>("[f2] T         ");
}

// ----------------------------------------
// 포인터 T* 파라미터에 const 인수 전달 예
// ----------------------------------------
template <typename T>
void f_ptr(T* param) {
	show_T<T>("[f_ptr] T       ");
	show_expr("[f_ptr] param   ", param);
}

// ----------------------------------------
// 데모 실행
// ----------------------------------------
int main() {
	cout << "=== 경우 1) 값 전달 ===\n";
	{
		int x = 27;
		const int cx = x;
		const int& rx = x;
		case_value(x);   // T=int,        param=int
		case_value(cx);  // T=int,        param=int
		case_value(rx);  // T=int,        param=int
	}

	cout << "\n=== 경우 2) 참조/포인터 (보편참조 아님) ===\n";
	{
		int x = 27;
		const int cx = x;
		const int& rx = x;

		case_lref(x);     // T=int,         param=int&
		case_lref(cx);    // T=const int,   param=const int&
		case_lref(rx);    // T=const int,   param=const int&

		case_clref(x);    // T=int,         param=const int&
		case_clref(cx);   // T=int,         param=const int&
		case_clref(rx);   // T=int,         param=const int&

		int* p = &x;
		const int* pc = &x;
		case_ptr(p);      // T=int,         param=int*
		case_ptr(pc);     // T=const int,   param=const int*
	}

	cout << "\n=== 경우 3) 보편 참조 ===\n";
	{
		int x = 27;
		const int cx = x;
		const int& rx = x;

		case_uref(x);     // lvalue → T=int&,         param=int&
		case_uref(cx);    // lvalue → T=const int&,   param=const int&
		case_uref(rx);    // lvalue → T=const int&,   param=const int&
		case_uref(27);    // rvalue → T=int,          param=int&&
	}

	cout << "\n=== 포인터 const 두 겹 (const char* const) 값 전달 ===\n";
	{
		const char* const ptr = "abc";
		ptr_value_param(ptr); // T=const char*, param=const char*
	}

	cout << "\n=== 배열/함수 decay ===\n";
	{
		const char name[] = "abc";
		value_param(name); // T=const char*, param=const char* (배열→포인터)
		ref_param(name);   // T=const char[4], param=const char(&)[4] (참조로 받으면 유지)

		cout << "arraySize(name) = " << arraySize(name) << "\n";

		f1(someFunc);     // T=void(*)(int,double)
		f2(someFunc);     // T=void(&)(int,double)
	}

	cout << "\n=== 포인터 파라미터에 const 인수 전달 예 ===\n";
	{
		const int a = 0;
		f_ptr(&a); // T=const int, param=const int*
	}

	return 0;
}