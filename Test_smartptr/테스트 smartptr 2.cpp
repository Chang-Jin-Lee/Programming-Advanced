// 사용자 이름과 전화 번호를 보관하는 map 데이타를 동적할당하고, (unique_ptr)

// 아래 정보를 입력한 후, 출력하세요.
// "user01", "010-1111-1111"
// "user02", "010-2222-2222"

//---------------------------------------------------------------------------------
#include <iostream>
#include <memory>
#include <map>

int main()
{
	using UserMap = std::map<std::string, std::string>;
	auto sptr = std::make_unique<UserMap>();

	sptr->insert(std::make_pair("user01", "010-1111-1111"));
	sptr->insert(std::make_pair("user02", "010-2222-2222"));

	for (auto& e : *sptr)
		std::cout << e.first << std::endl;
}


//------------------------------------------------------------------------
//unique_ptr 로 할당된 객체를 shared_ptr로 참조하도록 변경하세요.

#include <memory>  
#include <vector>
#include <iostream>
using namespace std;

int main() 
{
	unique_ptr<int> usp(new int);
	*usp = 10;

	shared_ptr<int> ssp1 = move(usp);

}