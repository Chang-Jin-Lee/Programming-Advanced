# 01_auto

---

# C++11 auto 키워드 정리

## 기본 개념

* 컴파일 타임에 타입 추론
* 반드시 초기화 필요
* 동적 타입 아님 (JS, Python 과 다름)

## 추론 규칙

* `auto d = 5.0;` → double
* `auto i = 1 + 2;` → int
* `auto n = 0.123;` → double (0.123f → float)
* `const int& a = 0; auto aa = a;` → int (const, & 무시)
* `auto b = {1,2,3};` → initializer\_list<int>

## 포인터 / 참조

* 포인터: `auto* p = new T();` 권장
* 참조: `auto& r = ref;`
* const 참조: `const auto& r = ref;` 권장

## 반복자

```cpp
for (auto it = v.begin(); it != v.end(); ++it) { ... }
```

반복자 타입이 길 때 유용

## 함수 관련

* 함수 반환값 저장에 유용
* 함수 포인터도 추론 가능

  ```cpp
  auto funcPtr = TestFunc; // int(*)(int,int)
  ```

## 사용 권장

* 반복자
* 템플릿 매개변수
* 함수 반환형
* 함수 포인터

## 주의

* 가독성이 떨어질 수 있음
* 명확한 타입일 때는 auto보다 직접 타입 쓰는 것 권장

---

## 연습 문제

1. `auto n = 0.123;` 의 타입은? `auto n = 0.123f;` 로 바꾸면?
2. `const int& a = 0; auto aa = a;` 의 타입은?
3. `auto b = {1,2,3};` 의 타입은?
4. `auto c = "abc";` 의 타입은?
5. 아래 코드에서 각각의 타입은?

   ```cpp
   Widget w;
   auto o = w;      // ?
   auto* p1 = &w;   // ?
   auto& r1 = w;    // ?
   ```
6. `const int b = 1; auto& a1 = b;` 의 타입은?
7. 함수 포인터 `auto pf = TestFunc;` 의 타입은?
8. 람다 `auto func = [](auto x, auto y){return x+y;};` 에서 `func(2,3.5)`의 반환 타입은?

---