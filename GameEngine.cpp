#include "GameEngine.h"

#include "MapLoader.h"
#include "Map.h"
#include "Player.h"
#include "Cards.h"

#include <process.h>
#include <iostream>
#include <filesystem>
#include <vector>

#include <algorithm>
#include <random>
#include <exception>

namespace fs = std::filesystem;
using namespace std;

GameEngine::GameEngine() {
	validExecution = true;
}

GameEngine::~GameEngine() {
	delete map;
	for (auto player : players) {
		delete player;
	}
}

void GameEngine::gameStartPhase() {
	cout << "Initializing game engine..." << endl;

	string fileName = queryDirectory("maps");
	cout << "Loading " + fileName + " from file..." << endl;
	createMap("maps\\"+fileName);
	//map = Map::getTestMap(); //UNCOMMENT ABOVE WHEN MAPLOADER IS FIXED.

	cout << "Checking map validity..." << endl;
	if (map->validate()) {
		cout << "Map is valid!" << endl;
	}
	else {
		cout << "Map is invalid, terminating..." << endl;
		validExecution = false;
	}

	if (validExecution) {
		cout << "Creating players...";
		int playerCount = queryPlayerCount();
		cout << "Creating players..." << endl;
		players = createPlayers(playerCount);

		deck = new Deck(10, 10, 10, 10, 10, 10);

		//Enable/Disable Observers HERE, ask JT when it is go time.
	}
	cout << "\n";
}

void GameEngine::startupPhase() {
	if (!validExecution) {
		cout << "Execution invalid, cancelling startup phase..." << endl;
	}
	else {

		cout << "Running startup phase..." << endl;

		//Shuffle elements in players.
		cout << "Shuffling player list..." << endl;
		auto rng = std::default_random_engine{};
		std::shuffle(std::begin(players), std::end(players), rng);

		//Assign all territories to players.
		cout << "Assigning territories to players..." << endl;
		vector<Territory*> toAssign = map->getTerritories();
		assignTerritoriesToPlayers(players, toAssign);

		//Assign initial army counts.
		cout << "Assigning initial armies..." << endl;
		assignInitialArmies(players);
	}
}

string GameEngine::queryDirectory(string directory) {
	vector<string> namelist;
	cout << "Files names:" << endl;
	try
	{
		for (const auto& entry : fs::directory_iterator(directory)) {
			cout << entry.path().filename() << endl;
		}
	}
	catch (exception& e)
	{
		cout << "Unable to display file contents..." << endl;
	}

	cout << "\n";

	cout << "Enter the file path of a map file: ";

	string path = "";
	cin >> path;

	cout << "\n";

	return path;
}

void GameEngine::createMap(string path) {
	MapLoader mapLoader = MapLoader();
	
	//Add continents
	vector<Territory*> continentList = mapLoader.GetContinentList();
	continentList = mapLoader.ReadMapFile(path, continentList);

	//Add territories
	vector<Territory*> countryList = mapLoader.GetCountryList();
	countryList = mapLoader.ReadMapFileForCountries(path, countryList);

	//Add borders
	vector<vector<Territory*>> bordersList = mapLoader.GetBordersList();
	bordersList = mapLoader.ReadMapFileForBorders(path, bordersList, countryList);

	//Create the map

	map = mapLoader.CombineInfos(continentList, countryList, bordersList);
}

int GameEngine::queryPlayerCount() {
	cout << "Enter the number of players(2-5): ";

	int count;
	cin >> count;
	cout << "\n";

	if (count < 2) {
		cout << "Input too small, rounding player count up to 2." << endl;
		count = 2;
	}
	else if(count > 5){
		cout << "Input too large, rounding player count down to 5." << endl;
		count = 5;
	}

	return count;
}

vector<Player*> GameEngine::createPlayers(int playerCount) {
	vector<Player*> players;
	for (int i = 0; i < playerCount; i++) {
		vector<Territory*> list;
		players.push_back(new Player(list, new Hand(0, 0, 0, 0, 0, 0), i));
	}
	return players;
}

