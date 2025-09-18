//변수 aa의 자료형은?

#include <iostream>
using namespace std;

void main()
{
	const int& a = 0;
	auto aa = a;					//(	int	)
}

답)	int

//문제) 자료형은 ? ---------------------------------------

#include <iostream>
using namespace std;

void main()
{
	auto c = "abc";			  		//(	const char*	)	

	auto b = { 1, 2, 3, 4 };  		//(	initializer	)	

	auto f1 = { 1 };		  		//(	initializer	)	
	auto f2{ 1 };			  		//(	int	)	

	std::string g = { "abc" };		//(	string	)	
	auto h = g.begin();		  		//(	iterator	)	

	class Base {};
	auto base1 = new Base();  		//(	Base	)	-> Base*
	auto* base2 = new Base(); 		//(	Base*	)	

	int& r = ff;
	auto rr = r;			  		//(	int	)	

	int ff;
	const auto& cc = ff;	  		//(	const int&	)	
}

//문제) 자료형은 ? ---------------------------------------

#include <iostream>
using namespace std;
int main()
{
	const int v1 = 0;
	auto a = v1;				// a 의 자료형은?	int

	const int v2 = 0;
	auto& b = v2;				// b 의 자료형은?	const int&

	int* v3 = nullptr;
	auto c = v3;				// c 의 자료형은?	int*

	const int* v4 = 0;
	auto d = v4;				// d 의 자료형은?	const int*

	const int v5 = 0;
	auto&& e = v5;				// e 의 자료형은?	const int	-> const int&

	auto v6 = std::move(v1);
	auto&& f = v6;				// f 의 자료형은?	int&

	return 0;
}

class Widget {
public:
	void Fn() {}
};

//문제) 자료형은 ? ---------------------------------------

int main()
{
	Widget x;					// 아래 선언된 자료형을 추론해 보세요.

	auto o = x;					// ( Widget	)			

	auto* a1 = &x;				// (Widget*	)			
	auto  a2 = &x;				// (Widget*	)			
	a2->Fn();

	auto& r1 = x;				// (Widget&	)			
	auto r2 = x;				// (Widget	)			
	r1.Fn();

	const Widget y;				
	auto  obj1 = y;				// (Widget	)			
	auto  obj2 = &y;			// (Widget*	)	 -> const Widget*
	auto& obj3 = y;				// (const Widget	)	-> const Widget&

	auto&& obj4 = y;			// (const Widget&	)			
	auto&& obj5 = Widget();		// (Widget&&	)			
}


// 변수 a와 b의 차이를 설명하고, 자료형 혼선을 없애기 위한 방안을 제시하라

#include <iostream>
#include <vector>
using namespace std;

int main()
{
	std::vector<bool> v = { true,true,true,true };

	bool a = v[3];

	auto b = v[3];

	auto b2 = static_cast<bool>(v[3]);
}
