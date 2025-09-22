// 기본: 참조 카운트, reset, get, use_count, 벡터에서의 소멸 시점 관찰
#include <iostream>
#include <memory>
#include <vector>
using namespace std;

class A {
	int* data;
public:
	A() { data = new int[100]; cout << "생성\n"; }
	~A() { cout << "소멸\n"; delete[] data; }
	void func() {}
};

int main() {
	shared_ptr<int> ptr1(new int(5));
	cout << ptr1.use_count() << '\n';   // 1

	auto ptr2(ptr1);
	cout << ptr1.use_count() << '\n';   // 2

	auto ptr3 = ptr1;
	cout << ptr1.use_count() << '\n';   // 3

	// use_count(), reset()
	shared_ptr<A> p = make_shared<A>();
	cout << "소유자 수: " << p.use_count() << '\n'; // 1
	auto han = p;
	cout << "소유자 수: " << p.use_count() << '\n'; // 2
	han.reset();
	cout << "소유자 수: " << p.use_count() << '\n'; // 1

	// get()
	shared_ptr<A> spA(new A());
	A* pA = spA.get();
	spA.get()->func();
	(*spA).func();
	spA->func();

	// use_count()
	shared_ptr<A> p1(new A());
	shared_ptr<A> p2(p1);
	cout << p1.use_count() << '\n';     // 2
	cout << p2.use_count() << '\n';     // 2

	// vector에서의 소멸 순서
	vector<shared_ptr<A>> vec;
	vec.push_back(shared_ptr<A>(new A()));
	vec.push_back(shared_ptr<A>(vec[0]));
	vec.push_back(shared_ptr<A>(vec[1]));

	cout << "첫 번째 소멸!\n";
	vec.erase(vec.begin());     // 아직 소유자 남아있으므로 소멸 X

	cout << "다음 원소 소멸!\n";
	vec.erase(vec.begin());     // 여전히 소유자 남아 있으면 X

	cout << "마지막 원소 소멸!\n";
	vec.erase(vec.begin());     // 소유자 0 → 여기서 소멸

	cout << "프로그램 종료!\n";
}

// make_shared 권장 / 예외 / 커스텀 deleter / 배열 삭제자
#include <iostream>
#include <memory>
using namespace std;

struct Widget { int x = 0; };

int main() {
	// 권장
	auto p1 = std::make_shared<Widget>();

	// 커스텀 deleter 필요 시
	auto loggingDel = [](Widget* pw) { /* 로그 */ delete pw; };
	std::shared_ptr<Widget> spw(new Widget, loggingDel);

	// 배열 삭제자(C++17 전)
	std::shared_ptr<int> spArr1(new int[10], std::default_delete<int[]>());
	std::shared_ptr<int> spArr2(new int[10], [](int* p) { delete[] p; });

	// C++17 이후
	std::shared_ptr<int[]> spArr3(new int[10]);
}

// enable_shared_from_this: 안전한 자기 shared_ptr 획득
#include <iostream>
#include <memory>
using namespace std;

class A : public std::enable_shared_from_this<A> {
	int* data;
public:
	A() { data = new int[100]; cout << "자원 획득!\n"; }
	~A() { cout << "소멸자!\n"; delete[] data; }

	std::shared_ptr<A> get_shared_ptr() {
		return shared_from_this();      // 같은 control block 공유
	}
};

int main() {
	auto pa1 = std::make_shared<A>();  // 반드시 먼저 shared_ptr로 관리되어야 함
	auto pa2 = pa1->get_shared_ptr();

	cout << pa1.use_count() << '\n';   // 2
	cout << pa2.use_count() << '\n';   // 2
}

// 값 전달 vs 참조 전달에 따른 use_count 변화
#include <iostream>
#include <memory>
using namespace std;

class Car {};

void by_value(std::shared_ptr<Car> c) {
	// c가 스코프 들어오며 +1, 빠져나가며 −1
}
void by_const_ref(const std::shared_ptr<Car>& c) {
	// 카운트 변동 없음
}

