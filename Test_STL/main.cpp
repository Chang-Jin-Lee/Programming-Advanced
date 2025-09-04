/*
* @brief   : STL 연습 과제 — 컨테이너/알고리즘/함수객체/람다 실습
* @details :
*   - 이 파일은 vector/list/map과 표준 알고리즘(for_each, sort, unique, remove_if,
*     accumulate, count_if, splice 등)을 예제로 다룹니다.
*   - 핵심 포인트
*       1) remove–erase idiom의 정확한 사용 (특히 sequence 컨테이너)
*       2) unique는 "연속" 중복만 제거 → 보편적 중복 제거 시 정렬 후 unique
*       3) list 전용 멤버(remove_if, splice) 활용
*       4) 비교 함수/함수객체의 const-정확성
*   - 참고: 표준 C++의 진입점은 int main() 권장(현 코드는 void main()).
*/

#include <iostream>
#include <vector>
#include <list>
#include <map>
#include <algorithm>
#include <functional>
#include <numeric>
#include <string> // @brief: std::string 사용을 위한 헤더

using namespace std;

/*
* @brief   : 내림차순 비교 함수 객체
* @details :
*   - sort의 비교자로 사용합니다.
*   - 매개변수는 값 복사 가능하나, const 참조 + operator() const 권장.
*/
struct CompareGrater {
	bool operator()(int a, int b) {
		return a > b;
	}
};

