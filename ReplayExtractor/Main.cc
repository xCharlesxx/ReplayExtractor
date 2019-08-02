#include "ReplayExtractor.h"
const char* replayFolder = "A:/ProgramFiles/StarCraft II/Replays/Great/";

int main(int argc, char* argv[]) 
{
	sc2::Coordinator coordinator;
	if (!coordinator.LoadSettings(argc, argv)) 
		return 1;

	if (!coordinator.SetReplayPath(replayFolder)) 
	{
		std::cout << "Unable to find replays." << std::endl;
		return 1;
	}

	coordinator.SetRealtime(false); 
	//coordinator.AddCommandLine("command"); 
	//coordinator.SetDataVersion("3.16.1.55958");
	ReplayExtractor replay_observer(30, 2, "", "", sc2::Race::Zerg, sc2::Race::Terran, 0);

	//Impossible to change player perspective at the start of a game
	coordinator.AddReplayObserver(&replay_observer);
	//coordinator.SetRender(sc2::RenderSettings);
	//coordinator.
	
	while (coordinator.Update());
	while (!sc2::PollKeyPress());
}