int main() {
	auto c1 = std::make_shared<Car>();   // use_count = 1
	auto c2 = c1;                        // 2
	auto c3 = c1;                        // 3

	by_value(c3);                        // 진입 4, 종료 3
	cout << c3.use_count() << '\n';      // 3

	c3.reset();                          // 2
	cout << c1.use_count() << '\n';      // 2
	cout << c2.use_count() << '\n';      // 2
	cout << c3.use_count() << '\n';      // 0 (null)

	const std::shared_ptr<Car>& cref = c1;
	cout << cref.use_count() << '\n';    // 2 (증가 없음)
}

// static_pointer_cast: 형변환 시 컨트롤 블록 공유
#include <iostream>
#include <memory>
using namespace std;

struct Car { virtual ~Car() = default; };
struct Truck : Car {};

int main() {
	shared_ptr<Car> pCar(new Truck());
	shared_ptr<Truck> pTruck = static_pointer_cast<Truck>(pCar);

	cout << pCar.use_count() << '\n';   // 2
	cout << pTruck.use_count() << '\n'; // 2
}


// 배열 형태를 shared_ptr로 사용 (C++17 이전/이후)
#include <iostream>
#include <memory>
using namespace std;

struct A {
	int n = 0;
	A() { cout << "A()\n"; }
	~A() { cout << "~A()\n"; }
	A(int v) :n(v) {}
	void func() { cout << n << '\n'; }
};

int main() {
	// C++17 이전
	std::shared_ptr<int> sp1(new int[10], std::default_delete<int[]>());
	std::shared_ptr<int> sp2(new int[10], [](int* p) { delete[] p; });

	// C++17 이후
	std::shared_ptr<A[]> spi(new A[3]);
	A* p = spi.get();  // 배열의 첫 원소 주소
	p[0].func();
}

// 순환 참조 문제(메모리 누수) 예시 1: 서로가 서로를 shared_ptr로 소유
#include <iostream>
#include <memory>
using namespace std;

class A {
	int* data;
	std::shared_ptr<A> other;  // 순환(shared_ptr)
public:
	A() { data = new int[100]; cout << "자원 획득!\n"; }
	~A() { cout << "소멸자!\n"; delete[] data; }
	void set_other(std::shared_ptr<A> o) { other = o; }
};

int main() {
	auto pa = std::make_shared<A>();
	auto pb = std::make_shared<A>();
	pa->set_other(pb);
	pb->set_other(pa);
	// 프로그램 종료 시에도 소멸자 호출 안 됨(누수)
}

// 순환 참조 문제 예시 2: A - B, Person - Group, Camera - Light
#include <iostream>
#include <memory>
#include <vector>
using namespace std;

// A <-> B
class A;
class B {
public:
	std::shared_ptr<A> a;
	~B() { cout << "B destroyed\n"; }
};
class A {
public:
	std::shared_ptr<B> b;
	~A() { cout << "A destroyed\n"; }
};
int main1() {
	auto a = std::make_shared<A>();
	auto b = std::make_shared<B>();
	a->b = b; b->a = a; // 순환
	return 0;           // 소멸자 안 찍힘
}

// Person <-> Group
class Person;
class Group {
public:
	std::vector<std::shared_ptr<Person>> members;
	~Group() { cout << "Group destroyed\n"; }
};
class Person {
public:
	std::vector<std::shared_ptr<Group>> groups;
	~Person() { cout << "Person destroyed\n"; }
};
int main2() {
	auto p1 = std::make_shared<Person>();
	auto p2 = std::make_shared<Person>();
	auto g1 = std::make_shared<Group>();
	auto g2 = std::make_shared<Group>();

	p1->groups.push_back(g1);
	p2->groups.push_back(g2);
	g1->members.push_back(p1);
	g2->members.push_back(p2);
	return 0;           // 소멸자 안 찍힘
}

// Camera <-> Light
class Light;
class Camera {
public:
	std::vector<std::shared_ptr<Light>> lights;
	~Camera() { cout << "Camera destroyed\n"; }
};
class Light {
public:
	std::shared_ptr<Camera> camera;
	~Light() { cout << "Light destroyed\n"; }
};
int main3() {
	auto cam = std::make_shared<Camera>();
	auto l1 = std::make_shared<Light>();
	auto l2 = std::make_shared<Light>();
	cam->lights.push_back(l1);
	cam->lights.push_back(l2);
	l1->camera = cam;
	l2->camera = cam;
	return 0;           // 소멸자 안 찍힘
}