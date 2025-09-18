# C++ 메모리 관리 & 동적 할당 (`new` / `delete`) + 스마트 포인터/RAII

## 1) 동적 할당 기본
- `new T` : 힙(heap)에 `sizeof(T)` 바이트 할당 후 **생성자 호출** (기본형도 0이 아님)
- `delete p` : `new`로 확보한 **단일 객체** 해제
- `new T[N]` : 힙에 배열 N개 할당 (배열 생성자 호출)
- `delete[] p` : `new[]`로 확보한 **배열** 해제

> **반드시 짝 맞춰 해제**: `new → delete`, `new[] → delete[]`

### 초기화 형태
```cpp
int*    pi = new int;         // 초기화 안 됨(불특정 값)
int*    pj = new int(10);     // 값 초기화
double* pd = new double{3.14}; // 단일 요소 리스트 초기화 (C++11)
int*    a1 = new int[3]{};    // 0으로 초기화된 배열
int*    a2 = new int[5]{1,2,3,4,5}; // 배열 초기화
char*   s  = new char[20]{ "Hello" };
````

---

## 2) 클래스 객체 동적 할당

```cpp
struct Test {
  Test() { /*...*/ }
  explicit Test(int x) { /*...*/ }
  void Print() const { /*...*/ }
};

auto p1 = new Test;      // 기본 생성
auto p2 = new Test(10);  // 값 전달 생성
delete p1;
delete p2;

// 배열
auto plist = new Test[3]{ Test(1), Test(2), Test(3) };
for (int i=0;i<3;++i) plist[i].Print();
delete[] plist;

// 2차원처럼 포인터 배열
Test** rows = new Test*[3]{ new Test(4), new Test(5), new Test(6) };
for (int i=0;i<3;++i) rows[i]->Print();
for (int i=0;i<3;++i) delete rows[i];
delete[] rows;
```

---

## 3) 흔한 오류 (메모리 오류/누수)

* **해제 누락**: `new` 하고 `delete` 하지 않음 → 누수
* **재할당 누수**: 같은 포인터에 `delete` 없이 `new` 재할당
* **중복 해제(double free)**: 같은 자원을 두 번 `delete`
* **잘못된 해제**: `new[]` 한 것을 `delete` 로, 혹은 반대로 해제
* **댕글링 포인터**: 이미 해제된 메모리를 다시 사용

> 해결 원칙: RAII, 스마트 포인터 사용

---

## 4) RAII (Resource Acquisition Is Initialization)

* “자원의 획득은 초기화”: 객체의 **생성자에서 자원 획득**, **소멸자에서 해제**
* 예외가 나도 스택 풀리면서 자동 해제

```cpp
struct MyBuffer {
  char* p{};
  explicit MyBuffer(size_t n) : p(new char[n]{}) {}
  ~MyBuffer() { delete[] p; }
};
```

---

## 5) 스마트 포인터 (C++11\~)

* `std::unique_ptr<T>`: **배타적 소유권**, 복사 금지/이동 허용
* `std::shared_ptr<T>`: **공유 소유권**, 참조 횟수 관리
* `std::weak_ptr<T>`: 순환 참조 방지 (관찰 전용)

```cpp
#include <memory>

// C++14
auto up = std::make_unique<Foo>();  // 권장 (예외 안전, 단일 할당)
// C++11
std::unique_ptr<Foo> up2(new Foo());

{
  auto tmp = std::make_unique<Foo>();
} // 블록 종료 시 자동 해제
```

> **권장**: `new` 직접 사용 대신 `std::make_unique` / `std::make_shared` 사용

---

## 6) “포인터의 문제점”과 해결

* 단일/배열 구분 어려움 → `delete`/`delete[]` 혼동 위험
* 소유권 불명확 → 중복/누락 해제
* 예외/조기 반환 시 해제 누락 위험
  → **스마트 포인터 + RAII** 로 해결

---

## 7) 시험 포인트

1. `new`/`delete` 와 `new[]`/`delete[]` 매칭 규칙
2. `new` vs `malloc`: 생성자/소멸자 호출 여부, 타입 안전성, 예외
3. RAII 개념과 장점 (예외 안전, 자동 해제)
4. `unique_ptr`/`shared_ptr`/`weak_ptr` 차이
5. 누수/중복 해제/댕글링 포인터 예와 방지법

---

## 8) 보너스: 간단 스마트 포인터 구현(학습용)

* 복사 금지, 이동 허용
* `operator*`, `operator->` 제공

```cpp
template<typename T>
class SPtr {
  T* p{};
public:
  explicit SPtr(T* ptr) : p(ptr) {}
  ~SPtr() { delete p; }

  SPtr(const SPtr&) = delete;
  SPtr& operator=(const SPtr&) = delete;

  SPtr(SPtr&& other) noexcept : p(other.p) { other.p = nullptr; }
  SPtr& operator=(SPtr&& other) noexcept {
    if (this != &other) { delete p; p = other.p; other.p = nullptr; }
    return *this;
  }

  T& operator*()  const { return *p; }
  T* operator->() const { return  p; }
  explicit operator bool() const { return p != nullptr; }
};
```

---