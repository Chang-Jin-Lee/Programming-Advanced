//����)

#include <iostream>
using namespace std;
int main()
{
	const int v1 = 0;
	auto a = v1;				// [O] a �� �ڷ�����? int

	const int v2 = 0;
	auto& b = v2;				// [O] b �� �ڷ�����? const int&

	int* v3 = nullptr;
	auto c = v3;				// [O] c �� �ڷ�����? int*

	const int* v4 = 0;
	auto d = v4;				// [X] d �� �ڷ�����? int*
								// �̰� const int* ��.
	const int v5 = 0;
	auto&& e = v5;				// [O] e �� �ڷ�����? const int&

	auto v6 = std::move(v1);
	auto&& f = v6;				// [O] f �� �ڷ�����? int&

	return 0;
}


#include <iostream>
#include <array>
using namespace std;

int main()
{
	const int v1 = 0;
	auto v2 = std::move(v1);	// int
	auto&& val1 = v2;			// int&
}

// val1 : int &


int main()
{
	const int* ca = 0;
	auto val1 = ca;				// const int*
	auto&& val2 = val1;			// const int*&
}

// val1 : const int *
// val2 : const int *&

