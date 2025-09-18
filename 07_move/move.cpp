// copy_move_semantics.cpp
// g++ -std=c++17 copy_move_semantics.cpp && ./a.out

#include <iostream>
#include <cstring>
#include <utility>
#include <vector>
#include <string>

using namespace std;

// ------------------------------------------------------------
// 1) 기본 Copy 데모: 생성/복사/대입과 호출 지점
// ------------------------------------------------------------
namespace demo_copy_basics {

	struct Foo {
		Foo() { cout << "[Foo] 생성자\n"; }
		Foo(const Foo&) { cout << "[Foo] 복사 생성자\n"; }
		Foo& operator=(const Foo&) {
			cout << "[Foo] 복사 대입연산자\n"; return *this;
		}
	};

	void func_by_value(Foo a) { (void)a; }   // 복사 생성자
	void func_by_ref(Foo& a) { (void)a; }   // 없음
	void func_by_ptr(Foo* a) { (void)a; }   // 없음

	void run() {
		cout << "\n=== [1] Copy Basics ===\n";
		Foo a1;           // 생성
		Foo a2;           // 생성
		Foo a3 = a1;      // 복사 생성자
		a2 = a1;          // 복사 대입

		func_by_value(a1);   // 복사 생성자
		func_by_ref(a1);     // 없음
		func_by_ptr(&a1);    // 없음
	}

} // namespace demo_copy_basics

// ------------------------------------------------------------
// 2) 깊은 복사 데모: Person (char* 소유)
//    Rule of Five 구현(복사/이동/소멸 포함)
// ------------------------------------------------------------
namespace demo_deep_copy {

	struct Person {
		char* name{};
		int   age{};

		Person() = default;

		Person(const char* n, int a) : age(a) {
			size_t len = std::strlen(n) + 1;
			name = new char[len];
			std::memcpy(name, n, len);
		}

		// 복사 생성자 (깊은 복사)
		Person(const Person& p) : age(p.age) {
			cout << "[Person] Copy Ctor\n";
			if (p.name) {
				size_t len = std::strlen(p.name) + 1;
				name = new char[len];
				std::memcpy(name, p.name, len);
			}
		}

		// 복사 대입 (깊은 복사)
		Person& operator=(const Person& p) {
			cout << "[Person] Copy Assign\n";
			if (this != &p) {
				delete[] name;
				name = nullptr;
				age = p.age;
				if (p.name) {
					size_t len = std::strlen(p.name) + 1;
					name = new char[len];
					std::memcpy(name, p.name, len);
				}
			}
			return *this;
		}

		// 이동 생성자
		Person(Person&& other) noexcept : name(other.name), age(other.age) {
			cout << "[Person] Move Ctor\n";
			other.name = nullptr; other.age = 0;
		}

		// 이동 대입
		Person& operator=(Person&& other) noexcept {
			cout << "[Person] Move Assign\n";
			if (this != &other) {
				delete[] name;
				name = other.name; age = other.age;
				other.name = nullptr; other.age = 0;
			}
			return *this;
		}

		~Person() { delete[] name; }

		void print() const {
			cout << "Person{name=" << (name ? name : "(null)") << ", age=" << age << "}\n";
		}
	};

	Person make() {
		Person tmp("이순신", 54);
		return tmp; // NRVO/PRVO
	}

	void run() {
		cout << "\n=== [2] Deep Copy (Rule of Five) ===\n";
		Person p1("홍길동", 30);
		Person p2 = p1;  // Copy
		p2.age = 40;
		p1.print(); p2.print();

		Person p3;
		p3 = p1;         // Copy Assign
		p3.age = 50;
		p1.print(); p3.print();

		Person p4 = make(); // C++17: 복사/이동 없이 직접 초기화 가능 (상황에 따라 이동이 보일 수 있음)
		p4.print();

		Person p5 = std::move(p1); // Move Ctor
		p1.print(); p5.print();
	}

} // namespace demo_deep_copy

// ------------------------------------------------------------
// 3) std::move 데모: 실제 이동은 rvalue 오버로드가 수행
// ------------------------------------------------------------
namespace demo_std_move {

	struct Foo {
		Foo() { cout << "[MoveDemo::Foo] 생성자\n"; }
		Foo(const Foo&) { cout << "[MoveDemo::Foo] 복사 생성자\n"; }
		Foo(Foo&&) { cout << "[MoveDemo::Foo] 이동 생성자\n"; }
	};

	void run() {
		cout << "\n=== [3] std::move 데모 ===\n";
		Foo a;
		cout << "----- 복사 -----\n";
		Foo b(a);              // copy
		cout << "----- 이동(std::move) -----\n";
		Foo c(std::move(a));   // move
		(void)b; (void)c;
	}

} // namespace demo_std_move

// ------------------------------------------------------------
// 4) lvalue/rvalue 모두 수용하는 B 생성자 오버로드
// ------------------------------------------------------------
namespace demo_forwarding {

