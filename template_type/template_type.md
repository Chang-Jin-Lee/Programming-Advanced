# C++ 템플릿 타입 추론 규칙 정리

## 개요
템플릿 타입 추론에는 항상 **두 축**이 있습니다.

- **T**: 템플릿 매개변수 자체
- **ParamType**: 함수 파라미터에 기술된 타입(예: `T`, `const T&`, `T*`, `T&&` 등)

추론 규칙은 **ParamType**의 형태(값/참조/포인터/보편참조)에 따라 달라집니다.  
또한 템플릿 타입 추론은 일반적으로 `auto` 추론과 유사하게 **참조를 제거**해서 생각하면 이해가 빠릅니다.

---

## 경우 1) ParamType이 값(value) 전달 (`T param`)
- 인수의 **참조성(&)**, **상수/휘발성(const/volatile)** 은 **무시**됩니다.
- 즉, `T`와 `param` 모두 **비참조**이며 **top-level const/volatile 제거** 후의 타입으로 추론됩니다.

예시:
```cpp
template <typename T>
void f(T param);

int x = 27;            // int
const int cx = x;      // const int
const int& rx = x;     // const int&

f(x);   // T=int,        param=int
f(cx);  // T=int,        param=int
f(rx);  // T=int,        param=int
````

---

## 경우 2) ParamType이 참조/포인터 (보편참조가 아님)

### (a) 참조(`T&`, `const T&`)

* 인수(expr)가 참조라면 **참조는 무시**하고 **기저 타입**으로 `T`를 맞춘 뒤,
* **ParamType**에 써준 `const`/`volatile`이 **그대로 반영**됩니다.

```cpp
template <typename T>
void f(T& param);      // 또는: void f(const T& param);

int x = 27;
const int cx = x;
const int& rx = x;

f(x);   // param=int&,          T=int
f(cx);  // param=const int&,    T=const int
f(rx);  // param=const int&,    T=const int
```

`const T&`로 받으면 `T`는 **기저 타입**으로만 추론됩니다.

```cpp
template <typename T>
void f(const T& param);

f(x);   // param=const int&,  T=int
f(cx);  // param=const int&,  T=int
f(rx);  // param=const int&,  T=int
```

### (b) 포인터(`T*`, `const T*`)

* 포인터 자체의 **top-level const** 는 값 전달 시 제거되지만,
* **가리키는 대상의 const(= low-level const)** 는 **보존**됩니다.

```cpp
template <typename T>
void f(T* param);

int x = 27;
const int* px = &x;

f(&x);  // param=int*,        T=int
f(px);  // param=const int*,  T=const int
```

---

## 경우 3) ParamType이 **보편 참조(Universal Reference)** (`T&&`)

* 인수가 **lvalue**면: `T`는 **lvalue 참조**로 추론, `param`도 **lvalue 참조**가 됩니다.
* 인수가 **rvalue**면: 일반 규칙처럼 `T`는 비참조 타입, `param`은 **rvalue 참조**가 됩니다.

```cpp
template <typename T>
void f(T&& param); // Universal reference

int x = 27;
const int cx = x;
const int& rx = x;

f(x);     // x: lvalue    → T=int&,        param=int&
f(cx);    // cx: lvalue   → T=const int&,  param=const int&
f(rx);    // rx: lvalue   → T=const int&,  param=const int&
f(27);    // 27: rvalue   → T=int,         param=int&&
```

> 참고: **참조 붕괴 규칙**에 따라 `T& & → T&`, `T& && → T&`, `T&& & → T&`, `T&& && → T&&`.

---

## 포인터의 const 두 겹: `const char* const`

```cpp
template <typename T>
void f(T param); // 값 전달

const char* const ptr = "abc";
f(ptr); // T=const char*  (포인터 자체의 top-level const는 사라지고,
        //                  가리키는 대상의 const는 보존)
```

---

## 배열/함수의 **포인터 붕괴(decay)**

* **값 전달** 매개변수로 받을 때: 배열/함수 **이름**은 **포인터**로 붕괴합니다.
* **참조**로 받을 때: **붕괴하지 않고** 원형을 유지합니다.

```cpp
// 값 전달
template <typename T>
void f(T param);

const char name[] = "abc";
f(name); // T=const char*, param=const char*   (배열→포인터)


// 참조 전달
template <typename T>
void g(T& param);
g(name); // T=const char[4], param=const char (&)[4]
```

### 배열 크기 얻기 예시

```cpp
template <typename T, std::size_t N>
constexpr std::size_t arraySize(T(&)[N]) noexcept { return N; }
```

### 함수 포인터/참조 추론

```cpp
void someFunc(int, double);  // 타입: void(int,double)

template <typename T>
void f1(T param);  // 값 전달 → 함수 포인터

template <typename T>
void f2(T& param); // 참조 전달 → 함수 참조

f1(someFunc); // T=void(*)(int,double)
f2(someFunc); // T=void(&)(int,double)
```

---

## 시험 포인트 정리

1. `T param` vs `T&/const T&/T*` vs `T&&` 의 추론 차이 서술
2. **보편 참조**에서 lvalue/rvalue 인수에 따른 `T`/`param` 타입 변화
3. 포인터의 **top-level const** vs **low-level const** 구분
4. 배열/함수 이름의 **포인터 붕괴**와 **참조로 받을 때의 차이**
5. `const char* const` 사례에서 `T`가 어떻게 추론되는지
6. 참조 붕괴 규칙 네 줄 암기

---

## 자주 틀리는 포인트

* 값 전달(`T param`)일 때 **const/volatile/참조가 제거**된다는 점
* `T&&`가 항상 rvalue 참조가 아님. **보편 참조**일 수 있음
* 포인터의 **두 겹 const** 중 어느 쪽이 보존되는지 혼동
* 배열/함수는 값 전달 시 **자동 포인터 붕괴**