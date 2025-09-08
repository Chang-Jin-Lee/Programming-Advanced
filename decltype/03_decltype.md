````markdown
# C++ 템플릿 함수 리턴 타입 정리  
`auto` / `decltype` / `decltype(auto)` / 후행 리턴 타입(Trailing return type) / 전달 참조(Forwarding reference)

## 1) 문제 상황
템플릿 함수에서 결과 타입이 **템플릿 인자에 의존**할 때, 반환/출력 파라미터의 **정확한 타입을 컴파일 타임**에 결정해야 한다.

예)
```cpp
template <typename T, typename U>
void add(T t, U u, ??? result); // result* 에 들어갈 정확한 타입은?
````

## 2) 해결 요약

* **출력 파라미터(pointer)**: `decltype(t + u)*`
* **반환 타입 (C++11)**: 후행 리턴 타입 `auto f(...) -> decltype(표현식)`
* **반환 타입 (C++14+)**: `auto` 로 추론 가능. 단, **참조/const 보존**이 필요하면 `decltype(auto)` 사용
* **표현식 앞에서 `decltype(t+u)`를 반환 타입으로 직접 쓰지 말 것**
  (선언 시점엔 `t`, `u`가 아직 식별 불가 → **후행 리턴 타입**으로 옮겨야 함)

## 3) 핵심 규칙

* `auto` 는 대체로 **const/참조를 탈락**시켜 **값 타입**으로 추론
* `decltype(expr)` 는 **expr의 타입 원형을 보존**
* `decltype(auto)` 는 **반환 시** `expr`의 **참조/const 여부를 그대로 보존**
* `std::forward<T>(x)` 와 **전달 참조(T&&)** 를 함께 써서 **값/참조/값범주 보존**

## 4) 자주 쓰는 패턴

### 4.1 출력 파라미터(out parameter)

```cpp
template <typename T, typename U>
void add(T t, U u, decltype(t + u)* result) { // t+u 의 정확한 타입 포인터
    *result = t + u;
}
```

### 4.2 반환 타입 결정 (C++11: 후행 리턴 타입)

```cpp
template <typename T, typename U>
auto add(T t, U u) -> decltype(t + u) {
    return t + u;
}
```

### 4.3 반환 타입 결정 (C++14: auto / decltype(auto))

```cpp
// 값 타입으로 충분할 때
template <typename T, typename U>
auto add_val(T t, U u) {
    return t + u; // auto: 값으로 추론
}

// 참조/const 보존이 필요할 때
template <typename T, typename U>
decltype(auto) add_ref_preserving(T&& t, U&& u) {
    return (t + u); // expr 가 참조를 반환하면 참조 유지
}
```

### 4.4 보편/전달 참조(Forwarding reference) + forward

```cpp
template<typename T, typename U>
auto Plus(T&& lhs, U&& rhs) {
    return std::forward<T>(lhs) + std::forward<U>(rhs);
}
```

## 5) auto vs decltype(auto) (반환)

* **문제 예시**: `auto` 반환은 복사본을 만들어 **레퍼런스 의미가 사라짐**
* **해결**: `decltype(auto)` 로 반환하면 **원형 유지**

```cpp
class C {
    int arr[10] = {1,2,3,4,5,6,7,8,9,10};
public:
    int& operator[](int i) { return arr[i]; }
};

template<typename T>
auto changearr(T& c, int index) {     // auto → 값 반환
    return c[index];                  // int&를 int로 복사
}

template<typename T>
decltype(auto) GetArr(T&& c, int index) {   // decltype(auto) → 원형 유지
    return std::forward<T>(c)[index];       // int& 유지
}
```

## 6) 시험 포인트

1. `auto` 와 `decltype(auto)` 반환의 차이 (참조/const 보존 여부)
2. 왜 `decltype(t+u)` 를 **함수 선언부 반환 타입**에 직접 쓰면 안 되는가?
   → `t`,`u` 식별 불가. **후행 리턴 타입**으로 이동해야 함
3. 출력 파라미터에 올바른 타입 포인터를 붙이는 법 → `decltype(표현식)*`
4. 전달 참조/`std::forward` 의 필요성과 효과
5. `Plus(A, B)` 예에서 문자열과 정수 모두 동작하는 이유 (연산자 오버로드/템플릿 추론)
6. nullptr의 자료형은 nullptr_t임_

---