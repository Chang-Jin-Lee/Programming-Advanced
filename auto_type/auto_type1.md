# C++ `auto` 타입 추론 규칙 정리

## 개요
- `auto` 타입 추론은 * *템플릿 타입 추론(T / ParamType) * *과 거의 동일하게 동작합니다.
- 차이점: **중괄호 초기화 * *가 주어졌을 때 `auto`는 * *`std::initializer_list`* * 를 특별 취급합니다.
- C++17부터 `auto x{ 27 }; `의 추론 결과가** `int`** 로 바뀌는 등, 표준 버전에 따른 차이가 있습니다.

-- -

## 1) `auto`와 템플릿 추론의 대응
- `auto`는 템플릿의 * *`T`* * 와 동일한 역할
- 변수 선언부의 타입 지정자(예: `const auto& `)는 템플릿의 * *`ParamType`* * 역할

예)
```cpp
auto x = 27;       // ParamType: auto
const auto cx = x;  // ParamType: const auto
const auto& rx = x; // ParamType: const auto&
````

-- -

## 2) 세 가지 경우

### 경우 1: 값(value) 전달(`auto`)

	* **참조 / const / volatile** 이 제거된 * *비참조 타입 * *으로 추론

	```cpp
	auto x = 27;       // int
const auto cx = x;  // const int
```

### 경우 2: 참조 / 포인터(보편 참조가 아님) (`const auto& `, `auto*` 등)

* `&`로 받으면 * *lvalue 참조 * *로, `const`를 명시하면 * *상수성 유지 * *

```cpp
const auto& rx = x;  // const int&
```

### 경우 3: 보편 참조(Universal reference) (`auto& & `)

	* 초기값이 * *lvalue * *면 `auto`는 * *lvalue 참조 * *로 추론되어 * *`&`* * 로 수렴
	* 초기값이 * *rvalue * *면 * *`& & `* * 유지

	```cpp
	auto&& uref1 = x;  // x는 lvalue → uref1: int&
auto&& uref2 = 27; // rvalue     → uref2: int&&
```

-- -

## 3) 배열 / 함수의 포인터 붕괴(decay)

* **값 * *으로 받을 때 : 배열 / 함수 * *이름 * *은 * *포인터 * *로 붕괴
* **참조 * *로 받을 때 : **붕괴하지 않고 원형 유지 * *

```cpp
const char name[] = "abc";
auto  arr1 = name;  // const char* (배열 → 포인터)
auto& arr2 = name;  // const char (&)[4] (참조로 유지)

void someFunc(int, double);
auto  func1 = someFunc; // void (*)(int,double)
auto& func2 = someFunc; // void (&)(int,double)
```

-- -

## 4) `auto`와 중괄호 초기화(`std::initializer_list`)

	* `auto` 변수의 * *중괄호 초기치 * *는 특별 취급됩니다.

	| 선언 | C++11 / 14 | C++17                        |
	| ---------------- - | ---------------------------- | ---------------------------- |
	| `auto x1 = 27; ` | `int` | `int`                        |
	| `auto x2(27); ` | `int` | `int`                        |
	| `auto x3 = { 27 }; ` | `std::initializer_list<int>` | `std::initializer_list<int>` |
	| `auto x4{ 27 }; ` | `std::initializer_list<int>` | **`int`* * |

	*서로 다른 타입이 섞인 중괄호 초기치는 * *`std::initializer_list<T>`* * 의 `T`를 추론할 수 없어 * *컴파일 에러 * *가 납니다.

	```cpp
	// auto x = {1, 2, 3.0}; // 오류
	```

	* 템플릿 인수 추론에서는 * *그 자체만 * *으론 중괄호 초기치를 받을 수 없습니다.

	```cpp
	template <typename T>
void f(T param);

// f({11, 23, 9}); // 오류: T를 추론할 수 없음
```

* 대신 * *`std::initializer_list<T>`* * 를 명시하면 추론 성공 :

```cpp
template <typename T>
void f(std::initializer_list<T> il);

f({ 11, 23, 9 }); // OK, T=int
```

-- -

## 5) 실전 팁

* **참조 / const 보존 * *이 필요한 반환에는 `decltype(auto)` 사용
*** 보편 참조 + `std::forward`* * 로 값 범주 보존
* 배열 크기가 필요하면 참조 매개변수 템플릿으로 * *배열 길이 추론 * *(`T(&)[N]`)

---

## 6) 자주 나오는 질문(시험 포인트)

1. `auto& & `가 항상 rvalue 참조인가 ? → * *아님.* * lvalue에 바인딩되면 * *`&`* * 로 수렴.
2. `auto`와 템플릿 추론의 관계 ? → `auto` ⇔ `T`, 선언부 수식자 ⇔ `ParamType`.
3. `auto`와 중괄호 초기화 차이 ? → `std: : initializer_list` 특별 취급, C++17에서 `auto x{ 27 }`는 `int`.
4. 배열 / 함수 포인터 붕괴 규칙 ? → * *값으로 받을 때만 * *decay, 참조로 받으면 유지.
5. 서로 다른 타입이 섞인 `{...}` 를 `auto`로 받을 수 있는가 ? → * *불가 * *(추론 실패).

---