// shared_ptr_all_in_one.cpp
// Build: g++ -std=gnu++17 -O2 shared_ptr_all_in_one.cpp && ./a.out
#include <iostream>
#include <memory>
#include <vector>
#include <string>
#include <cstring>

using namespace std;

// ------------------------------------------------------------
// 1) 기본: use_count, 복사/대입, reset, get
// ------------------------------------------------------------
namespace basic_demo {

	struct A {
		int* data;
		A() { data = new int[100]; cout << "A 생성\n"; }
		~A() { cout << "A 소멸\n"; delete[] data; }
		void func() {}
	};

	void run() {
		cout << "\n=== [1] 기본 use_count/reset/get ===\n";

		// int shared_ptr
		auto ptr1 = std::shared_ptr<int>(new int(5));
		cout << "ptr1.use_count = " << ptr1.use_count() << "\n"; // 1

		auto ptr2(ptr1); // 복사 생성
		cout << "ptr1.use_count = " << ptr1.use_count() << "\n"; // 2

		auto ptr3 = ptr1; // 대입
		cout << "ptr1.use_count = " << ptr1.use_count() << "\n"; // 3

		// reset / use_count
		auto p = std::make_shared<A>();
		cout << "p.use_count = " << p.use_count() << "\n"; // 1
		auto han = p; // 공유
		cout << "p.use_count = " << p.use_count() << "\n"; // 2
		han.reset(); // 소유 포기
		cout << "p.use_count = " << p.use_count() << "\n"; // 1

		// get()
		auto spA = std::shared_ptr<A>(new A());
		A* raw = spA.get(); // delete 금지!
		raw->func();
		(*spA).func();
		spA->func();
	}

} // namespace basic_demo

// ------------------------------------------------------------
// 2) vector 내 shared_ptr 소멸 시점 확인
// ------------------------------------------------------------
namespace vector_lifecycle {

	struct A {
		int* data{};
		A() { data = new int[10]; cout << "A()\n"; }
		~A() { cout << "~A()\n"; delete[] data; }
	};

	void run() {
		cout << "\n=== [2] vector 요소 소멸 시점 ===\n";
		std::vector<std::shared_ptr<A>> vec;

		vec.push_back(std::shared_ptr<A>(new A()));
		vec.push_back(std::shared_ptr<A>(vec[0]));
		vec.push_back(std::shared_ptr<A>(vec[1]));

		cout << "첫 번째 erase\n";
		vec.erase(vec.begin()); // 참조 수 감소되지만 아직 소유자 남음

		cout << "두 번째 erase\n";
		vec.erase(vec.begin()); // 또 감소

		cout << "세 번째 erase\n";
		vec.erase(vec.begin()); // 마지막 소유자 제거 → 소멸
		cout << "프로그램 계속\n";
	}

} // namespace vector_lifecycle

// ------------------------------------------------------------
// 3) enable_shared_from_this
// ------------------------------------------------------------
namespace shared_from_this_demo {

	struct Node : std::enable_shared_from_this<Node> {
		void hello() { cout << "Node::hello\n"; }
		std::shared_ptr<Node> get_self() { return shared_from_this(); }
	};

	void run() {
		cout << "\n=== [3] enable_shared_from_this ===\n";
		auto n1 = std::make_shared<Node>();
		auto n2 = n1->get_self(); // 같은 컨트롤 블록 공유
		cout << "use_count: " << n1.use_count() << "\n";
		n2->hello();
	}

} // namespace shared_from_this_demo

// ------------------------------------------------------------
// 4) 함수 인자 전달: 값/참조에 따른 use_count 차이
// ------------------------------------------------------------
namespace param_passing {

	struct Car {};

	void by_value(std::shared_ptr<Car> c) { (void)c; }          // +1 → 함수 끝나면 -1
	void by_cref(const std::shared_ptr<Car>& c) { (void)c; }    // 변화 없음

	void run() {
		cout << "\n=== [4] 값 vs 참조 인자 전달 ===\n";
		auto c1 = std::shared_ptr<Car>(new Car()); // use_count = 1
		auto c2 = c1; // 2
		auto c3 = c1; // 3
		cout << "before: " << c1.use_count() << "\n"; // 3

		by_value(c3); // 4 → 함수 끝나면 다시 3
		cout << "after by_value: " << c1.use_count() << "\n"; // 3

		by_cref(c3);  // 3 유지
		cout << "after by_cref: " << c1.use_count() << "\n"; // 3

		c3.reset();   // 2
		cout << "after reset c3: " << c1.use_count() << "\n"; // 2

		const std::shared_ptr<Car>& c4 = c1;
		cout << "cref alias use_count: " << c4.use_count() << "\n"; // 2
	}

} // namespace param_passing

// ------------------------------------------------------------
// 5) 캐스팅: static_pointer_cast / dynamic_pointer_cast
// ------------------------------------------------------------
namespace casting_demo {

	struct Base { virtual ~Base() = default; };
	struct Derived : Base {};

