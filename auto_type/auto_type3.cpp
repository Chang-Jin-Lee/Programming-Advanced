// auto_explicitly_typed_initializer.cpp
// g++ -std=c++17 auto_explicitly_typed_initializer.cpp

#include <iostream>
#include <vector>
#include <string>

using namespace std;

struct Widget {};

// 임시 vector<bool>을 반환 (의도적으로 프록시/수명 이슈를 유발할 수 있는 형태)
std::vector<bool> features(const Widget& w) {
	std::vector<bool> v(5, false);
	v[3] = true; // 임의로 한 비트만 true
	return v;    // RVO로 반환되는 임시 컨테이너
}

double calcEpsilon() { return 1.1; }

int main() {
	// 1) 기본 비교: auto& / decltype((x)) / 배열 요소 참조
	{
		int a = 0;
		auto& val1 = a;          // int&
		decltype((a)) val2 = a;  // int& (괄호로 lvalue 처리)
		int arr[] = { 1,2,3 };
		auto& val3 = arr[0];     // int&
		(void)val1; (void)val2; (void)val3;
	}

	// 2) 프록시 타입 문제 예시 (UB 코드: 주석만 제공)
	{
		Widget w;

		// 안전: 즉시 bool로 변환하여 저장 (임시 vector<bool>과 무관해짐)
		bool highPriority1 = features(w)[3];
		cout << "[bool   ] highPriority1 = " << highPriority1 << "\n";

		// 위험(설명만): 프록시 타입이 auto로 저장되어 임시 컨테이너 소멸 후 UB 가능
		// auto highPriority2 = features(w)[3];    // 지양 (프록시 저장, UB 가능)
		// cout << "[auto??] highPriority2 = " << highPriority2 << "\n";

		// 해결: 명시적 타입 초기치 관용구
		auto highPriority3 = static_cast<bool>(features(w)[3]);
		cout << "[cast  ] highPriority3 = " << highPriority3 << "\n";
	}

	// 3) 축소 변환 의도 명시
	{
		// 묵시적 축소
		float ep1 = calcEpsilon();
		// 의도적/명시적 축소
		auto  ep2 = static_cast<float>(calcEpsilon());
		cout << "[epsilon] ep1=" << ep1 << " ep2=" << ep2 << "\n";
	}

	// 4) vector<bool> 로컬 객체의 경우에도 값으로 고정하고 싶다면 cast
	{
		vector<bool> bits = { true, false, true };
		// 프록시를 값(bool)으로
		auto b0 = static_cast<bool>(bits[0]);
		auto b1 = static_cast<bool>(bits[1]);
		cout << "[bits] b0=" << b0 << " b1=" << b1 << "\n";
	}

	return 0;
}