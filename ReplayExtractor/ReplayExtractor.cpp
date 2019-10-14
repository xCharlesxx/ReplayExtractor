#include "ReplayExtractor.h"
#include <Windows.h>
#include <winapifamily.h>

ReplayExtractor::ReplayExtractor(int _maxMins, int _minMins, int _numPlayers, std::string _mapName, std::string _version,
	sc2::Race _playerRace, sc2::Race _opponentRace, int _playerMMR)
{
	maxMins = _maxMins;
	minMins = _minMins; 
	numPlayers = _numPlayers; 
	mapName = _mapName; 
	version = _version; 
	playerRace = _playerRace;
	opponentRace = _opponentRace;
	playerMMR = _playerMMR; 
}


ReplayExtractor::~ReplayExtractor()
{
}

std::string ReplayExtractor::TranslateRaceEnum(sc2::Race race)
{
	switch (race)
	{
	case sc2::Terran:
		return "Terran"; 
		break;
	case sc2::Zerg:
		return "Zerg";
		break;
	case sc2::Protoss:
		return "Protoss"; 
		break;
	case sc2::Random:
		return "Random"; 
		break;
	default:
		break;
	}
	return "Error: invalid race";
}
	//ReplayExtractor() : sc2::ReplayObserver() {}


//Add conditions to this function to Ignore replays in your replay folder (By returning false)
//Used for sorting replay files
bool ReplayExtractor::IgnoreReplay(const sc2::ReplayInfo& replayInfo, uint32_t& player_id) 
{
	int zergPlayer = 0; 
	//Check https://github.com/Blizzard/s2client-api/pull/273 request progress
	if (replayInfo.players[0].race == playerRace && replayInfo.players[1].race == opponentRace)
		zergPlayer = 0; 
	else if (replayInfo.players[0].race == opponentRace && replayInfo.players[1].race == playerRace)
		zergPlayer = 1; 
	else
	{
		std::cout << "Not valid replay: Matchup is: " << TranslateRaceEnum(replayInfo.players[0].race) << " vs "
														<< TranslateRaceEnum(replayInfo.players[1].race) << std::endl;

		std::cout << "Not valid replayselect: Matchup is: " << TranslateRaceEnum(replayInfo.players[0].race_selected) << " vs "
			<< TranslateRaceEnum(replayInfo.players[1].race_selected) << std::endl;
		//if (DeleteFileA(replayInfo.replay_path.c_str()))
		//	std::cout << "FileDeleted" << std::endl;
		//else
		//	std::cout << "Error: File not Deleted" << std::endl;

		return true;
	}

	if (replayInfo.duration / 60 > maxMins || replayInfo.duration / 60 < minMins)
	{
		std::cout << "Not valid replay: Replay length is: " << replayInfo.duration / 60 << std::endl;
		return true;
	}

	if (replayInfo.players[numPlayers].player_id != 0)
	{
		std::cout << "Not valid replay: Number of players exceeds: " << numPlayers << std::endl; 
		return true;
	}
	if (replayInfo.players->mmr < 1000)
	{
		std::cout << "Not valid replay: MMR: " << replayInfo.players[zergPlayer].mmr << std::endl;
		return true; 
	}

	stepNum++;
	std::string dir = "A:/ProgramFiles/StarCraft II/Replays/" + replayInfo.map_name + "/";
	CreateDirectoryA(dir.c_str(), NULL); 
	std::string path = dir + std::to_string(stepNum) + ".SC2Replay";

	if (!CopyFile(replayInfo.replay_path.c_str(), path.c_str(), true))
		std::cout << "Error File Already Exists" << std::endl;
	return true;
}

void ReplayExtractor::OnGameStart()
{
	//const sc2::ObservationInterface* obs = Observation();
	//if (!IsValidReplay(30, 2, "", "", sc2::Race::Zerg, sc2::Race::Terran, 0))
	//{
	//sc2::ReplayInfo replayInfo = ReplayControl()->GetReplayInfo();
	//uint32_t playerID = replayInfo.players[0].player_id;
	//IgnoreReplay(replayInfo, playerID);
	//std::cout << "IgnoredReplay\n";
	//}

		//Get observation data 

	//	sc2::GameInfo gi = obs->GetGameInfo(); 
	//	//gi.options.raw = true; 


	////Check observation data isn't empty
	//assert(obs->GetUnitTypeData().size() > 0);
	////Resize vectors to number of valid unit IDs
	//count_units_built_.resize(obs->GetUnitTypeData().size());
	//units_known_.resize(obs->GetUnitTypeData().size()); 
	////Fill vector
	//std::fill(count_units_built_.begin(), count_units_built_.end(), 0);
}

void ReplayExtractor::OnUnitCreated(const sc2::Unit* unit)
{
	//Assert: if expression is false then abort 
	//If unit type is valid (unit id is within bounds of count_units_built_)
	assert(uint32_t(unit->unit_type) < count_units_built_.size());
	++count_units_built_[unit->unit_type];
}

void ReplayExtractor::OnStep()
{
	//const sc2::ObservationInterface* obs = Observation();
	//Output.push_back(obs->GetMinerals() + "," + obs->GetVespene());
}

void ReplayExtractor::OnUnitDestroyed(const sc2::Unit* unit)
{
	//Self=1 Enemy=4
	if (unit->alliance == 4)
	{
		std::cout << "Killed Enemy\n";
		if (units_known_[unit->unit_type].size() == 0)
		{
			std::cout << "Error, enemy killed that was never seen\n";
			return;
		}

		for (uint32_t i = 0; i < units_known_[unit->unit_type].size(); ++i)
		{
			//if unit has already been seen before, remove from seen list
			if (units_known_[unit->unit_type][i] == unit->tag)
				units_known_[unit->unit_type].erase(units_known_[unit->unit_type].begin() + i);
		}
	}
	else if (unit->alliance == 1)
	{
		std::cout << "Unit Killed\n";
		//Assert: if expression is false then abort 
		//If unit type is valid (unit id is within bounds of count_units_built_)
		assert(uint32_t(unit->unit_type) < count_units_built_.size());
		--count_units_built_[unit->unit_type];
	}
}

void ReplayExtractor::OnUnitEnterVision(const sc2::Unit* unit)
{
	if (unit->alliance == 4)
	{
		uint32_t lastSeen = unit->last_seen_game_loop; 
		for (uint32_t i = 0; i < units_known_[unit->unit_type].size(); ++i)
		{
			//if unit has already been seen before
			if (units_known_[unit->unit_type][i] == unit->tag)
				return; 
		}
		//Else add to units_known_
		units_known_[unit->unit_type].push_back(unit->tag);
		std::cout << "Units known:\n";
		for (uint32_t i = 0; i < units_known_.size(); ++i)
		{
			if (units_known_[i].size() == 0)
				continue;

			std::cout << unit_types[i].name << ": " << std::to_string(units_known_[i].size()) << std::endl;
		}
		std::cout << std::endl; 
	}
}

void ReplayExtractor::OnGameEnd()
{
	//Print all units created
	std::cout << "Units created:\n";
	for (uint32_t i = 0; i < count_units_built_.size(); ++i)
	{
		if (count_units_built_[i] == 0)
			continue;

		std::cout << unit_types[i].name << ": " << std::to_string(count_units_built_[i]) << std::endl;
	}
	for (uint32_t i = 0; i < Output.size(); ++i)
		std::cout << "Minerals & Vespine: " << Output[i] << std::endl;

	std::cout << "Finished" << std::endl;
}