	void run() {
		cout << "\n=== [5] pointer cast (control block 공유) ===\n";
		std::shared_ptr<Base> pb(new Derived());
		std::shared_ptr<Derived> pd = std::static_pointer_cast<Derived>(pb);
		cout << "pb.use_count = " << pb.use_count()
			<< ", pd.use_count = " << pd.use_count() << "\n"; // 같은 컨트롤 블록
	}

} // namespace casting_demo

// ------------------------------------------------------------
// 6) 배열 관리 (C++17 전/후)
// ------------------------------------------------------------
namespace array_demo {

	struct A {
		int n{};
		A() { cout << "A()\n"; }
		~A() { cout << "~A()\n"; }
		explicit A(int v) : n(v) {}
		void func() { cout << "A.n=" << n << "\n"; }
	};

	void run() {
		cout << "\n=== [6] 배열 shared_ptr (C++17 전/후) ===\n";

		// C++17 전: deleter 지정
		std::shared_ptr<int> spi_old(new int[10], std::default_delete<int[]>());

		// C++17: T[] 지원
		std::shared_ptr<A[]> spi(new A[3]);
		A* raw = spi.get();
		raw[0].func();
	}

} // namespace array_demo

// ------------------------------------------------------------
// 7) 순환 참조 문제 & weak_ptr 해결
// ------------------------------------------------------------
namespace cycle_demo_bad {

	struct A {
		std::shared_ptr<A> other; // 서로가 shared_ptr로 보유 → 순환 참조
		~A() { cout << "A destroyed\n"; }
	};

	void run() {
		cout << "\n=== [7-1] 순환 참조 (bad) ===\n";
		auto pa = std::make_shared<A>();
		auto pb = std::make_shared<A>();
		pa->other = pb;
		pb->other = pa;
		// main 종료 시에도 소멸 안 될 수 있음(참조 카운트 고리)
	}

} // namespace cycle_demo_bad

namespace cycle_demo_good {

	struct A {
		std::weak_ptr<A> other; // weak_ptr로 고리 끊기
		~A() { cout << "A destroyed\n"; }
	};

	void run() {
		cout << "\n=== [7-2] weak_ptr로 해결 (good) ===\n";
		auto pa = std::make_shared<A>();
		auto pb = std::make_shared<A>();
		pa->other = pb; // 참조수 증가 없음
		pb->other = pa;
		// scope 끝나면 참조수 0 → 정리
	}

} // namespace cycle_demo_good

// ------------------------------------------------------------
// 8) 컨테이너 순환 예시 (Group/Person, Camera/Light)
// ------------------------------------------------------------
namespace container_cycle_examples {

	struct Person;
	struct Group {
		std::vector<std::shared_ptr<Person>> members; // OK
		~Group() { cout << "Group destroyed\n"; }
	};
	struct Person {
		// bad: shared_ptr<Group> 를 양방향으로 가지면 순환
		// 해결: 여기선 예시만, 실제 해결은 한쪽을 weak_ptr로
		std::vector<std::shared_ptr<Group>> groups;
		~Person() { cout << "Person destroyed\n"; }
	};

	struct Camera;
	struct Light {
		std::shared_ptr<Camera> camera; // 한쪽을 weak_ptr로 바꾸는 게 정석
		~Light() { cout << "Light destroyed\n"; }
	};
	struct Camera {
		std::vector<std::shared_ptr<Light>> lights;
		~Camera() { cout << "Camera destroyed\n"; }
	};

	void run() {
		cout << "\n=== [8] 컨테이너 양방향 참조 (주의: weak_ptr 필요) ===\n";
		auto p1 = std::make_shared<Person>();
		auto p2 = std::make_shared<Person>();
		auto g1 = std::make_shared<Group>();
		auto g2 = std::make_shared<Group>();
		p1->groups.push_back(g1);
		p2->groups.push_back(g2);
		g1->members.push_back(p1);
		g2->members.push_back(p2);
		// 실제로는 한쪽을 weak_ptr로 해야 누수 방지
	}

} // namespace container_cycle_examples

// ------------------------------------------------------------
// 9) 커스텀 deleter 예시
// ------------------------------------------------------------
namespace custom_deleter_demo {

	struct W { ~W() { cout << "W destroyed\n"; } };

	void run() {
		cout << "\n=== [9] custom deleter (shared_ptr 타입 동일) ===\n";
		auto del1 = [](W* pw) { cout << "del1\n"; delete pw; };
		auto del2 = [](W* pw) { cout << "del2\n"; delete pw; };

		std::shared_ptr<W> w1(new W, del1);
		std::shared_ptr<W> w2(new W, del2);

		std::vector<std::shared_ptr<W>> v{ w1, w2 }; // 타입 동일
		(void)v;
	}

} // namespace custom_deleter_demo

int main() {
	basic_demo::run();
	vector_lifecycle::run();
	shared_from_this_demo::run();
	param_passing::run();
	casting_demo::run();
	array_demo::run();
	cycle_demo_bad::run();
	cycle_demo_good::run();
	container_cycle_examples::run();
	custom_deleter_demo::run();
	cout << "\n=== 프로그램 종료 ===\n";
	return 0;
}