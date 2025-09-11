# `auto`가 원치 않은 타입으로 추론될 때 — 명시적 타입 초기치 관용구

## 요약
- `auto`는 편리하지만, **프록시 타입**(예: `std::vector<bool>::reference`) 같은 “보이지 않는” 타입을 **원치 않게** 추론할 수 있다.
- 이럴 때는 **명시적 타입 초기치 관용구(explicitly typed initializer idiom)** 를 사용한다:
  ```cpp
  T var = static_cast<T>(expr);
  // 또는
  auto var = static_cast<T>(expr);
````

* 효과: **의도한 실제 타입**으로 고정, 수명/의미가 불명확한 프록시 타입 저장을 회피.

---

## 왜 문제가 되나? (프록시 타입)

예를 들어 `std::vector<bool>::operator[]`는 `bool&`가 아니라
`std::vector<bool>::reference`라는 **프록시 객체**를 반환한다.
다음과 같은 코드에서:

```cpp
bool highPriority1 = features(w)[3];  // OK: 즉시 bool로 변환되어 값 저장
auto highPriority2 = features(w)[3];  // 문제 소지: 프록시 타입이 auto로 저장됨
```

* `features(w)`가 **임시 `vector<bool>`** 을 반환하면, `operator[]`가 돌려준 **프록시**는 그 임시 컨테이너에 **의존**한다.
* `auto`가 이 프록시 타입을 그대로 저장하면, **컨테이너가 소멸된 뒤**의 프록시가 되어 **미정의 동작(UB)** 이 발생할 수 있다.
* 반면 `bool`로 **즉시 변환**하면 값만 저장되므로 안전하다.

---

## 해결: 명시적 타입 초기치 관용구

```cpp
auto highPriority = static_cast<bool>(features(w)[3]);  // 의도적으로 bool로 고정
```

의도가 분명해지고, 프록시 수명 문제를 피한다.

---

## 추가 예: 의도 표현 (축소 변환)

```cpp
double calcEpsilon();
float  ep1 = calcEpsilon();                 // 묵시적 축소
auto   ep2 = static_cast<float>(calcEpsilon()); // 의도적 축소를 명시
```

---

## 참고: `auto`, `auto&`, `decltype((x))`

```cpp
int  a  = 0;
auto& v1 = a;        // int&
decltype((a)) v2 = a; // int&  (괄호로 인해 lvalue → 참조)
int arr[]{1,2,3};
auto& v3 = arr[0];   // int&
```

프록시를 반환하는 API(예: `vector<bool>::operator[]`)에서는 **값으로 가져오려면** `static_cast<bool>` 로 **명시적 변환**을 권장.

---

## 피해야 할 패턴

```cpp
// 프록시 타입을 그대로 auto에 저장
auto someVar = expression_returning_proxy_type;  // 지양
```

## 권장 패턴

```cpp
bool v = static_cast<bool>(expression_returning_proxy_type); // 지향
// 또는
auto v  = static_cast<bool>(expression_returning_proxy_type);
```

---

## 시험 포인트

1. `std::vector<bool>::operator[]` 반환 타입이 왜 문제인가? → 프록시(reference)라서 수명/의미 불명확.
2. `auto`가 프록시 타입을 저장할 때 발생 가능한 현상? → 컨테이너 수명 종료 후 **UB**.
3. 해결책? → **명시적 타입 초기치 관용구** (`static_cast<T>(expr)`).
4. 축소 변환을 안전하게/명확하게 표현하는 법? → `static_cast<float>(...)`.
5. `auto&` vs `decltype((x))` 의 공통점? → 둘 다 lvalue 참조로 바인딩 (괄호가 lvalue로 만든다).

---
