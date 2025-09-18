# C++ 보편 참조(Universal Reference), `std::forward`, 완벽 전달, 복사 생략(Copy Elision)

## 0) 요약

*** 보편 참조(Universal / Forwarding reference)** : `T&& `** (템플릿 인자 T가 ‘추론’될 때만)** 또는 `auto&& `.
lvalue도 rvalue도** 모두** 받는다.

*** 참조 한정 소거(reference collapsing)** 

& 와&& 가 겹치면 결과는 아래 규칙을 따른다 :

`& + & = > &`, `& + && = > &`, `& & +& = > &`, `& & +&& = > && `
* **`std::forward<T>(x)`* * : x를 * *원래 값 범주 그대로 * *내보낸다.
T가 `U&`이면 lvalue로, `U& & `이면 rvalue로 캐스팅(조건부 캐스팅).
* **왜 필요한가 * *: 템플릿 래퍼 / 팩토리 / 컨테이너 등에서 * *불필요한 복사 없이 * *호출자 인자의 * *값 범주 보존 * *(완벽 전달).
* **복사 생략(Copy Elision) * *: 컴파일러가 복사 / 이동을 * *생략 * *할 수 있는 최적화.
C++17에서는 \ * \ * 일부 시나리오(함수에서 prvalue 반환)\ * \ * 에서 * *강제(elision guaranteed) * *.

-- -

## 1) 보편 참조(Universal / Forwarding Reference)

### 성립 조건

* `template <class T> void f(T&& x); `** 처럼 T가 추론될 때** 의 `T&& `
* `auto&& x = expr; `
*** 아님** : `const T&& ` / `const auto&& ` (이건 * *항상 * *rvalue만, 보편 참조 아님)

### 값 범주에 따른 T 연역

* 인자가 * *lvalue * *면 `T`는 `U&`로 연역되어 * *`T& & ` ⇒ `U& && ` ⇒ `U&`* * (lvalue)
*인자가 * *rvalue * *면 `T`는 `U`로 연역되어 * *`T& & ` ⇒ `U& & `* * (rvalue)

-- -

## 2) `std::forward`와 완벽 전달

* 서명(개념적으로) :

	```cpp
	template<class T> constexpr T&& forward(remove_reference_t<T>& t) noexcept;
template<class T> constexpr T&& forward(remove_reference_t<T>&& t) noexcept;
```
* 효과:

*`forward<U& > (x)`  →** lvalue** 로
* `forward<U&& > (x)` → \* \* rvalue(xvalue)\* \* 로
* 용도: 보편 참조 파라미터를 다른 함수에** 그대로** 넘길 때(팩토리, 래퍼, 컨테이너 삽입, 생성자 포워딩 등)

> 주의: **이름을 가진 변수** 는 표현식으로 쓰면** 항상 lvalue** .
> rvalue로 다시 넘기려면 `std::move` 또는 `std::forward`가 필요.

-- -

## 3) 흔한 실수와 방지법

* `T t`로 받는 템플릿 : **값 전달** 이라** 항상 lvalue** 로 취급 → rvalue 정보 유실
→** `T&& ` + `std::forward<T>(t)`* * 사용
* `const` 섞어서 `const T && `: 보편 참조 * *아님**, 이동도 대부분 막힘.특별한 목적 없으면 지양.
* rvalue로 받은 매개변수도** 이름이 있는 순간 lvalue** → 다시 넘길 때 `std::move` 또는 `std::forward`.
* `std::move` 남용 : 정말로 * *더 이상 쓰지 않을 객체** 에만.
* `vector` 등 컨테이너 : **이동 생성자 / 대입에 `noexcept`* * 없으면 * *복사로 폴백 * *가능.

-- -

## 4) 복사 생략(Copy Elision) — RVO / NRVO, Passing Temporary

*** RVO** : `return T{ ... }; ` 과 같은** 무명 임시** 반환 → 복사 / 이동 * *생략 * *가능
* **NRVO * *: 함수 내 * *명명된 지역변수 * *`T obj; return obj; ` →** 생략 시도** (강제 아님)
*** C++17 * *: prvalue 반환 시 복사 생략 * *강제 * *(언어 차원 보장).NRVO는 여전히 * *최적화 * *.
* **임시를 값으로 인자 전달 * *: 컴파일러에 따라 복사 / 이동 생략될 수 있음(보장X).

-- -

## 5) 값 범주 짧게 정리

*** lvalue** : 이름 / 주소로 식별 가능한 것(좌변 가능)
* **prvalue * *: “순수 값”(C++17부턴 문맥에 맞춰 직접 초기화)
* **xvalue * *: 곧 소멸 / 자원 회수 가능한 glvalue(주로 `std::move(x)` 결과)
* **rvalue * * = prvalue ∪ xvalue

-- -

## 6) 예상 시험문항(단답 / 서술)

1. * *보편 참조가 성립하는 조건 * *을 쓰고, `const T&& `가 왜 보편 참조가 아닌지 설명하라.
2. * *참조 겹침 규칙 * *네 가지 결과를 표로 쓰라.
3. `template<class T > void wrap(T && x) { callee(x); }`에서 rvalue 전달 시 왜 이동이 안 되는지, 해결책은 ?
4. `std: : forward`와 `std::move`의 차이와 사용 위치를 설명하라.
5. C++17의 * *강제 복사 생략 * *이 의미하는 바를 RVO / NRVO와 대비해 서술하라.
6. `vector`가 * *이동 대신 복사 * *를 선택하는 경우와 원인을 설명하라.
7. `const` 객체에 `std::move`를 적용했을 때 실제로 * *이동이 안 되는 * *이유는 ?
8. 아래 코드에서 각 호출에 선택되는 오버로드는 ? (lvalue / rvalue / const 조합 문제 출제)

-- -

## 7) 체크리스트(구현 시)

* 래퍼 / 팩토리: `template<class T > f(T && x) { g(std::forward<T>(x)); }`
* 소유 자원 클래스 : Rule of Five(+`noexcept` 이동)
* 컨테이너 성능 : 이동 생성자 / 대입에 `noexcept`
* 반환 최적화 : `return T{ ... }; ` (C++17 혜택)
* 불필요한 `const`로 이동 막지 않기

---