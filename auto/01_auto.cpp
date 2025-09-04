/*
 * @brief : C++11 auto 키워드 정리 예제
 * @details :
 *   - auto는 컴파일 타임 타입 추론 (동적 타입 아님!)
 *   - 반드시 초기화 필요
 *   - 반복자, 템플릿, 함수 반환형, 함수 포인터 등에서 특히 유용
 *   - 권장:
 *       포인터 → auto*
 *       const 참조 → const auto&
 *   - 주의: 가독성 저하 가능 → 명시적 타입이 더 직관적일 때는 auto 대신 명시
 */

#include <iostream>
#include <vector>
#include <string>
using namespace std;

int TestFunc(int a, int b) { return a + b; }

class Cat {
    string name;
    int age;
public:
    Cat(string s, int n) : name(s), age(n) {}
};

int main()
{
    cout << "=== 기본 타입 추론 ===" << endl;
    auto d1 = 5.0;          // double
    auto i1 = 1 + 2;        // int
    auto n = 0.123;         // double (0.123f → float)
    cout << "d1(double): " << d1 << ", i1(int): " << i1 << ", n(double): " << n << endl;

    unsigned char x = 1;
    auto ax = x;            // unsigned char → int 로 승격
    ax = 1.3;               // 소수 대입시 double→int 변환
    printf("x=%d, ax=%d\n", x, ax);

    cout << "\n=== const, reference 무시 ===" << endl;
    const int& a = 0;
    auto aa = a;            // int (const, ref 무시)
    cout << "aa(int): " << aa << endl;

    cout << "\n=== initializer_list 주의 ===" << endl;
    auto a1{ 1 };           // int
    auto b = { 1,2,3 };     // initializer_list<int>
    auto f1 = { 1 };        // initializer_list<int>
    auto f2{ 1 };           // int
    cout << "a1(int): " << a1 << ", f2(int): " << f2 << endl;

    cout << "\n=== 포인터와 참조 ===" << endl;
    Cat* myCat = new Cat("CoCo", 2);
    auto  myCatPtr1 = myCat;    // Cat*
    auto* myCatPtr2 = myCat;    // Cat* (권장: 가독성 ↑)
    cout << "myCatPtr1 and myCatPtr2 are Cat*" << endl;

    Cat myCat2("Mimi", 3);
    Cat& myCat2Ref = myCat2;
    auto  CatRef1 = myCat2Ref;  // Cat (참조 무시)
    auto& CatRef2 = myCat2Ref;  // Cat& (참조 유지)
    cout << "CatRef1 is Cat, CatRef2 is Cat&" << endl;

    cout << "\n=== const auto& ===" << endl;
    const int b2 = 1;
    auto& a2 = b2;              // const int& 로 추론
    const auto& a3 = b2;        // const int& (권장)
    cout << "a2=" << a2 << ", a3=" << a3 << endl;

    cout << "\n=== 반복자(iterator) ===" << endl;
    vector<int> v = { 1,2,3,4,5 };
    for (auto it = v.begin(); it != v.end(); ++it) {
        cout << *it << " ";
    }
    cout << endl;

    cout << "\n=== 함수 포인터 ===" << endl;
    auto funcPtr = TestFunc;    // int(*)(int,int) 자동 추론
    cout << "funcPtr(3,4) = " << funcPtr(3, 4) << endl;

    delete myCat;
    return 0;
}
