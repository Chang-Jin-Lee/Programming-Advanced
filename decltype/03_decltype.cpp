// template_return_examples.cpp
// 템플릿 함수 리턴 타입: decltype, trailing return type, auto, decltype(auto), forwarding

#include <iostream>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

using namespace std;

//------------------------------------------------------------------------------
// 1) 출력 파라미터(out parameter) 타입 결정: decltype(t+u)*
//------------------------------------------------------------------------------
template <typename T, typename U>
void add_out(T t, U u, decltype(t + u)* result) {
	*result = t + u;
}

//------------------------------------------------------------------------------
// 2) 반환 타입 결정
//   - C++11: 후행 리턴 타입 (t,u 를 파라미터 뒤에서 참조)
//------------------------------------------------------------------------------
template <typename T, typename U>
auto add_trailing(T t, U u) -> decltype(t + u) {
	return t + u;
}

//   - C++14: auto 로 값 타입 반환
template <typename T, typename U>
auto add_auto_value(T t, U u) {
	return t + u; // 보통 값으로 충분할 때
}

//   - C++14: decltype(auto) 로 참조/const 보존
template <typename T, typename U>
decltype(auto) add_decltype_auto(T&& t, U&& u) {
	// 주의: (t+u)가 참조를 반환하는 연산이라면 참조 유지
	return (t + u);
}

//------------------------------------------------------------------------------
// 3) 전달 참조 + forward
//------------------------------------------------------------------------------
template<typename T, typename U>
auto Plus(T&& lhs, U&& rhs) {
	return std::forward<T>(lhs) + std::forward<U>(rhs);
}

//------------------------------------------------------------------------------
// 4) auto 반환의 함정 vs decltype(auto)
//------------------------------------------------------------------------------
class C {
	int arr_[10] = { 1,2,3,4,5,6,7,8,9,10 };
public:
	int& operator[](int i) { return arr_[i]; }
};

template<typename T>
auto changearr(T& c, int index) {      // auto: 값 복사
	return c[index];                   // int& -> int 로 복사되어 참조 소실
}

template<typename T>
decltype(auto) GetArr(T&& c, int index) { // decltype(auto): 원형 유지
	return std::forward<T>(c)[index];     // int& 유지
}

//------------------------------------------------------------------------------
// 5) 데모
//------------------------------------------------------------------------------
int main() {
	// (1) 출력 파라미터
	{
		int    i = 10;
		double d = 2.5;
		auto   r = 0.0; // double
		add_out(i, d, &r); // decltype(i+d) == double
		cout << "[add_out] " << r << "\n";
	}

	// (2) 반환 타입 (후행 리턴 타입 / auto / decltype(auto))
	{
		cout << "[add_trailing] " << add_trailing(10, 20.5) << "\n"; // double
		cout << "[add_auto_value] " << add_auto_value(10, 20) << "\n";  // int
		// add_decltype_auto 는 참조를 보존할 때 의미가 크다.
		// 여기서는 (t+u)가 prvalue라 값으로 추론됨.
		cout << "[add_decltype_auto] " << add_decltype_auto(1, 2) << "\n"; // int
	}

	// (3) 전달 참조 + forward
	{
		string A("abc"), B("def");
		cout << "[Plus string] " << Plus(A, B) << "\n"; // "abcdef"
		cout << "[Plus int]    " << Plus(10, 20) << "\n"; // 30
	}

	// (4) auto vs decltype(auto) 반환 차이
	{
		C cls;
		auto i = changearr(cls, 3); // 값 복사(int)
		i = 100;                    // cls[3] 는 바뀌지 않음
		cout << "[changearr] cls[3] = " << cls[3] << " (unchanged)\n";

		GetArr(cls, 3) = 100;       // decltype(auto): int& → 실제 원소 변경
		cout << "[GetArr]   cls[3] = " << cls[3] << " (modified)\n";
	}

	// (5) vector 예시: operator[] 는 참조를 반환
	{
		vector<int> v{ 10, 20 };
		auto a = v[0];          // int (복사)
		decltype(v[0]) r = v[0]; // int& (참조)
		r = 77;                 // v[0] 실제 변경
		cout << "[vector[]] a=" << a << ", v[0]=" << v[0] << "\n";
	}

	return 0;
}