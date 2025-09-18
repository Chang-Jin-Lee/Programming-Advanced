
//---------------------------------------------------------------------------------
// array 배열을 전달해서 배열의 합을 구하려고 합니다. 
// error의 이유는? 어떻게 수정하면 될까요?

// 템플릿 인자로 사이즈를 넣어주면 됩니다.

#include <iostream>
#include <cstddef>

template<std::size_t n>
int sumArray( int (&arr)[n] )
{
	int sum = 0;
	for (const auto& number : arr)		//error
		sum += number;
	return sum;
}

int main()
{
	int arr[5] = { 9, 7, 5, 3, 1 };
	std::cout << sumArray(arr);
	return 0;
}


//---------------------------------------------------------------------------------
//범위기반 for 문을 사용해서 배열의 요소를 1씩 더하는 코드를 작성 하세요.
#include <iostream>
int main()
{
	int buff[10] = { 1,2,3,4,5 };

	//code
	for (auto& e : buff) e++;

	for (auto e : buff) std::cout << e << " "; std::cout << "\n";


	return 0;
}
//for (auto& e : buff) e++;


//---------------------------------------------------------------------------------
// B 클래스인지 검사해서 맞다면 맴버 값을 출력하는 코드를 작성하세요.
#include <iostream>

struct A { virtual void vf() {} };
struct B : public A { int m = 100; };

void memberPrint(A* a)
{
	if (B* b = dynamic_cast<B*>(a))
	{
		std::cout << b->m << "\n";
	}
	else
	{
		std::cout << "Not B class\n";
	}
}

int main() {
	A a;
	B b;
	memberPrint(&a);
	memberPrint(&b);
}
