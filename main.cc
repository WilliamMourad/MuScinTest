#include "G4RunManagerFactory.hh"
#include "G4RunManager.hh"
#include "G4MTRunManager.hh"

// My classes
#include "ParallelWorld.hh"
#include "DetectorConstruction.hh"
#include "PrimaryGeneratorAction.hh"
#include "RunAction.hh"
#include "EventAction.hh"
#include "TrackingAction.hh"
#include "SteppingAction.hh"
#include "ActionInitialization.hh"
#include "YAMLParser.hh"

// Physics 
#include "G4PhysListFactory.hh"
#include "FTFP_BERT.hh"
#include "G4OpticalPhysics.hh"
#include "G4OpticalParameters.hh"
#include "G4ParallelWorldPhysics.hh"

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

	G4bool enableParamsFromConfigFile = true;	// enable import of simulation parameters from config.yaml
	G4bool enableTrackingVerbose = false;		// enable verbose output from TrackingAction (for debugging ONLY)
	G4bool enableVis = true;					// enable visualization, set to false when running heavy batch jobs
	G4bool enableCuts = false;					// enable production cuts, to test different responses
	G4int threads = 20;							// number of threads to be used in MT mode, ignore in ST mode
	
	G4String siliconPMSDName = "/SiliconPM";
	G4String scintLVName = "ScintLogic";
	G4String opCName = "OpticalPhotonHitsCollection";
	G4String particleName = "mu-";

	// Forward declaration of simulation parameters
	G4double worldSizeXYZ, gap, coatingThickness, siPMThickness;
	BoxGeometry scintGeometry;
	ScintillatorProperties scintData;
	ParticleGunSettings gunSettings;
	GPSSettings gpsSettings;

	if (enableParamsFromConfigFile) {
		// Parameters are imported from an external YAML config file
		// The YAMLParser class is still very basic and lacks proper error handling (I'll add it soon), 
		// make sure to assign correct values in the config file, be very careful or it will just crash
		#pragma region Imported Simulation Parameters
		const char* configFilename = "config.yaml"; // I'll make it so that this can be passed as a command line argument later
		YAMLParser parser(configFilename);
		auto root = parser.getRoot();

		// Detector Geometry
		auto geometryNode = parser.require(root, "detector_geometry");
		
		worldSizeXYZ = parser.as_double(parser.require(geometryNode, "world_size_xyz"));
		gap = parser.as_double(parser.require(geometryNode, "gap"));

		auto componentsNode = parser.require(geometryNode, "components");
		auto scintNode = parser.require(componentsNode, "scintillator");
		auto sipmNode = parser.require(componentsNode, "sipm");
		auto coatingNode = parser.require(componentsNode, "coating");

		auto scintBoxGeomNode = parser.require(scintNode, "box_geometry");
		auto scintDataNode = parser.require(scintNode, "scint_data");


		scintGeometry = {
			parser.as_double(scintBoxGeomNode[0]),
			parser.as_double(scintBoxGeomNode[1]),
			parser.as_double(scintBoxGeomNode[2])
		};
		
		scintData = {
			scintGeometry,
			parser.as_double(parser.require(scintDataNode, "yield_factor")),
			parser.as_double(parser.require(scintDataNode, "yield")),
			parser.as_double(parser.require(scintDataNode, "wl_peak")),
			parser.as_double(parser.require(scintDataNode, "wl_left")),
			parser.as_double(parser.require(scintDataNode, "wl_right")),
			parser.as_double(parser.require(scintDataNode, "decay_time")),
			parser.as_double(parser.require(scintDataNode, "r_index")),
			parser.as_double(parser.require(scintDataNode, "abs_length"))
		};

		siPMThickness = parser.as_double(parser.require(sipmNode, "thickness"));
		coatingThickness = parser.as_double(parser.require(coatingNode, "thickness"));

		// Primary Generator
		auto primaryGenNode = parser.require(root, "primary_generator");
		auto gunNode = parser.require(primaryGenNode, "particle_gun");
		auto gpsNode = parser.require(primaryGenNode, "gps");

		gunSettings = {
			parser.as_bool(parser.require(gunNode, "is_active")),
			parser.as_int(parser.require(gunNode, "particle_number")),
			parser.as_double(parser.require(gunNode, "particle_energy")),
			{
				parser.as_double(parser.require(gunNode, "position")[0]),
				parser.as_double(parser.require(gunNode, "position")[1]),
				parser.as_double(parser.require(gunNode, "position")[2])
			},
			{
				parser.as_double(parser.require(gunNode, "direction")[0]),
				parser.as_double(parser.require(gunNode, "direction")[1]),
				parser.as_double(parser.require(gunNode, "direction")[2])
			}
		};

		

		gpsSettings = {
			parser.as_bool(parser.require(gpsNode, "is_active")),
			parser.as_int(parser.require(gpsNode, "particle_number")),
			parser.as_double(parser.require(gpsNode, "particle_mean_energy")),
			parser.as_double(parser.require(gpsNode, "particle_energy_deviation")),
			{
				parser.as_double(parser.require(gpsNode, "position")[0]),
				parser.as_double(parser.require(gpsNode, "position")[1]),
				parser.as_double(parser.require(gpsNode, "position")[2])
			},
			parser.as_double(parser.require(gpsNode, "radius")),
			{
				parser.as_double(parser.require(gpsNode, "rotation1")[0]),
				parser.as_double(parser.require(gpsNode, "rotation1")[1]),
				parser.as_double(parser.require(gpsNode, "rotation1")[2])
			},
			{
				parser.as_double(parser.require(gpsNode, "rotation2")[0]),
				parser.as_double(parser.require(gpsNode, "rotation2")[1]),
				parser.as_double(parser.require(gpsNode, "rotation2")[2])
			},
			parser.as_double(parser.require(gpsNode, "beam_aperture_x")),
			parser.as_double(parser.require(gpsNode, "beam_aperture_y"))
		};
		
		#pragma endregion Imported Simulation Parameters
	}
	else {
		// Set parameters manually without a config file
		#pragma region Hardcoded Simulation Parameters

		worldSizeXYZ = 1. * m;
	
		// This is more of an explode factor between the solid components
		// I use it mostly for visualization/debug purposes
		gap = 0 * cm; // must be set to 0 in the final version 

		// The thickness of the reflective coating has yet to be formally established
		// but for the purposes of this project any reasonable value will do
		coatingThickness = 50 * um;
		siPMThickness = 3 * mm;

		// The size of the scintillator has yet to be formally established
		// but for the purposes of this project any reasonable value will do
		scintGeometry = BoxGeometry{
			50. * mm, 
			50. * mm, 
			3 * mm 
		};

		// I set these values to match those of a BC-412 scintillator 
		// (the full datasheet can be found at https://www.robot-domestici.it/joomla/component/virtuemart/scintillatore-plastico-50--50--5-mm/getfile?file_id=5392)
		scintData = ScintillatorProperties{
			scintGeometry,
			1,				// scalingFactor: lower values reduce the scintillation yield
			8000.,			// scintYield (photon/MeV)
			2.85 * eV,		// waveLengthPeak	(434 nm)
			3.09 * eV,		// waveLengthLeft	(402 nm)
			2.41 * eV,		// waveLengthRight	(514 nm)
			3.3 * ns,		// decayTime
			1.58,			// refractiveIndex
			210. * cm,		// absorptionLength
		};

		// These values are used in the primary generator action
		
		gunSettings = ParticleGunSettings{
			false,							// isActive
			1,								// particleN
			55 * MeV,						// particleEnergy
			{0, 0, -2.5 * cm},				// gunPosition
			{0, 0, 1}						// gunDirection
		};

		gpsSettings = GPSSettings{
			true,							// isActive
			1,								// particleN
			55 * MeV,						// particleMeanEnergy
			0 * MeV,						// particleEnergySigma
			{0, 0, -2.5 * cm},				// gpsPosition
			0.6 * cm,						// gpsRadius
			{1, 0, 0},						// gpsRot1
			{0, -1, 0},						// gpsRot2
			0.01,							// beamApertureX
			0.01							// beamApertureY
		};

		#pragma endregion Hardcoded Simulation Parameters
	}


	#pragma region RunManager Definition

	// MT Mode
	G4MTRunManager* runManager = new G4MTRunManager;
	runManager->SetNumberOfThreads(threads);

	// ST Mode
	// G4RunManager* runManager = G4RunManagerFactory::CreateRunManager(G4RunManagerType::Default);

	#pragma endregion RunManager Definition


	#pragma region PhysicsList Definition & Initialization


	G4PhysListFactory factory;
	auto physicsList = factory.GetReferencePhysList("FTFP_BERT_EMZ");
	// auto physicsList = new FTFP_BERT;
	
	auto pwPhysics = new G4ParallelWorldPhysics("ParallelWorld");

	auto optPhysics = new G4OpticalPhysics();
	auto optParams = G4OpticalParameters::Instance();
	optParams->SetScintTrackSecondariesFirst(true);

	physicsList->RegisterPhysics(optPhysics);
	physicsList->RegisterPhysics(pwPhysics);
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
		scintLVName,
		opCName,
		enableCuts
	);

	ParallelWorld* parallelWorld = new ParallelWorld(
		"ParallelWorld",
		scintGeometry.sizeZ,
		siPMThickness,
		gap
	);

	detectorConstruction->RegisterParallelWorld(parallelWorld);

	#pragma endregion DetectorConstruction Definition & Initialization


	#pragma region User Actions Definition

	PrimaryGeneratorActionParameters primaryGeneratorActionParameters = PrimaryGeneratorActionParameters{
		particleName,
		gunSettings,
		gpsSettings
	};
	
	RunActionParameters runActionParameters = RunActionParameters{
		enableCuts
	};
	
	EventActionParameters eventActionParameters = EventActionParameters{ 
		scintLVName,
		siliconPMSDName, 
		opCName,
	};

	TrackingActionParameters trackingActionParameters = TrackingActionParameters{};

	SteppingActionParameters steppingActionParameters = SteppingActionParameters{
		scintLVName
	};

	#pragma endregion User Actions Definition
	

	runManager->SetUserInitialization(detectorConstruction);
	runManager->SetUserInitialization(physicsList);
	runManager->SetUserInitialization(new ActionInitialization(
		primaryGeneratorActionParameters,
		runActionParameters,
		eventActionParameters,
		trackingActionParameters,
		steppingActionParameters
	));

	runManager->Initialize();
	
	auto ui = new G4UIExecutive(argc, argv);
	auto UImanager = G4UImanager::GetUIpointer();
	auto visManager = new G4VisExecutive(argc, argv);
	

	UImanager->ApplyCommand("/control/execute macros\\init.mac");

	UImanager->ApplyCommand("/control/execute macros\\build_custom_gui.mac");
	if (enableVis)
	{
		visManager->Initialize();
		UImanager->ApplyCommand("/control/execute macros\\vis.mac");
		UImanager->ApplyCommand("/control/execute macros\\paint_geometry.mac");
	}

	if (enableTrackingVerbose)
	{
		UImanager->ApplyCommand("/tracking/verbose 1");
	}
	else {
		UImanager->ApplyCommand("/tracking/verbose 0");
	}

	ui->SessionStart();
	
	delete ui;
	if (enableVis){ delete visManager; }
	delete runManager;

	return 0;
}
