````markdown
# C++ `decltype` / `declval` / `decltype(auto)` 정리

## 1. decltype
- **컴파일 타임**에 식의 타입을 추출하는 키워드  
- 문법:  
  ```cpp
  decltype(식) 변수명;
````

* 런타임이 아닌 **타입 연산 전용**

---

### (1) 기본 규칙

1. **괄호 없는 id-expression (식별자 표현식)**

   * 변수 이름, 함수 이름, enum, 멤버 변수 등 → 해당 식의 타입 그대로

   ```cpp
   int a = 3;
   decltype(a) b = 2;  // int
   ```

2. **참조는 그대로 유지**

   ```cpp
   int& ra = a;
   decltype(ra) rb = a;  // int&
   ```

3. **괄호로 감싸면 lvalue → T&**

   ```cpp
   int c = 3;
   decltype(c) c1 = 2;   // int
   decltype((c)) c2 = c; // int&   <-- 괄호 때문에 lvalue
   ```

4. **연산식**

   * `prvalue` → T
   * `lvalue` → T&
   * `xvalue` → T&&

   ```cpp
   int a, b;
   decltype(a + b) x;   // int
   decltype(a = b) y;   // int& (대입은 lvalue)
   ```

---

### (2) auto 와의 차이

* `auto` : 값을 기준으로 타입 추론 (const, reference 무시)
* `decltype` : 원래 타입 그대로 유지

```cpp
const int i = 4;
auto j = i;      // int (const 무시)
decltype(i) k=i; // const int
```

```cpp
int arr[10];
auto arr2 = arr;     // int* (배열 decay)
decltype(arr) arr3;  // int[10] (배열 타입 그대로)
```

---

## 2. declval

* **생성자 호출 없이** 객체 타입을 “가상의 값”으로 만들어 멤버 타입에 접근 가능
* `#include <utility>` 필요
* 런타임 실행 불가 (컴파일 타임 전용)

```cpp
template <typename T>
decltype(std::declval<T>().f()) call_f_and_return(T& t) {
    return t.f();
}
```

---

## 3. decltype(auto)

* **C++14** 부터 지원
* 반환 타입을 그대로 추론 (참조/const 포함)
* `auto` 대신 `decltype(auto)` 를 쓰면 반환 타입을 **원형 유지** 가능

```cpp
template <typename T>
decltype(auto) call_f_and_return(T& t) {
    return t.f();
}
```

---

## 4. 함수 리턴 타입과 decltype

* C++11: 후행 반환 타입 필요

  ```cpp
  template <typename T, typename U>
  auto add(T t, U u) -> decltype(t + u) {
      return t + u;
  }
  ```
* C++14 이후: `auto` 자동 추론 가능
* 참조/const 보존하려면 `decltype(auto)`

---

## 5. 시험 포인트

1. `decltype(a)` vs `decltype((a))` 차이

   * 전자: int
   * 후자: int&
2. `decltype` 과 `auto` 의 차이
3. `declval` 의 용도: 생성자 호출 없이 타입 추출
4. `decltype(auto)` 의 장점: 참조/const 유지
5. `decltype(a+b)` 의 결과는? (연산식의 value category 확인)

---