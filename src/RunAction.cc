#include "RunAction.hh"


RunAction::RunAction(RunActionParameters runActionParameters)
{
	_runActionParameters = runActionParameters;

	timer = new G4Timer();

	analysisManager = G4AnalysisManager::Instance();
	analysisManager->Reset();
	analysisManager->SetVerboseLevel(1);
	analysisManager->SetNtupleMerging(true);

	// Create Ntuples and histograms here using analysisManager
	// I need to figure out how to parametrize this better, probably passing a struct to the RunAction constructor
	analysisManager->CreateH1("ScintOpticalPhotonsEnergy", "Scint Optical Photons Energy (eV)", 1000, 2.4, 3.1);
	analysisManager->CreateH1("CerOpticalPhotonsEnergy", "Cer Optical Photons Energy (eV)", 100, 1, 10);			// not fundamental, can be removed to save space
	analysisManager->CreateH1("ScintOpticalPhotonsTime", "Scint Optical Photons Time (ns)", 1000, 0, 30);
	analysisManager->CreateH1("CerOpticalPhotonsTime", "Cer Optical Photons Time (ns)", 10, 0, 1);					// not fundamental, can be removed to save space

	analysisManager->CreateH2("ScintOpticalPhotonsSpread", "Scint Optical Photons Spread; X (mm); Y (mm)", 100, -40, 40, 100, -40, 40);	// not fundamental, can be removed to save space
	analysisManager->CreateH2("CerOpticalPhotonsSpread", "Cer Optical Photons Spread; X (mm); Y (mm)", 1000, -40, 40, 1000, -40, 40);		// not fundamental, can be removed to save space

	analysisManager->CreateH1("OpticalPhotonsReflections", "Optical Photons Reflections", 10, 0, 10);

	analysisManager->CreateNtuple("PerEventCollectedData", "Per-Event Collected Data");
	analysisManager->CreateNtupleDColumn("EventID");
	analysisManager->CreateNtupleDColumn("ScintOPsCollected");
	analysisManager->CreateNtupleDColumn("CerOPsCollected");
	analysisManager->CreateNtupleDColumn("ScintTotalEdep");
	analysisManager->CreateNtupleDColumn("SiPMTotalEdep");
	analysisManager->CreateNtupleDColumn("CoatingTotalEdep");
	analysisManager->CreateNtupleDColumn("MuPathLength");
	analysisManager->FinishNtuple();
}

RunAction::~RunAction()
{
	delete timer;
}

void RunAction::BeginOfRunAction(const G4Run* run)
{
	timer->Start();

	analysisManager->OpenFile("output.root");
	
	// Reset ntuple
	// analysisManager->Reset();
}

void RunAction::EndOfRunAction(const G4Run* run)
{
	analysisManager->Write();
	analysisManager->CloseFile(false);

	timer->Stop();
}