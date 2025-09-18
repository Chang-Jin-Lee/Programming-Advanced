# C++ 임시(익명) 객체 & 값 카테고리 정리

## 1) 임시(익명) 객체란?

* **임시 객체(temporary)**, **익명 객체(anonymous)**: 이름이 없는, 일시적으로만 존재하는 객체.
* 예시

  ```cpp
  int(1);     // 값 1을 담는 임시 int
  A();        // A 타입의 임시 객체
  A().print();// 생성 직후 곧바로 멤버 함수 호출
  ```
* 임시 객체는 보통 **rvalue**(정확히는 prvalue 또는 xvalue)로 취급되며, **표현식이 끝나면 곧바로 소멸**한다.

---

## 2) `decltype` / `auto` 와 임시 객체

* `decltype(A())` : `A()`는 **prvalue** → `decltype(prvalue)` 는 **`A`**
* `auto obj1 = A();` : 대입 초기화 → `obj1`의 타입은 **`A`**
* `auto&& obj2 = A();` : 임시에 바인딩 → **`A&&`** (rvalue 참조)
* `decltype(std::move(obj))` : `std::move(obj)` 는 **xvalue** → 타입은 **`A&&`**
  (단, `A&& v;` 형태의 **미초기화 rvalue 참조 변수 선언은 불가**하니 주의)

---

## 3) 값 카테고리(Value Category)

* **glvalue**: 객체/비트필드/함수의 **식별성**을 나타내는 표현식

  * **lvalue**: glvalue 중 xvalue가 아닌 것
  * **xvalue**: “곧 소멸될 수 있는” glvalue (주로 `std::move(x)` 등)
* **rvalue**: **prvalue** 또는 **xvalue**

  * **prvalue**: 값을 계산해 초기화하는 표현식 (전통적 “임시”)
* 핵심 기억:

  * `*p` (역참조)는 **lvalue**
  * 조건 연산자 `(cond ? a : b)` 는 두 피연산자가 같은 lvalue 타입이면 **lvalue**를 돌려줄 수 있음
  * 리터럴 `42` 는 **prvalue**, `std::move(x)` 는 **xvalue**

---

## 4) rvalue 참조(`&&`)와 이동(move)

* `int&& r = 42;` : 임시값에 바인딩 (**rvalue 참조**)
* `int&& rr = std::move(x);` : **lvalue를 xvalue로 변환**하여 이동 의미 부여
* **주의**: 매개변수로 받은 `int&& a` 자체는 **식으로 쓰일 때 lvalue**다. (타입은 rvalue 참조지만, 이름을 가진 변수는 lvalue 표현식)

---

## 5) 보편 참조(Forwarding Reference)

* 템플릿에서 `T&&` 가 **형식 인자 추론 대상**일 때, 인수의 값 범주에 따라 `T&` 또는 `T`가 되어 **`&`/`&&`로 붕괴**한다.

  ```cpp
  auto&& u1 = x;   // x가 lvalue → u1은 int&
  auto&& u2 = 0;   // rvalue     → u2는 int&&
  ```

---

## 6) `const &&` (드문 사용)

* **수정 불가한 rvalue 참조** 시그니처. 보통 라이브러리 내부에서 **오용 방지**, **오버로드 분리**, **반환 값 범주 유지** 등에 제한적으로 사용.

---

## 7) 멤버 함수의 **참조 한정자(ref-qualifier)**

* 멤버 함수에 `&`, `&&`, `const &`, `const &&` 를 붙여 **호출 주체의 값 범주/상수성에 따라 오버로드** 가능.

  ```cpp
  struct S {
    std::string&       get() &      { return s; }           // lvalue 대상에서
    std::string        get() &&     { return std::move(s); } // rvalue 대상에서
    const std::string& get() const& { return s; }            // const lvalue
    std::string        get() const&&{ return s; }            // const rvalue
  };
  ```

---

## 8) C++17: \*\*임시 실체화(temporary materialization)\*\*와 prvalue

* C++17 전: 클래스형 prvalue는 **항상 임시 객체**를 생성 → 복사/이동 제약에 걸릴 수 있었음.
* C++17 후: prvalue는 **필요한 문맥에서 직접 대상 객체를 초기화**(개념적 임시 제거) → 복사/이동 불필요한 경우가 많아짐.

  * 복사/이동 불가능 타입도 **직접 초기화**로 사용 가능해지는 케이스가 생김.

---

## 9) 자주 나오는 포인트(시험 대비)

1. `decltype(prvalue)` 는 왜 `T` 인가? (`&`/`&&` 안 붙음)
2. `int&& a = 42;` 이후 식 `a`의 값 카테고리는? → **lvalue**
3. `std::move(x)` 의 값 카테고리와 의미는? → **xvalue**, 이동 의미 부여
4. 조건 연산자 반환이 **lvalue**가 되는 경우?
5. `ref-qualifier` 네 가지 조합의 의도와 차이점
6. C++17 임시 실체화로 달라진 prvalue 동작 설명

---

## 10) 안전 코딩 팁

* 이동을 의도할 때만 `std::move` 사용 (이후 해당 객체는 “이전 상태”일 수 있음)
* `const &&` 오버로드는 신중히(대개 필요 없음)
* ref-qualifier로 **반환 최적화와 API 사용성**을 명확히

---