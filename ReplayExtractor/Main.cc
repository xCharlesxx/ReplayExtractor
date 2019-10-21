#include "ReplayExtractor.h"
#include <sc2api/sc2_api.h>
#include <Windows.h>
#include <iostream>
#include <stdlib.h>

using namespace std;

const char* replayFolder = "D:\\replays\\**\\";

int main(int argc, char* argv[]) 
{
	string input1 = "D:\\replays\\**\\";
	string input2 = "C:\\replays\\";

	if (MoveFileA(input1.c_str(), input2.c_str()))
		cout << "Success" << endl;
	else
		cout << "Fail" << endl; 

	sc2::Coordinator coordinator;

	if (!coordinator.LoadSettings(argc, argv)) 
		return 1;

	if (!coordinator.SetReplayPath(replayFolder)) 
	{
		std::cout << "Unable to find replays." << std::endl;
		return 1;
	}

	ReplayExtractor replay_observer(20, 3, 2, "", "", sc2::Race::Zerg, sc2::Race::Terran, 0);
	coordinator.AddReplayObserver(&replay_observer);

	coordinator.SetRealtime(false); 
	//coordinator.AddCommandLine("command");
	coordinator.SetProcessPath("D:\\Games\\StarCraft II\\Versions\\Base75800\\SC2_x64.exe");
	coordinator.SetDataVersion("DDFFF9EC4A171459A4F371C6CC189554");


	//Impossible to change player perspective at the start of a game

	//coordinator.SetRender(sc2::RenderSettings);
	//coordinator.
	
	while (coordinator.Update());
	while (!sc2::PollKeyPress());
}