# `std::shared_ptr` 완전 정리 (시험 대비 요약)

## 1) 개념과 목적

* **공유 소유(shared ownership)** 스마트 포인터. 여러 개가 **같은 객체**를 가리킬 수 있음.
* 내부적으로 **참조 횟수(reference count)** 를 관리하여, **마지막 소유자가 사라질 때** `delete` 실행.
* **언제 쓰나**

  * 여러 주체가 **동등하게 소유**해야 하는 자원
  * 소유권이 명확히 한 곳(배타적)이라면 `unique_ptr`이 더 낫다(가볍고 빠름).

## 2) 기본 사용법

* 생성: `std::shared_ptr<T> p{new T(...)};` 또는 **`std::make_shared<T>(...)` 권장**
* 참조 수: `p.use_count()`
* 소유 해제: `p.reset()` (인자 없으면 **해당 소유 포기** → 참조 수 감소)
* 원시 포인터 얻기(주의): `p.get()` (소유권 없음, **delete 금지**)

## 3) `make_shared`를 권장하는 이유

* **예외 안전성/성능**(컨트롤 블록 + 객체 **동시 할당**으로 더 작고 빠른 코드)
* 단, 다음 상황에선 부적합할 수 있음:

  * **커스텀 메모리 관리**가 필요한 타입
  * **매우 큰 객체** + `weak_ptr`가 오래 남는 환경(컨트롤 블록과 객체가 합쳐져 있어 메모리 압박)
  * **커스텀 deleter** 지정, **중괄호 초기치** 필요 시 → 직접 생성자 사용

## 4) 컨트롤 블록(Control Block) 이해

* **레퍼런스 카운트, deleter, 타입 정보** 등을 담는 메타 구조체.
* 다음 경우 **컨트롤 블록 생성**:

  * `std::make_shared`
  * `std::shared_ptr` 를 `unique_ptr`에서 변환
  * **raw pointer** 로 `shared_ptr` 생성
* **중요**: **같은 raw 포인터**로 `shared_ptr`을 **두 번** 만들면 **서로 다른 컨트롤 블록** → **중복 해제 위험**
  → **항상** `new`를 **직접 변수에 담지 말고** 생성자/팩토리에 **즉시** 넘겨라:

  ```cpp
  // OK
  auto sp = std::shared_ptr<T>(new T, customDel);
  auto sp2 = sp; // 같은 컨트롤 블록

  // NG (raw ptr를 두 곳에서 각각 shared_ptr로 감싸면 컨트롤 블록 2개)
  T* raw = new T;
  std::shared_ptr<T> a(raw);
  std::shared_ptr<T> b(raw); // 위험!
  ```

## 5) `enable_shared_from_this` 와 `shared_from_this()`

* `this`로부터 **안전하게 `shared_ptr`** 를 얻기 위한 베이스 클래스.
* `class X : public std::enable_shared_from_this<X> { ... };`
* **반드시 해당 객체가 이미 shared\_ptr로 관리되고 있을 때만** `shared_from_this()` 사용 가능.
  (그렇지 않으면 컨트롤 블록 없이 예외/UB)

## 6) 함수 인자 전달과 참조 카운트

* **값으로(shared\_ptr by value)** 받으면 **복사**가 일어나 **참조 수 +1**. 함수 끝나면 -1.
* **`const shared_ptr<T>&`** 로 받으면 **참조 수 증가 없음**(임시 객체 생성/소멸 없음)
  → 읽기만 한다면 **상수 참조**가 효율적.

## 7) 캐스팅

* `static_pointer_cast<T>(sp)`, `dynamic_pointer_cast<T>(sp)` 등 **비멤버 함수**로 **안전하게** 형변환.
  → **컨트롤 블록 공유** 상태 유지(참조 수 일관성 유지).

## 8) 배열과 `shared_ptr`

* **C++17 전**: 배열을 `shared_ptr`로 관리하려면 **삭제자 지정** 필요

  ```cpp
  std::shared_ptr<int> sp(new int[10], std::default_delete<int[]>());
  ```
* **C++17 이후**: `std::shared_ptr<T[]>` 지원

  ```cpp
  std::shared_ptr<A[]> sp(new A[3]);
  ```
* 배열은 가능하면 **`std::vector`, `std::array`** 같은 컨테이너 사용 권장.

## 9) `shared_ptr`의 비용/특징

* **포인터 2개 크기**(자원 포인터 + 컨트롤 블록 포인터)
* 컨트롤 블록은 **동적 할당**, 참조 수 증감은 **원자적(atomic)** (다중 스레드 안전 카운팅)

## 10) 순환 참조(Circular Reference) 문제와 해결법

* **문제**: `A`가 `shared_ptr<B>`를, `B`가 `shared_ptr<A>`를 멤버로 가지면 참조 수가 0이 되지 않음 → **메모리 누수**
* **해결**: **한쪽을 `std::weak_ptr`** 로 바꿔 **소유권 고리 끊기**

  * `weak_ptr`는 **참조 수에 기여하지 않음**
  * 필요 시 `lock()` 으로 **일시적 `shared_ptr`** 획득 (만료 시 `nullptr`)

## 11) 벡터/컨테이너에서의 수명

* `std::vector<shared_ptr<T>>` 에 담으면, **컨테이너 요소 소멸/삭제 시 참조 수 감소**
  → **마지막 소유자**가 사라지는 지점에서 소멸자 호출 순서를 추적해보면 동작 이해에 좋음.

## 12) 커스텀 deleter

* `unique_ptr`과 달리, **deleter 타입이 `shared_ptr` 타입에 반영되지 않음**
  → deleter가 서로 달라도 **같은 컨테이너에 담기 쉬움**

  ```cpp
  auto del1 = [](T* p){ /*...*/ delete p; };
  auto del2 = [](T* p){ /*...*/ delete p; };
  std::shared_ptr<T> a(new T, del1);
  std::shared_ptr<T> b(new T, del2);
  std::vector<std::shared_ptr<T>> v{a, b}; // OK
  ```

## 13) 가비지 컬렉션과의 차이

* `shared_ptr`은 **언제 해제되는지 시점이 결정적**(참조 수가 0 되는 순간).
* GC는 **주기적 스캔/추적**, 해제 시점이 **비결정적**.

---

## 예상 시험문항

1. `make_shared` 와 `shared_ptr(new T)` 의 차이와 장단점을 쓰시오.
2. 같은 raw 포인터로 `shared_ptr`을 두 번 생성하면 왜 문제가 되는지 설명하시오.
3. `enable_shared_from_this` 를 사용하는 이유와 주의사항을 쓰시오.
4. 함수 인자를 `shared_ptr<T>` 와 `const shared_ptr<T>&` 로 받을 때 참조 카운트 변화 설명.
5. 순환 참조가 왜 발생하며, `weak_ptr` 로 해결하는 원리를 쓰시오.
6. `shared_ptr` 배열 관리 방법(C++17 전/후)을 쓰시오.
7. 커스텀 deleter 가 `unique_ptr` 과 `shared_ptr` 에서 타입에 미치는 영향의 차이를 쓰시오.
8. 컨트롤 블록이 무엇인지, 언제 생성되는지 쓰시오.

---