// auto_explicitly_typed_initializer.cpp
// g++ -std=c++17 auto_explicitly_typed_initializer.cpp

#include <iostream>
#include <vector>
#include <string>

using namespace std;

struct Widget {};

// �ӽ� vector<bool>�� ��ȯ (�ǵ������� ���Ͻ�/���� �̽��� ������ �� �ִ� ����)
std::vector<bool> features(const Widget& w) {
	std::vector<bool> v(5, false);
	v[3] = true; // ���Ƿ� �� ��Ʈ�� true
	return v;    // RVO�� ��ȯ�Ǵ� �ӽ� �����̳�
}

double calcEpsilon() { return 1.1; }

int main() {
	// 1) �⺻ ��: auto& / decltype((x)) / �迭 ��� ����
	{
		int a = 0;
		auto& val1 = a;          // int&
		decltype((a)) val2 = a;  // int& (��ȣ�� lvalue ó��)
		int arr[] = { 1,2,3 };
		auto& val3 = arr[0];     // int&
		(void)val1; (void)val2; (void)val3;
	}

	// 2) ���Ͻ� Ÿ�� ���� ���� (UB �ڵ�: �ּ��� ����)
	{
		Widget w;

		// ����: ��� bool�� ��ȯ�Ͽ� ���� (�ӽ� vector<bool>�� ��������)
		bool highPriority1 = features(w)[3];
		cout << "[bool   ] highPriority1 = " << highPriority1 << "\n";

		// ����(����): ���Ͻ� Ÿ���� auto�� ����Ǿ� �ӽ� �����̳� �Ҹ� �� UB ����
		// auto highPriority2 = features(w)[3];    // ���� (���Ͻ� ����, UB ����)
		// cout << "[auto??] highPriority2 = " << highPriority2 << "\n";

		// �ذ�: ����� Ÿ�� �ʱ�ġ ���뱸
		auto highPriority3 = static_cast<bool>(features(w)[3]);
		cout << "[cast  ] highPriority3 = " << highPriority3 << "\n";
	}

	// 3) ��� ��ȯ �ǵ� ���
	{
		// ������ ���
		float ep1 = calcEpsilon();
		// �ǵ���/����� ���
		auto  ep2 = static_cast<float>(calcEpsilon());
		cout << "[epsilon] ep1=" << ep1 << " ep2=" << ep2 << "\n";
	}

	// 4) vector<bool> ���� ��ü�� ��쿡�� ������ �����ϰ� �ʹٸ� cast
	{
		vector<bool> bits = { true, false, true };
		// ���Ͻø� ��(bool)����
		auto b0 = static_cast<bool>(bits[0]);
		auto b1 = static_cast<bool>(bits[1]);
		cout << "[bits] b0=" << b0 << " b1=" << b1 << "\n";
	}

	return 0;
}