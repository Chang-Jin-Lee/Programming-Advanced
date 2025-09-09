# C++ RTTI & 타입 Traits 정리

## 1. RTTI (Run Time Type Information)
- 실행 시간에 객체의 실제 타입을 알아내는 기능
- C++에서 RTTI 제공 키워드:
  - `typeid`
  - `dynamic_cast`

---

### (1) `typeid` 를 이용한 RTTI
```cpp
if (typeid(*pB) == typeid(Derived2)) {
    std::cout << typeid(*pB).name() << std::endl;
} else {
    std::cout << "No!" << std::endl;
}
````

* `typeid(*pB)` → 객체의 실제 타입 확인 가능
* 부모 클래스에 **virtual 함수**가 있어야 동작

---

### (2) `dynamic_cast` 를 이용한 RTTI

```cpp
if (dynamic_cast<Derived2*>(pB)) {
    std::cout << typeid(*pB).name() << std::endl;
} else {
    std::cout << "No!" << std::endl;
}
```

* 성공 시 → 변환된 포인터 반환
* 실패 시 → `nullptr` 반환

---

## 2. 타입 Traits

### (1) `is_base_of_v` (상속 관계 확인)

* `std::is_base_of_v<Base, Derived>`
* `Base` 가 `Derived` 의 부모인지 확인
* public, private 상속 모두 `true`

```cpp
class A {};
class B : public A {};
class C : private A {};

static_assert(std::is_base_of_v<B, B> == true);      // 동일 클래스: true
static_assert(std::is_base_of_v<int, int> == false); // 기본 자료형: false
static_assert(std::is_base_of_v<A, B> == true);      // public 상속: true
static_assert(std::is_base_of_v<A, C> == true);      // private 상속: true
```

---

### (2) `is_same_v` (동일 타입인지 확인)

* `std::is_same_v<T1, T2>`
* 두 타입이 같은지 확인

```cpp
class P1 { };
class P2 : public P1 { };

cout << is_same_v<P1, P1> << endl; // true
cout << is_same_v<P1, P2> << endl; // false
```

`decltype` 과 함께 사용 가능:

```cpp
P1 p1; P2 p2;
cout << is_same_v<decltype(p1), decltype(p1)> << endl; // true
cout << is_same_v<decltype(p1), decltype(p2)> << endl; // false
```

---

## 3. 시험 포인트

1. `typeid` 와 `dynamic_cast` 차이점 설명

   * `typeid` : 타입 정보 직접 비교
   * `dynamic_cast` : 안전한 다운캐스팅, 실패 시 `nullptr`
2. `is_base_of_v<A, B>` 결과는? (상속 관계 여부 확인)
3. `is_same_v<T1, T2>` 의 용도? (두 타입 동일 여부 확인)
4. RTTI 사용하려면 부모 클래스에 필요한 조건? → `virtual` 함수

---