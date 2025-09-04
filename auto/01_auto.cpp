/*
 * @brief : C++11 auto Ű���� ���� ����
 * @details :
 *   - auto�� ������ Ÿ�� Ÿ�� �߷� (���� Ÿ�� �ƴ�!)
 *   - �ݵ�� �ʱ�ȭ �ʿ�
 *   - �ݺ���, ���ø�, �Լ� ��ȯ��, �Լ� ������ ��� Ư�� ����
 *   - ����:
 *       ������ �� auto*
 *       const ���� �� const auto&
 *   - ����: ������ ���� ���� �� ����� Ÿ���� �� �������� ���� auto ��� ���
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
    cout << "=== �⺻ Ÿ�� �߷� ===" << endl;
    auto d1 = 5.0;          // double
    auto i1 = 1 + 2;        // int
    auto n = 0.123;         // double (0.123f �� float)
    cout << "d1(double): " << d1 << ", i1(int): " << i1 << ", n(double): " << n << endl;

    unsigned char x = 1;
    auto ax = x;            // unsigned char �� int �� �°�
    ax = 1.3;               // �Ҽ� ���Խ� double��int ��ȯ
    printf("x=%d, ax=%d\n", x, ax);

    cout << "\n=== const, reference ���� ===" << endl;
    const int& a = 0;
    auto aa = a;            // int (const, ref ����)
    cout << "aa(int): " << aa << endl;

    cout << "\n=== initializer_list ���� ===" << endl;
    auto a1{ 1 };           // int
    auto b = { 1,2,3 };     // initializer_list<int>
    auto f1 = { 1 };        // initializer_list<int>
    auto f2{ 1 };           // int
    cout << "a1(int): " << a1 << ", f2(int): " << f2 << endl;

    cout << "\n=== �����Ϳ� ���� ===" << endl;
    Cat* myCat = new Cat("CoCo", 2);
    auto  myCatPtr1 = myCat;    // Cat*
    auto* myCatPtr2 = myCat;    // Cat* (����: ������ ��)
    cout << "myCatPtr1 and myCatPtr2 are Cat*" << endl;

    Cat myCat2("Mimi", 3);
    Cat& myCat2Ref = myCat2;
    auto  CatRef1 = myCat2Ref;  // Cat (���� ����)
    auto& CatRef2 = myCat2Ref;  // Cat& (���� ����)
    cout << "CatRef1 is Cat, CatRef2 is Cat&" << endl;

    cout << "\n=== const auto& ===" << endl;
    const int b2 = 1;
    auto& a2 = b2;              // const int& �� �߷�
    const auto& a3 = b2;        // const int& (����)
    cout << "a2=" << a2 << ", a3=" << a3 << endl;

    cout << "\n=== �ݺ���(iterator) ===" << endl;
    vector<int> v = { 1,2,3,4,5 };
    for (auto it = v.begin(); it != v.end(); ++it) {
        cout << *it << " ";
    }
    cout << endl;

    cout << "\n=== �Լ� ������ ===" << endl;
    auto funcPtr = TestFunc;    // int(*)(int,int) �ڵ� �߷�
    cout << "funcPtr(3,4) = " << funcPtr(3, 4) << endl;

    delete myCat;
    return 0;
}
