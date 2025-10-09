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
	analysisManager->CreateH1("ScintOpticalPhotonsEnergy", "Scint Optical Photons Energy (eV)", 100, 1, 10);
	analysisManager->CreateH1("CerOpticalPhotonsEnergy", "Cer Optical Photons Energy (eV)", 100, 1, 10);
	analysisManager->CreateH1("ScintOpticalPhotonsTime", "Scint Optical Photons Time (ns)", 1000, 0, 100);
	analysisManager->CreateH1("CerOpticalPhotonsTime", "Cer Optical Photons Time (ns)", 1000, 0, 100);

	analysisManager->CreateH2("ScintOpticalPhotonsSpread", "Scint Optical Photons Spread; X (mm); Y (mm)", 1000, -40, 40, 1000, -40, 40);
	analysisManager->CreateH2("CerOpticalPhotonsSpread", "Cer Optical Photons Spread; X (mm); Y (mm)", 1000, -40, 40, 1000, -40, 40);

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