# C++ `decltype` / `declval` / `decltype(auto)` 정리 (심화)

## 1) 핵심 정의
- `decltype(expr)` : **컴파일 타임**에 `expr`의 **타입을 그대로** 추출한다.  
- 타입 연산 전용이며 런타임 실행과 무관하다.

### 기본 규칙
- **괄호 없는 식별자 표현식(id-expression)** → 원래 타입을 그대로 가져온다.  
  예) 변수 이름, 멤버 이름, 함수 이름, enum 이름 등
- **식별자 표현식이 아닌 일반 표현식**일 때는 value category 규칙:
  - `prvalue` → `T`
  - `lvalue`  → `T&`
  - `xvalue`  → `T&&`
- **참조/const/volatile 보존**: `decltype`은 원형을 최대한 보존한다.  
  (반면 `auto`는 대체로 `const`/참조를 탈락시킨다.)

---

## 2) 자주 나오는 패턴과 답
### A. 변수/참조/우측값참조
```cpp
int a = 3;
decltype(a)   b = 2;  // int
int& ra = a;
decltype(ra)  rb = a; // int&
int&& x = 3;
decltype(x)   y = 2;  // int&&
````

### B. 멤버 접근

```cpp
struct A { double d; };
A* aa = nullptr;
decltype(aa->d) dd = 0.1; // double
```

### C. 괄호 유무에 따른 차이

```cpp
int c = 3;
decltype(c)   c1 = 2; // int
decltype((c)) c2 = c; // int&  ← 괄호로 인해 lvalue 처리
```

### D. 함수 / 함수식

```cpp
const int&& fx();          // 선언만 있어도 됨(호출 X)
decltype(fx())    t1;      // const int&&  (반환 타입 그대로)
decltype(fx)      t2;      // const int&& ()  ← 함수 타입
```

### E. `auto` 와 차이

```cpp
const int i = 4;
auto      j = i;  // int          (const 탈락)
decltype(i) k = i;  // const int  (보존)
```

```cpp
int arr[10];
auto        arr2 = arr;   // int*     (배열 → 포인터 decay)
decltype(arr) arr3{};     // int[10]  (배열 타입 그대로)
```

### F. `std::vector::operator[]` 반환

```cpp
std::vector<int> v{10, 20};
auto             a = v[0];        // int
decltype(v[0])   r = v[0];        // int&  ← 내부적으로 참조를 반환
```

### G. 3항 연산자(조건 연산자)와 value category

```cpp
int x = 0, y = 0;
const int cx = 40, cy = 50;
double d1 = 3.14, d2 = 9.8;

decltype(x * y)   t_xy;      // int     (prvalue)
decltype(cx * cy) t_cxcy;    // int     (prvalue)

decltype(d1 < d2 ? d1 : d2) t_dd;        // double&  (lvalue)
decltype(x  < d2 ? x  : d2) t_mixed;     // double   (타입 혼합 → 참조 아님)
```

---

## 3) `declval` / `decltype(auto)`

### `std::declval<T>()`

* 생성자를 호출하지 않고도 “존재한다고 가정한” `T` 객체를 표현해 **멤버 타입**에 접근할 때 사용.
* 오직 **컴파일 타임**에만 쓰이며 런타임에 쓰면 오류.

```cpp
#include <utility>

template <typename T>
decltype(std::declval<T>().f()) call_f_and_return(T& t) {
  return t.f();
}
```

### `decltype(auto)`

* C++14\~. 반환 타입을 **원형 그대로** 추론(참조/const 포함)하려는 경우 사용.

```cpp
template <typename T>
decltype(auto) call_f_and_return2(T& t) {
  return t.f();  // f()가 참조를 반환하면 참조 유지
}
```

---

## 4) `auto` vs `decltype` 비교 요점

| 항목       | auto           | decltype          |
| -------- | -------------- | ----------------- |
| const/참조 | 보통 탈락          | 보존                |
| 배열       | 포인터로 decay     | 배열 타입 유지          |
| 목적       | 값 기반 **타입 추론** | 식 기반 **타입 추출/보존** |

---

## 5) 실전 테크닉

### A. 3항 연산자 반환에서 & 제거하기

동일 타입 두 인수를 3항으로 묶으면 `decltype` 결과가 참조가 될 수 있다.
일관된 값 타입을 원하면 `std::remove_reference`로 벗겨서 사용.

```cpp
template<typename T1, typename T2>
auto fpmin_value(T1 x, T2 y)
  -> typename std::remove_reference<decltype(x < y ? x : y)>::type {
  return x < y ? x : y;
}
```

### B. 템플릿 함수 반환 타입

* C++11: 후행 반환 형식 사용

```cpp
template <typename T, typename U>
auto add(T t, U u) -> decltype(t + u) { return t + u; }
```

* C++14+: `auto` 또는 `decltype(auto)` 선택

  * 참조/const 보존 필요 시 `decltype(auto)`

---

## 6) 자주 나오는 시험 문제

[Q]
1. `decltype(a)` 와 `decltype((a))` 의 차이를 설명하라.
2. `auto` 와 `decltype` 의 차이와 각각의 쓰임새를 비교하라.
3. `decltype(v[0])` 가 `int&` 인 이유를 설명하라.
4. `decltype(fx())` vs `decltype(fx)` 의 결과를 쓰고 그 차이를 설명하라.
5. `decltype(d1 < d2 ? d1 : d2)` 가 `double&` 인 이유를 설명하라.
6. `std::declval` 의 용도와 런타임 사용 가능 여부를 설명하라.
7. 동일 타입 두 인수를 받는 3항 반환의 참조 문제를 어떻게 해결할 수 있는가?

[A]
1. `decltype(a)`는 변수 선언 타입, `decltype((a))`는 표현식 값 범주 반영 → lvalue면 `T&`.
2. `auto`는 초기값으로부터 추론(cvr 제거), `decltype`은 표현식 타입/범주 그대로.
3. `v[0]`은 lvalue → `decltype(v[0])` = `int&`.
4. `decltype(fx())`는 호출 결과 타입, `decltype(fx)`는 함수 타입.
5. `d1`·`d2` 모두 lvalue `double` → 삼항식도 lvalue → `double&`.
6. `std::declval`은 unevaluated 문맥에서 가짜 `T&&` 생성, 런타임 사용 불가.
7. 포인터 경유 반환 or `std::ref`/`std::cref`로 참조 문제 해결.

---