//============================================================================
// Name        : Player.cpp
// Author      : Jeremiah Tiongson (40055477)
// Description : Player compilation unit class.
//============================================================================

#include "Player.h"
#include "Order.h"
#include <ostream>
#include <vector>
#include <random>

using std::string;
using std::vector;

//fully parametrized constructor
Player::Player(vector<Territory*> playerTerritories, Hand* playerHand, string playerName)
{
	territories = playerTerritories;
	hand = playerHand;
	name = playerName;
	this->orders = new OrderList();
}

//copy constructor
Player::Player(const Player& p)
{
	name = p.name;
	territories = p.territories;
	hand = p.hand;
}
bool Player::isNegotiated(Player* p1, Player* p2)
{
	vector<Player*> myvec = p1->negotiated;
	vector<Player*>::iterator it = myvec.begin();
	for (it = myvec.begin(); it != myvec.end(); ++it)
	{


		if (*it == p2)
		{
			return true;
		};
		

	};
	return false;
}

//assignment operator overloading
Player* Player:: operator = (Player& p)
{
	return p.getNew();
}

//Gets the users' territories if the pointer is not null
vector<Territory> Player::getTerritories()
{
	vector<Territory> playerTerritories;

	for (auto it = territories.begin(); it != territories.end(); ++it)
	{
		if (it.operator*() != nullptr)
		{
			playerTerritories.push_back(*it.operator*());
		}
	}

	return playerTerritories;
}
vector<Territory*> Player::getTerritories2()
{
	return territories;
}
//Shuffles the player's territories and removes the last two if the size is greater than 2
vector<Territory> Player::toDefend()
{
	vector<Territory> terr_toDefend = getTerritories();
	std::random_device rd;
	std::mt19937 g(rd());
	std::shuffle(terr_toDefend.begin(), terr_toDefend.end(), g);

	//Remove last two territories
	if (terr_toDefend.size() > 2)
	{
		terr_toDefend.pop_back();
		terr_toDefend.pop_back();
	}

	return terr_toDefend;
}

//Shuffles the player's territories and removes the last territory is size is greater than 1
vector<Territory> Player::toAttack()
{
	vector<Territory> terr_toAttack = getTerritories();
	std::random_device rd;
	std::mt19937 g(rd());
	std::shuffle(terr_toAttack.begin(), terr_toAttack.end(), g);

	//Remove last two territories
	if (terr_toAttack.size() > 1)
	{
		terr_toAttack.pop_back();
	}

	return terr_toAttack;
}

//Issues a Bomb order and adds it to the Player's list of orders
/*void Player::issueOrder()
{
	Order* order = new Bomb();
	if(order != nullptr)
	{
		std::cout << "Bomb order successfully placed.\n\n";
		orders->add(order);
		//Print the list to verify that order is indeed in the list
		orders->printlist();
	}
}*/

//Returns the hand if it has a valid pointer to it
Hand Player::getHand()
{
	Hand h = Hand();
	if (hand != nullptr) 
		return *hand;
	return h;
}

Player* Player::getNew()
{
	return new Player(*this);
}

//ostream operator for Player prints number of territories and indicates whether the hand is valid or not
std::ostream& operator <<(ostream& out, const Player& p)
{
	if(p.hand != nullptr)
	{
		out << "\nPlayer: " << p.name << " has " << p.territories.size() << " territories and a valid Hand.\n";
	}else
	{
		out << "\nPlayer: " << p.name << " has " << p.territories.size() << " territories and an empty Hand.\n";
	}
	
	return out;
}