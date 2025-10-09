#include "G4RunManagerFactory.hh"
#include "G4RunManager.hh"
#include "G4MTRunManager.hh"

// My classes
#include "DetectorConstruction.hh"
#include "PrimaryGeneratorAction.hh"
#include "RunAction.hh"
#include "EventAction.hh"
#include "ActionInitialization.hh"

// Physics 
#include "FTFP_BERT.hh"
#include "G4OpticalPhysics.hh"
#include "G4OpticalParameters.hh"

// Visualization and UI
#include "G4VisExecutive.hh"
#include "G4UIExecutive.hh"
#include "G4UImanager.hh"

#include "G4SystemOfUnits.hh"
#include "Randomize.hh"
#include <ctime>


int main(int argc, char** argv) {
	
	// Set a different random seed based on current time / user preferences
	CLHEP::HepRandom::setTheSeed(time(NULL));


	#pragma region Simulation Parameters
	// A better implementation could be achieved using a json file to set these parameters (i may consider it later)

	G4bool enableTrackingVerbose = false;
	G4bool enableVis = true;	// enable visualization, set to false when running heavy batch jobs
	G4int threads = 20;			// number of threads to be used in MT mode, ignore in ST mode
	
	G4String siliconPMSDName = "/SiliconPM";
	G4String scintSDName = "/Scintillator";

	G4String opCName = "OpticalPhotonHitsCollection";
	G4String muCName = "MuHitsCollection";

	G4double worldSizeXYZ = 1. * m;
	
	// This is more of an explode factor between the solid components
	// I use it mostly for visualization/debug purposes
	G4double gap = 0 * cm; // must be set to 0 in the final version 

	// The thickness of the reflective coating has yet to be formally established
	// but for the purposes of this project any reasonable value will do
	G4double coatingThickness = 0.1 * mm;
	G4double siPMThickness = 3 * mm;

	// The size of the scintillator has yet to be formally established
	// but for the purposes of this project any reasonable value will do
	BoxGeometry scintGeometry = BoxGeometry{
		50. * mm, 
		50. * mm, 
		3 * mm 
	};

	// I set these values to match those of a BC-412 scintillator 
	// (the full datasheet can be found at https://www.robot-domestici.it/joomla/component/virtuemart/scintillatore-plastico-50--50--5-mm/getfile?file_id=5392)
	ScintillatorProperties scintData = ScintillatorProperties{
		scintGeometry,
		1,			// scalingFactor: lower values reduce the scintillation yield
		8000.,			// scintYield (photon/MeV)
		2.85 * eV,		// waveLengthPeak	(434 nm)
		3.09 * eV,		// waveLengthLeft	(402 nm)
		2.41 * eV,		// waveLengthRight	(514 nm)
		3.3 * ns,		// decayTime
		1.58,			// refractiveIndex
		210. * cm,		// absorptionLength
	};

	#pragma endregion Simulation Parameters


	#pragma region RunManager Definition

	// MT Mode
	G4MTRunManager* runManager = new G4MTRunManager;
	runManager->SetNumberOfThreads(threads);

	// ST Mode
	// G4RunManager* runManager = G4RunManagerFactory::CreateRunManager(G4RunManagerType::Default);

	#pragma endregion RunManager Definition


	#pragma region PhysicsList Definition & Initialization

	auto physicsList = new FTFP_BERT;
	
	auto optPhysics = new G4OpticalPhysics();

	auto optParams = G4OpticalParameters::Instance();
	// optParams->SetScintTrackSecondariesFirst(true);

	physicsList->RegisterPhysics(optPhysics);
	physicsList->SetVerboseLevel(1);
	
	# pragma endregion PhysicsList Definition & Initialization


	#pragma region DetectorConstruction Definition & Initialization

	DetectorConstruction* detectorConstruction = new DetectorConstruction(
		worldSizeXYZ, 
		scintData, 
		coatingThickness,
		siPMThickness,
		gap,
		siliconPMSDName,
		scintSDName,
		opCName,
		muCName
	);

	#pragma endregion DetectorConstruction Definition & Initialization


	#pragma region User Actions Definition

	PrimaryGeneratorActionParameters primaryGeneratorActionParameters = PrimaryGeneratorActionParameters{};
	
	RunActionParameters runActionParameters = RunActionParameters{};
	
	EventActionParameters eventActionParameters = EventActionParameters{ 
		siliconPMSDName, 
		scintSDName,
		opCName,
		muCName
	};

	#pragma endregion User Actions Definition
	

	runManager->SetUserInitialization(detectorConstruction);
	runManager->SetUserInitialization(physicsList);
	runManager->SetUserInitialization(new ActionInitialization(
		primaryGeneratorActionParameters,
		runActionParameters,
		eventActionParameters
	));

	runManager->Initialize();
	
	auto ui = new G4UIExecutive(argc, argv);
	auto UImanager = G4UImanager::GetUIpointer();
	auto visManager = new G4VisExecutive(argc, argv);
	
	if (enableVis)
	{
		visManager->Initialize();
		UImanager->ApplyCommand("/control/execute macros\\vis.mac");
		UImanager->ApplyCommand("/control/execute macros\\paint_geometry.mac");
		UImanager->ApplyCommand("/control/execute macros\\build_custom_gui.mac");

		if (enableTrackingVerbose)
		{
			UImanager->ApplyCommand("/tracking/verbose 1");
		}
		else {
			UImanager->ApplyCommand("/tracking/verbose 0");
		}
	}

	ui->SessionStart();
	
	delete ui;
	if (enableVis){ delete visManager; }
	delete runManager;

	return 0;
}
