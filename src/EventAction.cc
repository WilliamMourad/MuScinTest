#include "EventAction.hh"

#include "G4HCofThisEvent.hh"
#include "G4SystemOfUnits.hh"

#include "OpticalPhotonHit.hh"


EventAction::EventAction(EventActionParameters eventActionParameters) 
{
	_eventActionParameters = eventActionParameters;
	analysisManager = G4AnalysisManager::Instance();
}

EventAction::~EventAction() {}

void EventAction::BeginOfEventAction(const G4Event* event)
{
	
}

void EventAction::EndOfEventAction(const G4Event* event) 
{
	
	G4String siliconPMSDName = _eventActionParameters.siliconPMSDName;
	G4String scintSDName = _eventActionParameters.scintSDName;

	G4String opCName = _eventActionParameters.opCName;
	G4String muCName = _eventActionParameters.muCName;

	G4HCofThisEvent* hce = event->GetHCofThisEvent();
	if (!hce) return;

	auto SDManager = G4SDManager::GetSDMpointer();

	G4int siliconPM_op_HCID = SDManager->GetCollectionID(opCName);
	// G4int scintSD_HCID = sdManager->GetCollectionID(scintSDName + "/HitsCollection");
	
	auto siliconPMSD_HC = hce->GetHC(siliconPM_op_HCID);
	// auto scintSD_HC = hce->GetHC(scintSD_HCID);
	

	// From here on, i'll just fill the root structures with the data,
	// you shall expect to see lots of ugly loops, I know, but I'll leave optimization for another time...
	if (siliconPMSD_HC) {

		G4int nHits = siliconPMSD_HC->GetSize();


		for (G4int i = 0; i < nHits; i++) {
			auto hit = static_cast<OpticalPhotonHit*>(siliconPMSD_HC->GetHit(i));
			auto process = hit->GetProcess();
			auto edep = hit->GetEdep();
			auto time = hit->GetTime();
			auto position = hit->GetPosition();
			auto nReflections = hit->GetNReflections();
			auto nReflectionsAtCoating = hit->GetNReflectionsAtCoating();

			// dont forget to remove the g4 units
			if (process == "Scintillation") {
				analysisManager->FillH1(0, edep / eV); // Scint OP Energy 
				analysisManager->FillH1(2, time / ns); // Scint OP Time
				analysisManager->FillH2(0, position.x() / mm, position.y() / mm); // Scint OP Spread
			} else if (process == "Cerenkov") {
				analysisManager->FillH1(1, edep / eV); // Cer OP Energy
				analysisManager->FillH1(3, time / ns); // Cer OP Time
				analysisManager->FillH2(1, position.x() / mm, position.y() / mm); // Scint OP Spread
			}

			analysisManager->FillH1(4, nReflectionsAtCoating); // OP Reflections
		}
	} 

	/*if (scintSD_HC) {

		G4cout << "Scintillator Hits Collection: " << scintSD_HC->GetName() << G4endl;
		G4cout << "Number of hits: " << scintSD_HC->GetSize() << G4endl;
	} else {
		G4cout << "No Scintillator Hits Collection found." << G4endl;
	}*/
	
}

