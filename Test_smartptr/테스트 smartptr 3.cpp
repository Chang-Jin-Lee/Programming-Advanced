//Circular references - 순환 참조 문제 해결하세요.

#include <iostream>
#include <memory>
#include <vector>
using namespace std;

class Player;

class Party
{
public:
    Party() {}
    ~Party() { m_MemberList.clear(); std::cout << "~Party()" << "\n"; }
public:
    void AddMember(const shared_ptr<Player>& member) {
        m_MemberList.push_back(member);
    }
private:
    vector<shared_ptr<Player> > m_MemberList;
};

class Player
{
public:
    Player() {}
    ~Player() { std::cout << "~Player()" << "\n"; }
    void SetParty(const shared_ptr<Party>& party) {
        m_Party = party;
    }
private:
    weak_ptr<Party> m_Party;
};

int main() {
    shared_ptr<Party> party(new Party);

    for (int i = 0; i < 3; i++)
    {
        shared_ptr<Player> player(new Player);

        party->AddMember(player);
        player->SetParty(party);
    }

    party.reset();
}
