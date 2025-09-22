# C++ `std::shared_ptr` 정리 (시험 대비 요약)

## 핵심 개념

* **공유 소유(s hared ownership)**: 여러 스마트 포인터가 **하나의 객체를 함께 소유**.
* **참조 횟수(reference count)**: 같은 객체를 가리키는 `shared_ptr`의 개수.

  * 새 `shared_ptr`로 복사/대입되면 +1, 소멸/`reset()`되면 −1.
  * **0이 되면** 관리 대상 객체를 **자동으로 delete**.
* **Control Block**: `shared_ptr` 내부의 **메타데이터 덩어리**. 대상 포인터, 참조 카운트(소유/약한), 커스텀 deleter 등 **별도 메모리**에 보관.

  * `shared_ptr`는 **두 개의 포인터**(객체, 컨트롤블록)로 구성되어 일반 포인터 대비 크고, **원자적 증가/감소**로 스레드 안전 참조계수 유지.

## 생성과 증가/감소

* 생성

  * `std::make_shared<T>(args...)` **권장**: 한 번의 할당(컨트롤블록+객체), 짧고 빠르며 예외 안전.
  * `shared_ptr<T> p(new T)` 도 가능하지만 **같은 raw 포인터로 중복 생성 위험**(컨트롤블록 이중 생성 → **이중 해제 위험**).
* 증가

  * **복사 생성/대입**, **값으로 전달** → **참조 카운트 증가**.
* 감소

  * **수명 종료**, `reset()` 또는 다른 객체로 재바인딩 → **참조 카운트 감소**.

## 주요 멤버 함수

* `use_count()` : 현재 **소유자 수**(참조 카운트) 반환.
* `reset()` : 현재 소유 **포기**(인자 없을 때). 새 포인터로 **재설정** 가능.
* `get()` : **raw 포인터** 반환(소유권 없음).
* `operator*`, `operator->` : 대상 객체 접근.

## 전달 방식과 참조 카운트

* 함수 인자에 **값**(`shared_ptr<T>`)으로 받으면 **복사**되어 함수 진입 시 +1, 종료 시 −1.
* **참조**(`const shared_ptr<T>&`)로 받으면 **카운트 변동 없음** (임시객체 생성이 없다는 전제).

## 캐스팅

* `static_pointer_cast<T>(sp)`, `dynamic_pointer_cast<T>(sp)`, `const_pointer_cast<T>(sp)`: **shared\_ptr 간 타입 변환**(컨트롤블록 공유).

## 커스텀 deleter

* `shared_ptr<T> p(new T, deleter)` : 소멸 시 **특정 방식**으로 파괴.
* `shared_ptr`은 **deleter 타입이 포인터 타입에 영향 없음** → **서로 다른 deleter**를 가진 포인터들을 **한 컨테이너에 섞어 담을 수 있음**.

## 배열과 `shared_ptr`

* C++17 전에는 배열에 `shared_ptr<T>` 사용 시 **`delete[]` 지정** 필요:

  * `shared_ptr<int>(new int[10], std::default_delete<int[]>())`
* C++17부터 `shared_ptr<T[]>` 지원: `shared_ptr<int[]> sp(new int[10]);`
* 배열은 가급적 **`std::vector`/`std::array`** 사용 권장.

## `enable_shared_from_this`

* `class X : public enable_shared_from_this<X>` → `shared_from_this()`로 **자기 자신을 가리키는 `shared_ptr` 안전 획득**.
* **중요**: 이미 **어딘가에서 `shared_ptr`로 관리 중**인 객체여야 함.
* `shared_ptr<X>(this)` 같이 직접 생성하면 **별도 컨트롤블록**이 생겨 **이중 해제 위험**.

## 순환 참조(Circular Reference)와 `weak_ptr`

* **문제**: `shared_ptr`가 서로를 가리키면 참조 카운트가 **0이 되지 않아** **메모리 누수**.
* **해결**: 한쪽 링크(역방향, 소유가 아님)를 \*\*`weak_ptr`\*\*로 바꾸어 **cycle을 끊음**.

  * `weak_ptr`은 **참조 카운트 증가 없음**, 필요 시 `lock()`으로 일시적 `shared_ptr` 획득.

## 스레드 관련

* **참조 카운트 변경은 원자적** → 여러 스레드에서 안전하게 증가/감소.
* 단, **대상 객체의 상태 변경은 별도 동기화 필요**.

## `make_shared`가 항상 좋은가?

* 거의 대부분 **그렇다**. 단, 다음은 **부적합/주의**:

  1. **커스텀 메모리 관리**가 필요한 타입.
  2. **매우 큰 객체** + `weak_ptr`이 **오래** 살아남는 경우: 컨트롤블록이 객체와 함께 할당되어 **객체 생존**이 길어질 수 있음.
  3. **커스텀 deleter** 필요하거나 **중괄호 초기치** 등을 써야 하는 특수 케이스.

## `shared_ptr` vs `unique_ptr`

* **배타적 소유**가 맞다면 `unique_ptr`이 더 **가볍고 빠름**(원시포인터에 근접).
* 필요 시 `shared_ptr`로 **올리기 쉬움**(`shared_ptr<T> sp = std::move(up);`).

## 시험 예상 문제(핵심 포인트)

1. `shared_ptr`의 **control block**에 들어있는 정보와 **생성 경로**에 따른 생성 규칙(`make_shared`, raw 포인터, `unique_ptr`에서 변환).
2. raw 포인터로 \*\*두 개의 `shared_ptr`\*\*를 만든 경우에 생길 수 있는 문제와 예방법.
3. **참조 카운트 변화**: 값 전달 vs 참조 전달, `reset()`, 스코프 종료 시점.
4. `enable_shared_from_this`의 목적과 **why not `shared_ptr(this)`**.
5. **순환 참조**가 발생하는 상황과 **`weak_ptr`로의 해결** 구조.
6. `make_shared`의 장/단점, **deleter** 및 **배열** 처리 방법.
7. `use_count()` 차이: 동일 객체를 가리키는 **모든 `shared_ptr`에서 동일 값** 반환.
8. `shared_ptr`의 **원자성**과 스레드 안전성의 범위(참조계수만).

---