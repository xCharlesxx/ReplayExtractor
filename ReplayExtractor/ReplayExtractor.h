#pragma once
#include <iostream>


#include "sc2api\sc2_api.h"
#include "sc2utils\sc2_manage_process.h"

//Replay class inherits from Replay Observer 
class ReplayExtractor : public sc2::ReplayObserver
{
public:
	ReplayExtractor(int _maxMins, int _minMins, int _numPlayers, std::string _mapName, std::string _version,
		sc2::Race _playerRace, sc2::Race _opponentRace, int _playerMMR);
	~ReplayExtractor();
	std::string TranslateRaceEnum(sc2::Race race); 
	void OnGameStart(); 
	bool IgnoreReplay(const sc2::ReplayInfo& replay_info, uint32_t& player_id);
	void DeleteFileInDir(std::string filepath);
	void OnUnitCreated(const sc2::Unit*);
	void OnUnitDestroyed(const sc2::Unit*);
	void OnUnitEnterVision(const sc2::Unit*); 
	void OnStep(); 
	void OnGameEnd(); 
	//Define values we want to record
	std::vector<uint32_t> count_units_built_;
	//Units known 
	std::vector<std::vector<sc2::Tag>> units_known_; 

private: 
	const sc2::UnitTypes& unit_types = Observation()->GetUnitTypeData();
	int maxMins, minMins, numPlayers, playerMMR;
	std::string mapName, version;
	sc2::Race playerRace, opponentRace;
	std::vector<std::string> Output; 
	int stepNum = 0; 
};

