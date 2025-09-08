````markdown
# C++ `typeid` 정리

## 1. 개념
- `typeid` 연산자  
  - 자료형, 변수, 식의 타입 정보를 **`const type_info&` 객체**로 반환  
  - `type_info::name()` : 타입의 문자열 표현(`const char*`) 반환  
  - 런타임에 객체의 실제 형식을 확인할 수 있다 (RTTI: Run-Time Type Information)

---

## 2. 주요 멤버 함수
- `name()` : 타입의 문자열 반환  
- `hash_code()` : 타입의 해시 코드 반환  
- `before()` : 두 타입의 정렬 순서 비교 (0 또는 1 반환)  
- 비교 연산자 : `==`, `!=` 지원  

---

## 3. 사용 예제

### (1) 기본 사용
```cpp
#include <iostream>
#include <typeinfo>
using namespace std;

int main() {
    cout << typeid(int).name() << endl;       // 기본 자료형
    cout << typeid(type_info).name() << endl; // 클래스
    cout << typeid(main).name() << endl;      // 함수
    cout << typeid(cout).name() << endl;      // 객체
}
````

---

### (2) 다형성 확인 (virtual 함수 필요)

```cpp
class A { public: virtual void vf() {} };
class B : public A {};
class C : public A {};
class D : public A {};

int main() {
    B b; C c; D d;
    A* arr[] = { &b, &c, &d };

    cout << typeid(*arr[0]).name() << endl; // B
    cout << typeid(*arr[1]).name() << endl; // C
    cout << typeid(*arr[2]).name() << endl; // D
}
```

---

### (3) 안전한 형변환

```cpp
class A { public: virtual void vf() {} };
class B : public A { public: bool b; };
class C : public A { public: int c; };

void memberPrint(A* a) {
    if (typeid(B) == typeid(*a)) {
        B* b = (B*)a;
        cout << typeid(b->b).name() << endl;
    }
    else if (typeid(C) == typeid(*a)) {
        C* c = (C*)a;
        cout << typeid(c->c).name() << endl;
    }
}
```

---

### (4) 템플릿에서 타입 확인

```cpp
template <typename T>
T mymax(T a, T b) {
    cout << typeid(T).name() << endl;
    return (a > b ? a : b);
}

int main() {
    mymax(3, 5);        // int
    mymax(3.2, 1.5);    // double
}
```

---

### (5) 참조 타입 확인

```cpp
int main() {
    bool rt = typeid(int) == typeid(int&); 
    cout << rt << endl;                      // true

    cout << typeid(int&).name() << endl;     // int
}
```

---

## 4. 주의사항

* `typeid(Widget).name()`의 문자열 결과는 **컴파일러마다 다름**
* 포인터/참조 여부는 **무시될 수 있음** (`int` vs `int&` → 같다고 판단)
* 다형성 구분은 **부모 클래스에 virtual 함수가 있어야 가능**

---

## 5. 시험 포인트

1. `typeid` 반환 타입? → `const type_info&`
2. `typeid(*p)`로 실제 자식 클래스 확인 조건? → 부모 클래스에 `virtual` 함수 필요
3. `typeid(int) == typeid(int&)` ? → `true`
4. `type_info` 주요 함수? → `name()`, `hash_code()`, `before()`, `==`, `!=`
5. 컴파일러마다 달라질 수 있는 것? → `name()` 반환 문자열

---

## 6. 예상 문제

* `typeid`와 `dynamic_cast`의 차이점
* 부모 클래스에 `virtual` 함수가 없을 때 `typeid(*ptr)` 결과?
* 템플릿에서 `typeid(T).name()` 출력 특징은?
* `typeid(int).name()`과 `typeid(int&).name()` 결과 비교

---