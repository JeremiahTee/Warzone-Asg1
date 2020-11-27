#pragma once
#include "Cards.h"
#include "Map.h"

class Player;

class GameEngine : public Subject
{
public:
	Map* map;
	vector<Player*> players;
	vector<Player*> playersIssuingOrders;
	vector<Player*> playersExecutingOrders;
	Deck* deck;
	bool validExecution;
	int phase;
	Player* neutral;
	GameEngine();
	~GameEngine();
	int orderattempts=0;
	void gameStartPhase();
	void startupPhase();
	string queryDirectory(string directory);
	void createMap(string path);
	int queryPlayerCount();
	void createPlayers(int playerCount);
	void assignTerritoriesToPlayers(vector<Player*> playerList, vector<Territory*> territoryList);
	void assignInitialArmies(vector<Player*> playerList);
	void mainGameLoop();
	void reinforcementPhase();
	void orderIssuingPhase();
	void orderExecutionPhase();
	void eraseLosers();
	//Observer methods
	void enableGamePhaseObservers();
	void enableGameStatsObservers();
	void disableGamePhaseObservers();
	void disableGameStatsObservers();
};