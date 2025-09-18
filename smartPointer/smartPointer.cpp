// memory_management_new_delete.cpp
// g++ -std=c++17 memory_management_new_delete.cpp

#include <iostream>
#include <memory>
#include <cstring>   // std::strncpy
using namespace std;

//-----------------------------
// 1) 기초: new/delete
//-----------------------------
void basic_new_delete() {
    // 단일 객체
    char* pchar = new char;   // 초기화 안 됨
    *pchar = 'a';
    cout << "[char] addr=" << static_cast<void*>(pchar)
        << " val=" << *pchar
        << " size=" << sizeof(*pchar) << "\n";
    delete pchar;

    int* pnum = new int(3);
    delete pnum;

    double* pdouble = new double{ 3.141592 };
    delete pdouble;

    // 배열
    int* pary1 = new int[3] {}; // 0 초기화
    cout << "pary1[0]=" << pary1[0] << "\n";
    delete[] pary1;

    int* pary2 = new int[3] {}; // 동일
    delete[] pary2;

    char* pary3 = new char[20] { "Hello World!" };
    cout << "pary3=" << pary3 << "\n";
    delete[] pary3;

    int* pary4 = new int[5] {1, 2, 3, 4, 5};
    delete[] pary4;

    // 연습(주석): float 동적배열 초기화/출력/증감 등
    /*
    int num = 4;
    auto* fary = new float[num]{1.1f, 2.2f, 3.3f, 4.4f};
    for (int i=0;i<num;i++) cout << fary[i] << ' ';
    cout << '\n';
    delete[] fary;
    */
}

//-----------------------------
// 2) 클래스 동적할당
//-----------------------------
class Test {
    int num{};
public:
    Test() { cout << "Test(): " << num << "\n"; }
    explicit Test(int x) : num(x) { cout << "Test(int): " << num << "\n"; }
    void Print() const { cout << "Test::num = " << num << "\n"; }
};

void class_allocation() {
    Test* p1 = new Test;
    Test* p2 = new Test(10);
    delete p1;
    delete p2;

    Test* plist = new Test[3]{ Test(1), Test(2), Test(3) };
    for (int i = 0; i < 3; ++i) plist[i].Print();
    delete[] plist;

    Test** plist2 = new Test * [3] { new Test(4), new Test(5), new Test(6) };
    for (int i = 0; i < 3; ++i) plist2[i]->Print();
    for (int i = 0; i < 3; ++i) delete plist2[i];
    delete[] plist2;
}

//-----------------------------
// 3) 누수/중복해제/재할당 예(설명용)
//-----------------------------
void pitfalls_demo() {
    // 해제 누락 (지양)
    /*
    int* leak = new int(42);
    // delete leak; // 누락 → leak
    */

    // 재할당 누수 (지양)
    /*
    int* p = new int(1);
    p = new int(2); // 이전 1은 잃어버림 → leak
    delete p;
    */

    // 중복 해제 (지양)
    /*
    int* q = new int(3);
    delete q;
    delete q; // UB
    */

    // 잘못된 해제 (지양)
    /*
    int* arr = new int[10];
    delete arr;   // UB, delete[] 필요
    // 올바른 해제: delete[] arr;
    */
}

//-----------------------------
// 4) RAII 예시
//-----------------------------
class MyString {
public:
    char* c = nullptr;
    explicit MyString(size_t len) : c(new char[len] {}) {}
    ~MyString() { delete[] c; }
};

void raii_demo() {
    // 전통 스타일 (예외 시 위험)
    {
        char* c = new char[100] {};
        std::strncpy(c, "Hello, world!", 100);
        // throw 0; // 예외 시 누수 가능
        delete[] c;
    }
    // RAII: 예외 시에도 해제 보장
    {
        MyString s(100);
        std::strncpy(s.c, "Hello, world!", 100);
        // throw 0; // 예외여도 소멸자에서 해제
    }
}

//-----------------------------
// 5) 스마트 포인터
//-----------------------------
class Foo {
public:
    Foo() { cout << "Foo 생성\n"; }
    ~Foo() { cout << "Foo 소멸\n"; }
};

void smart_ptr_demo() {
    // 수동
    {
        Foo* ptr = new Foo();
        delete ptr;
    }
    // unique_ptr
    {
        unique_ptr<Foo> sptr(new Foo()); // C++11
        // unique_ptr<Foo> sptr = make_unique<Foo>(); // C++14 권장
    }
    // 블록 스코프 확인
    cout << "before block\n";
    {
        auto ptr2 = std::make_unique<Foo>();
    }
    cout << "after block\n";
}

//-----------------------------
// 6) 학습용: 간단 스마트 포인터 구현
//-----------------------------
template<typename T>
class SPtr {
    T* data{};
public:
    explicit SPtr(T* d) : data(d) {}
    ~SPtr() { delete data; }

    SPtr(const SPtr&) = delete;
    SPtr& operator=(const SPtr&) = delete;

    SPtr(SPtr&& other) noexcept : data(other.data) { other.data = nullptr; }
    SPtr& operator=(SPtr&& other) noexcept {
        if (this != &other) {
            delete data;
            data = other.data;
            other.data = nullptr;
        }
        return *this;
    }

    T& operator* () const { return *data; }
    T* operator->() const { return  data; }
    explicit operator bool() const { return data != nullptr; }
};

class A {
    int* buf{};
public:
    A() : buf(new int[100] {}) { cout << "자원 획득!\n"; }
    ~A() { delete[] buf; cout << "자원 해제!\n"; }
    void some() { cout << "일반 포인터처럼 사용 가능\n"; }
};

void custom_sptr_demo() {
    // 전통 포인터
    {
        A* p = new A();
        p->some();
        delete p;
    }
    // 학습용 스마트 포인터
    {
        SPtr<A> sp(new A());
        sp->some();
    }
    // unique_ptr로 대체 가능
    {
        auto up = std::make_unique<A>();
        up->some();
    }
}

//-----------------------------
// main
//-----------------------------
int main() {
    basic_new_delete();
    class_allocation();
    pitfalls_demo();     // 설명용, 실행코드는 주석
    raii_demo();
    smart_ptr_demo();
    custom_sptr_demo();
    return 0;
}