# C++ `typeid` & RTTI 정리(추가 예제)

## 1. 개념
- `typeid` 연산자
- 런타임에 객체의 형식을 확인 가능(RTTI: Run - Time Type Information)
- 자료형 / 변수 / 식을 입력받아 * *`const type_info & ` 객체 * *반환

- C++에서 RTTI 제공 키워드
- `dynamic_cast< > `
- `typeid`

-- -

## 2. 주요 특징
1. * *참조, const, volatile은 무시됨**
-`typeid(char)` == `typeid(const char)` == `typeid(char&)`

2. * *포인터는 const, volatile 구분**
-`typeid(char*)`, `typeid(const char*)`, `typeid(volatile char*)` 는 다름

3. * *클래스 타입 출력 시 `class` 키워드 포함됨 * *

4. * *다형성(polymorphism) 구분 * *
-부모 클래스에 `virtual` 함수가 있어야 실제 자식 클래스 구분 가능
- `typeid( * pb)` → `Derived` 출력

-- -

## 3. 출력 예시

```cpp
cout << typeid(char).name();          // char
cout << typeid(int).name();           // int
cout << typeid(__int64).name();       // __int64

cout << typeid(char&).name();         // char (참조 무시됨)
cout << typeid(const char).name();    // char (const 무시됨)
cout << typeid(volatile char).name(); // char (volatile 무시됨)

cout << typeid(char*).name();         // char*
cout << typeid(const char*).name();   // const char*
cout << typeid(volatile char*).name();// volatile char*

cout << typeid(Base).name();          // class Base
cout << typeid(Derived).name();       // class Derived

Derived* pd = new Derived;
Base* pb = pd;

cout << typeid(pb).name();   // class Base* (포인터 자체의 타입)
cout << typeid(*pb).name();  // class Derived (역참조, 실제 객체 타입)
cout << typeid(pd).name();   // class Derived*
cout << typeid(*pd).name();  // class Derived
````

-- -

## 4. 시험 포인트

1. `typeid(char)`와 `typeid(const char)` 비교 결과 ? → 동일
2. `typeid(char*)`와 `typeid(const char*)` 비교 결과 ? → 다름
3. 부모 포인터 `Base* pb = new Derived; ` 에서

* `typeid(pb)` → `Base*`
* `typeid(* pb)` → `Derived` (단, `Base`에 `virtual` 함수 있어야 함)
4. RTTI 제공 키워드 두 가지 ? → `dynamic_cast< > `, `typeid`

-- -
