//-------------------------------------------------------------------------
//아래 코드를 스마트 포인터로 변경하세요. unique_ptr

#include <iostream>
using namespace std;

int main(void)
{
	int* ptr(new int(5));
	cout << *ptr << endl;
	delete ptr;
}

#include <iostream>
#include <memory>
using namespace std;

int main(void)
{
	unique_ptr<int> p = make_unique<int>(5);
	cout << *p << endl;
}



//-------------------------------------------------------------------------
//소멸할 때 자동으로 삭제되도록 스마트 포인터 class 를 작성하세요.

#include <iostream>
#include <memory>
using namespace std;

class A
{
    int* data;
public:
    A() { data = new int[100]; }
    ~A() { delete[] data; }
    void func() { std::cout << "일반 포인처럼 사용!" << std::endl; }
};


// 연산자를 만들고 복사생성 안되게 막기.
// 그리고 템플릿화 잊지 말기
template<typename T>
class smart_ptr
{
    T* data;
public:
    smart_ptr(T* d) : data(d) {}
    ~smart_ptr() { delete data; }
    smart_ptr(const smart_ptr& data) = delete;
	T* operator->() { return data; }          //형변환 연산자
};

void main()
{
    smart_ptr<A> sp(new A());      //smart_ptr<T> 
    sp->func();
}