void GameEngine::assignTerritoriesToPlayers(vector<Player*> playerList, vector<Territory*> territoryList) {
	int playerIndex = 0;
	int territoryIndex = 0;


	if (playerList.size() > 0) {
		while (territoryIndex < territoryList.size()) {
			Map::assignTerritory(playerList.at(playerIndex), territoryList.at(territoryIndex));

			territoryIndex++;
			playerIndex++;

			if (playerIndex >= playerList.size()) {
				playerIndex = 0;
			}
		}
	}
}

void GameEngine::assignInitialArmies(vector<Player*> playerList) {
	int armyCount = 0;
	
	if (playerList.size() <= 2) {
		armyCount = 40;
	}
	else if (playerList.size() == 3) {
		armyCount = 35;
	}
	else if (playerList.size() == 4) {
		armyCount = 30;
	}
	else if (playerList.size() >= 5) {
		armyCount = 25;
	}

	cout << armyCount;
	cout << " armies are being assigned to each player..." << endl;

	for (auto player : playerList) {
		player->mapPlayed = map;
		Hand* playerHand = player->getHand();
		player->numOfArmies = armyCount;
		player->gameDeck = deck;
	}
	
}

void GameEngine::mainGameLoop()
{
	while (!map->checkWinner(players))
	{
		
		reinforcementPhase();
		
		playersIssuingOrders = players;
		orderIssuingPhase();
		playersExecutingOrders = players;
		orderExecutionPhase();
		for(auto it:players)
		{
			std::cout << it->getOwnedTerritories().size() << endl;
			
		}
	}
}
void GameEngine::reinforcementPhase()
{
	std::cout << "Reinforcement"<<endl;
	for (auto p : players) {
		vector<Territory*> myvec = p->territories;
		p->numOfArmies = floor(myvec.size() / 3);
		bool owns;
		int continentbonus = 4;//need to get this property from map/maploader for each continent
		
		for (auto& it  :map->getContinentMap())
		{
			
			owns = map->checkContinentOwnership(p, it.second);
			if (owns)
			{
				p->numOfArmies += continentbonus;
			}
		}
		if (p->numOfArmies < 3)
		{
			p->numOfArmies +=3;
		}
		p->tempArmies = p->numOfArmies;
	}
}
void GameEngine::orderIssuingPhase()
{
	std::cout << "Order" << endl;
	
	bool allDone = false;
	while(!allDone)
	{
		allDone = true;
		for(auto it:playersIssuingOrders)
		{
			if(it->doneIssue==false)
			{
				it->issueOrder();
				it->doneIssue = true;//This is sketchy, probs allows only one turn.
				allDone = false;
			}
		}
		
	}
	for (auto it : players)
	{
		it->doneDefence = false;
		it->doneAdvance = false;
		it->doneAttack = false;
		it->doneIssue = false;
		it->roundwiseordercount = 0;
		//TO DELETE PLAYER WHO HAS LOST
		/**/
	}
}
void GameEngine::orderExecutionPhase()
{
	std::cout << "OE" << endl;
	bool allDone = false;
	while (!allDone)
	{
		std::cout << players.back()->getTerritories2().size() << endl;
		
		allDone = true;
		for (auto it : players)
		{
			for(auto orderit:it->getOrderList()->getOrders())
			{
				
				if((orderit->name=="Deploy")&&!(orderit->executed))
				{
					if (!orderit->executed)
					{
						orderit->execute();
						allDone = false;
						std::cout << "Deploy Executed" << endl;
						break;
					}
				}
				else if((orderit->name=="Airlift") && !(orderit->executed))
				{
					if (!orderit->executed)
					{
						orderit->execute();
						allDone = false;
						break;
					}
				}
				else if ((orderit->name == "Blockade") && (!orderit->executed))
				{
					if (!orderit->executed)
					{
						orderit->execute();
						allDone = false;
						break;
					}
				}
				else 
				{
					if (!orderit->executed)
					{
						orderit->execute();
						allDone = false;
						break;
					}
				}
				
			}
		}
	}
	for (auto it : players)
	{
		
		if (it->getOwnedTerritories().size() == 0)
		{
			std::vector<Player*>::iterator position = std::find(players.begin(), players.end(), it);
			if (position != players.end()) // == myVector.end() means the element was not found
			{
				players.erase(position);
			}

		}
	}
	
}

