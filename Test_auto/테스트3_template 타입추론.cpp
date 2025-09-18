// 템플릿 타입 추론  // T, param 의 타입은 ???

#include <iostream>
#include <vector>
using namespace std;

template <typename T>
void f1(T param) {}					// param이 값으로 전달된다	-> T는 &, const, volatile 무시

template <typename T>
void f2(T& param) {}				// param은 T&				-> T는 & 무시, const, volatile 반영

template <typename T>
void f3(const T& param) {}			// param은 const T&			-> T는 &, const 무시 

template <typename T>
void f4(T* param);					// param이 포인터			-> T는 * 무시

template<typename T>
void f5(T&& param);					// param이 보편 참조(universal reference)

int main()
{
	int x = 27;
	const int cx = x;
	const int& rx = x;

	f1(x);							// T와 param의 타입은	int				int
	f1(cx);							// T와 param의 타입은	int				int
	f1(rx);							// T와 param의 타입은	int				int

	f2(x);							// T와 param의 타입은	int				int&
	f2(cx);							// T와 param의 타입은	const int		const int&
	f2(rx);							// T와 param의 타입은	const int		const int&

	f3(x);							// T와 param의 타입은	int				const int&
	f3(cx);							// T와 param의 타입은	int				const int&
	f3(rx);							// T와 param의 타입은	int				cosnt int&

	//int x = 27;					// T와 param의 타입은
	const int* px = &x;				// T와 param의 타입은
	f4(&x);							// T와 param의 타입은	int				int*
	f4(px);							// T와 param의 타입은	const int		const int*

	f5(x);							// T와 param의 타입은	int				int&
	f5(cx);							// T와 param의 타입은	const int&		const int&
	f5(rx);							// T와 param의 타입은	const int&		const int&
	f5(27);							// T와 param의 타입은	int				int&&
}

//-----------------------------------------------------------------------------
//ParamType이 보편 참조(universal reference) 인 경우
//		만일 expr이 좌측값이면, T 와 ParamType 둘 다 좌측값 참조로 추론된다.  ***
//		만일 expr이 우측값이면, '정상적인' 규칙들이 적용

