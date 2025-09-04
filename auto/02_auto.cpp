/*
 * @brief  : C++ auto 심화 요약 예제
 * @details:
 *   - auto: 컴파일 타임 타입 추론(동적 타입 아님), 반드시 초기화 필요
 *   - 규칙 핵심
 *       * auto      : const / & / volatile 제거(값으로 복사 추론)
 *       * auto&     : 참조 유지, const 유지
 *       * auto*     : 포인터 유지
 *       * auto&&    : 보편 참조(universal reference) — lvalue→&, rvalue→&&
 *   - 권장 사용: 반복자, 템플릿, 함수 반환값 보관, 함수 포인터, 긴 타입 대체
 *   - 주의: 가독성 저하 시 명시적 타입 사용
 */

#include <iostream>
#include <vector>
#include <string>
#include <type_traits>
#include <utility>     // std::move
using namespace std;

// 예제용 함수/타입
void TestFunc(int) {}

struct Widget {
    void Fn() {}
};

int add(int x, int y) { return x + y; }
// C++14 반환형 추론
auto Add14(int x, int y) { return x + y; }

int main() {
    cout << "=== 1. 기본/초기화 ===\n";
    {
        int a1 = 1;
        int a2(1);
        int a3{ 1 };        // C++11
        int b(a1);        // copy
        (void)a1; (void)a2; (void)a3; (void)b;

        auto c = a1;      // int
        int x = int();    // 0 초기화
        auto auto_x = x;  // int

        static_assert(is_same_v<decltype(c), int>);
        static_assert(is_same_v<decltype(auto_x), int>);
        cout << "c=" << c << ", auto_x=" << auto_x << '\n';
    }

    cout << "\n=== 2. 포인터/참조 ===\n";
    {
        int n = 1.2;          // 경고 가능: 1.2 → 1 (n는 int)
        auto& r = n;          // int&
        auto* p = &r;         // int*

        Widget w;
        auto  o = w;         // Widget (복사)
        auto* pW = &w;        // Widget*
        auto  pW2 = &w;       // Widget*
        auto& rw = w;         // Widget&
        auto  w2 = w;         // Widget (복사)
        (void)o; (void)pW; (void)pW2; (void)w2;
        rw.Fn();
        cout << "r=" << r << ", *p=" << *p << '\n';
    }

    cout << "\n=== 3. 타입 추론 규칙 (const/ref/volatile) ===\n";
    {
        int x = 42;
        const int& crx = x;
        auto a1 = crx;        // int (const/ref 제거)
        volatile int vx = 1024;
        auto a2 = vx;         // int (volatile 제거)

        const int* pi = &x;
        auto p = pi;          // const int*  (포인터/포인터의 cv는 유지)

        const int c = 7;
        auto& rc = c;         // const int&  (auto&는 const 유지)

        static_assert(is_same_v<decltype(a1), int>);
        static_assert(is_same_v<decltype(a2), int>);
        static_assert(is_same_v<decltype(p), const int*>);
        static_assert(is_same_v<decltype(rc), const int&>);

        cout << "a1=" << a1 << ", a2=" << a2 << ", *p=" << *p << ", rc=" << rc << '\n';
    }

    cout << "\n=== 4. auto&& (보편 참조) ===\n";
    {
        int d = 10;
        auto&& r1 = 5;           // int&& (rvalue)
        auto&& r2 = d;           // int&  (lvalue)
        auto&& r3 = std::move(d);// int&&
        double dv = 1.0;
        auto&& dr1 = dv;         // double&
        auto&& dr2 = 1.0;        // double&&
        (void)r1; (void)r2; (void)r3; (void)dr1; (void)dr2;
        cout << "universal ref demo completed\n";
    }

    cout << "\n=== 5. 배열/컨테이너/반복자 ===\n";
    {
        int buf[5] = { 0,1,2,3,4 };
        for (auto v : buf) { cout << v << ' '; }
        cout << '\n';

        vector<int> vv{ 1,2,3,4,5 };
        for (auto it = vv.begin(); it != vv.end(); ++it) {
            cout << *it << ' ';
        }
        cout << '\n';

        auto arr = { 1,2,3,4 };   // initializer_list<int>
        static_assert(is_same_v<decltype(arr), initializer_list<int>>);
        for (auto number : arr) cout << number << ' ';
        cout << '\n';
    }

    cout << "\n=== 6. 함수/함수 포인터/반환형 보관 ===\n";
    {
        auto sum = add(5, 6);       // int
        auto sum14 = Add14(7, 8);   // int (C++14)
        auto pf = TestFunc;         // void(*)(int)
        pf(123);
        cout << "sum=" << sum << ", sum14=" << sum14 << '\n';
    }

    cout << "\n=== 7. 람다 + auto ===\n";
    {
        // 반환형 지정/추론
        auto test1 = [](int* a) -> int& { return *a; }; // int& 리턴
        auto test2 = [](int* a) { return *a; };         // int   리턴

        int t = 99;
        int* pt = &t;
        auto  r1 = test1(pt);    // int (참조 반환을 값으로 받음 → 복사)
        auto& r2 = test1(pt);    // int& (참조 유지)
        (void)test2;
        r2 = 77;                 // 참조로 변경 → t 가 77로 변경
        cout << "r1=" << r1 << ", r2=" << r2 << ", t=" << t << '\n';

        // 제네릭 람다(C++14~)
        auto f = [](auto x, auto y) { return x + y; };
        cout << "f(1.1,5)=" << f(1.1, 5) << ", f(3.7,23.1)=" << f(3.7, 23.1) << '\n';
    }

    // 참고: 멤버 변수 타입으로는 auto 사용 불가(클래스 정의 내부 데이터 멤버)
    // struct S { auto n = 0; }; // 불가

    cout << "\n=== done ===\n";
    return 0;
}