void main()
{
	/*
	* @brief   : 문자열을 vector에 담아 출력
	* @details :
	*   - 컨테이너: vector<string>
	*   - 알고리즘: for_each + 람다
	*   - 출력 포맷: 공백 구분 일렬 출력
	*/
	// "apple","melon","banana"
	vector<string> fruits = { "apple", "melon", "banana" };
	for_each(fruits.begin(), fruits.end(), [](const string& fruit) {cout << fruit << " "; });
	cout << endl;

	/*
	* @brief   : 벡터를 내림차순 정렬 후 출력
	* @details :
	*   - 컨테이너: vector<int>
	*   - 알고리즘: sort(비교자에 함수객체 사용), for_each로 출력
	*   - 비교자: CompareGrater (철자상 Greater가 자연스럽지만 의도대로 동작)
	*/
	{
		vector<int>  vec = { 1,3,2,7,5,9 };
		sort(vec.begin(), vec.end(), CompareGrater());
		for_each(vec.begin(), vec.end(), [](int n) {cout << n << " "; });
	}
	cout << endl;

	/*
	* @brief   : 벡터에서 10보다 큰 원소 제거
	* @details :
	*   - 컨테이너: vector<int>
	*   - 알고리즘: remove_if로 "뒤로 몰기", erase로 꼬리 구간 삭제(정석 패턴)
	*   - 패턴명: remove–erase idiom
	*/
	{
		vector<int>  v = { 11, 3, 25, 71, 5, 9, 12, 7, 89 };
		v.erase(remove_if(v.begin(), v.end(), [](const int& n) { return n > 10; }), v.end());
		for_each(v.begin(), v.end(), [](int& n) { cout << n << " "; });
	}
	cout << endl;

	/*
	* @brief   : 벡터에서 홀수의 개수 세기
	* @details :
	*   - 컨테이너: vector<int>
	*   - 알고리즘: count_if(람다로 홀수 조건 지정)
	*/
	{
		vector<int>  v = { 3,2,7,9,4,1,3 };
		cout << count_if(v.begin(), v.end(), [](const int& n) { return n % 2 != 0; }) << endl;
	}
	cout << endl;

	/*
	* @brief   : 벡터의 중복 제거 (unique 사용)
	* @details :
	*   - unique는 "연속된" 중복만 제거합니다.
	*   - 일반적 의미의 중복 제거를 원하면 sort - unique - erase 순서를 권장합니다.
	*   - 현재 코드는 연속 중복만 제거한다는 점을 주석으로 명시합니다.
	*   - 예) {1,1,2,3,4,5,3,3,5,6} → unique만 쓰면 뒤쪽 3,5는 남음.
	*/
	{
		vector <int> v = { 1,1,2,3,4,5,3,3,5,6 };
		v.erase(unique(v.begin(), v.end()), v.end());
		for_each(v.begin(), v.end(), [](int& n) { cout << n << " "; });
	}
	cout << endl;

	/*
	* @brief   : 리스트에서 10보다 작은 숫자 제거
	* @details :
	*   - 컨테이너: std::list<int>
	*   - 주의: list에는 list::remove_if(멤버)가 있어 그 사용을 권장합니다.
	*   - remove–erase idiom을 쓰려면 erase(remove_if(...), end()) 형태여야 합니다.
	*   - 현 코드: erase(iterator) 한 개 원소만 지우므로 의도와 다를 수 있음(학습 주석).
	*   - 권장 대안(예시):
	*       ls.remove_if([](int n){ return n < 10; });            // list 멤버
	*       // 또는
	*       ls.erase(remove_if(ls.begin(), ls.end(),
	*                          [](int n){ return n < 10; }), ls.end()); // idiom
	*/
	list<int> ls = { 11, 3, 25, 71, 5, 9, 12, 7, 89 };
	ls.erase(remove_if(ls.begin(), ls.end(), [](const int& n) { return n < 10; }));
	for_each(ls.begin(), ls.end(), [](int& n) { cout << n << " "; });
	cout << endl;

	/*
	* @brief   : 리스트에서 특정 값(2) 삭제
	* @details :
	*   - 패턴: erase(remove_if(...), end()) 사용 → 값이 2인 원소 제거
	*   - 대안: ls.remove_if([](int n){ return n == 2; }); (list 전용 멤버)
	*/
	{
		std::list<int> ls = { 1,2,2,3,2,4,2 };
		ls.erase(remove_if(ls.begin(), ls.end(), [](const int& n) { return n == 2; }), ls.end());
		for_each(ls.begin(), ls.end(), [](int& n) { cout << n << " "; });
	}
	cout << endl;

	/*
	* @brief   : 두 리스트를 하나로 합치기(splice)
	* @details :
	*   - 요구 결과 예시: { 1,2,10,20,30,3,4,5 }
	*   - 현 구현: 빈 리스트에 list1, list2를 순서대로 splice → {1,2,3,4,5,10,20,30}
	*   - 특정 위치(예: 2 뒤)에 끼워 넣으려면 pos를 계산하여 splice 해야 함(학습 포인트).
	*/
	list<int> list1 = { 1,2,3,4,5 };
	list<int> list2 = { 10,20,30 };

	list<int> splicedList;
	splicedList.splice(splicedList.end(), list1);
	splicedList.splice(splicedList.end(), list2);
	for_each(splicedList.begin(), splicedList.end(), [](int& n) { cout << n << " "; });
	cout << endl;

	/*
	* @brief   : 리스트 각 원소에 2를 더한 "값"을 출력
	* @details :
	*   - 변형 출력: 원본을 변경하지 않고 n+2를 출력만 합니다.
	*   - 원본 변경을 원한다면 for (int& n : list3) n += 2; 방식 사용.
	*/
	list<int> list3 = { 1,2,3,4,5 };
	for_each(list3.begin(), list3.end(), [](int& n) { cout << n + 2 << " "; });
	cout << endl;

	/*
	* @brief   : 벡터 원소 합 구하기
	* @details :
	*   - 알고리즘: std::accumulate
	*   - 초기값: 0 (int 합)
	*/
	{
		std::vector<int> v2{ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
		int acumulatedSum = accumulate(v2.begin(), v2.end(), 0);
		cout << acumulatedSum;
	}
	cout << endl;

	/*
	* @brief   : map을 이용해 (이름, 점수) 랭킹을 출력
	* @details :
	*   - 컨테이너: map<string,int> → (정렬을 위해) vector<pair<...>>로 복사
	*   - 정렬 기준: 점수 내림차순 (동점자 처리 기준 추가 가능)
	*   - 출력: 이름과 점수
	*/
	// 민수 10000, 수연 5200, 지수 2500, 민호 5200
	map<string, int> scoreMap{ {"민수", 10000}, {"수연", 5200}, {"지수", 2500}, {"민호", 5200} };
	vector<pair<string, int>> scoreVec(scoreMap.begin(), scoreMap.end());
	sort(scoreVec.begin(), scoreVec.end(), [](auto& a, auto& b) { return a.second > b.second; });
	for_each(scoreVec.begin(), scoreVec.end(), [](const auto& unit) { cout << ' ' << unit.first << ' ' << unit.second << '\n'; });
	cout << endl;

	/*
	* @brief   : 범위[7,10]에 속하는 숫자의 개수 세기
	* @details :
	*   - 컨테이너: vector<int>
	*   - 알고리즘: count_if (경계 포함: >=7 && <=10)
	*/
	vector<int> arr = { 1,3,5,5,7,8,8,10,10,11,13 };
	cout << count_if(arr.begin(), arr.end(), [](const int& n) { return n >= 7 && n <= 10; }) << endl;
}
