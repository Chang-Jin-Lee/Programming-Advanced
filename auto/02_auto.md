---

# C++ `auto` 키워드 정리 2 (심화)

## 1. 기본 개념

* 초기화식의 타입에 맞춰 자동 추론.
* `auto`만 쓰면 const/ref는 무시.
* `auto&` → 참조 유지, const도 유지.
* `auto*` → 포인터 유지.
* `auto&&` → 보편 참조(universal reference).

---

## 2. 초기화 방식

```cpp
int a = 1;
int a(1);
int a{1};      // C++11
int b(a);      // copy
int(10);       // 이름 없는 int 임시객체

auto c = a;    // int
int x = int(); // 0 초기화
auto auto_x = x; // int
```

---

## 3. 포인터와 참조

```cpp
int n = 1.2;
auto& b = n;   // int& (참조 유지)
auto* c = &b;  // int* (포인터 유지)

Widget x;
auto o  = x;   // Widget (복사)
auto* a1 = &x; // Widget*
auto  a2 = &x; // Widget*
auto& r1 = x;  // Widget&
auto  r2 = x;  // Widget
```

---

## 4. 타입 추론 규칙

```cpp
const int& crx = x;
auto auto_crx1 = crx;  // int (const/ref 무시)

volatile int vx = 1024;
auto avx = vx;         // int (volatile 무시)

const int* pi = &i;
auto p = pi;           // const int* (포인터 const 유지)

const int c = 0;
auto& rc = c;          // const int& (auto&는 const 유지)
```

---

## 5. auto&& (보편 참조)

* 좌측값 → &
* 우측값 → &&

```cpp
int d = 10;
auto&& r1 = 5;          // int&&
auto&& r2 = d;          // int&
auto&& r3 = std::move(d); // int&&

double d1 = 1.0;
auto&& dr1 = d1;        // double&
auto&& dr2 = 1.0;       // double&&
```

---

## 6. 활용

1. 함수 포인터

```cpp
auto pfTest = TestFunc; // void(*)(int)
```

2. 배열, 벡터

```cpp
int buffer[5] = {0,1,2,3,4};
for (auto i : buffer) { cout << i; }
```

3. 함수 반환

```cpp
int add(int x,int y){return x+y;}
auto sum = add(5,6); // int
// C++14: auto Add(int x,int y){return x+y;}  // 반환형 추론
```

4. 클래스

```cpp
auto obj = MyClass(); // MyClass
// 멤버변수에는 auto 사용 불가
```

---

## 7. 람다와 auto

1. 반환형 추론

```cpp
auto test1 = [](int* a)->int& { return *a; }; // int& 리턴
auto test2 = [](int* a) { return *a; };       // int 리턴
```

2. 제네릭 람다 (C++14\~)

```cpp
auto func = [](auto x, auto y) { return x + y; };
cout << func(1.1, 5) << " " << func(3.7, 23.1);
```

---

# 시험 대비 문제

1. `auto x = 0.123;` 의 타입은? (0.123f로 바꾸면?)
2. `const int& a = 0; auto b = a;` 의 b 타입은?
3. `auto& rc = const int c = 10;` → rc의 타입은?
4. `int d = 10; auto&& r = d;` 와 `auto&& r = 5;` 각각의 타입은?
5. `auto arr = {1,2,3};` 의 타입은?
6. 함수 포인터 `auto pf = TestFunc;` 의 타입은?
7. 람다 `auto f = [](auto x, auto y){return x+y;};` 에서 `f(2,3.5)` 의 반환 타입은?

---