	struct A {
		A() { cout << "A()\n"; }
		A(const A&) { cout << "A() copy\n"; }
		A(A&&) { cout << "A() move\n"; }
	};

	struct B {
		A m_a;
		// lvalue는 복사
		B(const A& a) : m_a(a) {}
		// rvalue는 이동
		B(A&& a) : m_a(std::move(a)) {}
	};

	void run() {
		cout << "\n=== [4] Overload for lvalue/rvalue ===\n";
		A a;
		cout << "create B with lvalue:\n";
		B b1(a);               // copy
		cout << "create B with rvalue:\n";
		B b2(std::move(a));    // move
	}

} // namespace demo_forwarding

// ------------------------------------------------------------
// 5) const와 이동
// ------------------------------------------------------------
namespace demo_const_move {
	void run() {
		cout << "\n=== [5] const와 이동 ===\n";
		const int a = 10;
		int&& r = std::move(a); // 실제로 'a'를 바꿀 수 없으므로 이동 의미 없음 (상숫값 복사와 유사)
		cout << "r=" << r << "\n";
	}
} // namespace demo_const_move

// ------------------------------------------------------------
// 6) MyString: 복사/이동 + vector 동작, noexcept 중요
// ------------------------------------------------------------
namespace demo_mystring {

	class MyString {
		char* content{};
		int   len{};
		int   cap{};
	public:
		MyString() {
			// cout << "MyString() 기본 생성\n";
		}

		explicit MyString(const char* s) {
			// cout << "MyString(const char*) 생성\n";
			len = std::strlen(s);
			cap = len;
			content = new char[cap];
			std::memcpy(content, s, len);
		}

		// 복사 생성자
		MyString(const MyString& rhs) : len(rhs.len), cap(rhs.cap) {
			cout << "[MyString] Copy Ctor\n";
			if (cap) {
				content = new char[cap];
				std::memcpy(content, rhs.content, len);
			}
		}

		// 이동 생성자 (noexcept!)
		MyString(MyString&& rhs) noexcept : content(rhs.content), len(rhs.len), cap(rhs.cap) {
			cout << "[MyString] Move Ctor\n";
			rhs.content = nullptr; rhs.len = rhs.cap = 0;
		}

		// 복사 대입
		MyString& operator=(const MyString& rhs) {
			cout << "[MyString] Copy Assign\n";
			if (this != &rhs) {
				delete[] content; content = nullptr;
				len = rhs.len; cap = rhs.cap;
				if (cap) {
					content = new char[cap];
					std::memcpy(content, rhs.content, len);
				}
			}
			return *this;
		}

		// 이동 대입 (noexcept!)
		MyString& operator=(MyString&& rhs) noexcept {
			cout << "[MyString] Move Assign\n";
			if (this != &rhs) {
				delete[] content;
				content = rhs.content; len = rhs.len; cap = rhs.cap;
				rhs.content = nullptr; rhs.len = rhs.cap = 0;
			}
			return *this;
		}

		~MyString() { delete[] content; }

		// 단순 연결 (데모용)
		MyString operator+(const MyString& s) const {
			MyString r;
			r.reserve(len + s.len);
			// r.content는 cap 길이만큼 할당됨
			std::memcpy(r.content, content, len);
			std::memcpy(r.content + len, s.content, s.len);
			r.len = len + s.len;
			return r; // NRVO/Move
		}

		void reserve(int new_cap) {
			if (new_cap > cap) {
				char* prev = content;
				content = new char[new_cap];
				if (prev) std::memcpy(content, prev, len);
				cap = new_cap;
				delete[] prev;
			}
		}

		int length() const { return len; }

		void println() const {
			for (int i = 0; i < len; ++i) std::cout << content[i];
			std::cout << '\n';
		}
	};

	void run() {
		cout << "\n=== [6] MyString + vector (noexcept 이동) ===\n";
		MyString s1("abc"), s2("def");
		MyString s3 = s1 + s2;  // 연결: 복사/이동 관찰
		s3.println();

		std::vector<MyString> v;
		v.reserve(3); // 재할당 중 이동/복사 관찰
		cout << "push 1 ---\n"; v.push_back(s1);            // 복사
		cout << "push 2 ---\n"; v.push_back(s2);            // 복사
		cout << "push 3 ---\n"; v.push_back(std::move(s3)); // 이동
		// 이동 생성자가 noexcept 아니면 vector는 복사로 폴백할 수 있음
	}

} // namespace demo_mystring

// ------------------------------------------------------------
// main: 모든 데모 실행
// ------------------------------------------------------------
int main() {
	demo_copy_basics::run();
	demo_deep_copy::run();
	demo_std_move::run();
	demo_forwarding::run();
	demo_const_move::run();
	demo_mystring::run();
	return 0;
}