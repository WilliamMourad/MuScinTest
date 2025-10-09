#pragma once

#include "G4UserEventAction.hh"
#include "G4Event.hh"
#include "G4THitsMap.hh"
#include "G4SDManager.hh"
#include "G4AnalysisManager.hh"


struct EventActionParameters {
	G4String siliconPMSDName;
	G4String scintSDName;
	G4String opCName;
	G4String muCName;
};

class EventAction : public G4UserEventAction {
public:
	EventAction(EventActionParameters eventActionParameters);
	~EventAction();

	void BeginOfEventAction(const G4Event* event) override;
	void EndOfEventAction(const G4Event* event) override;

private:

	static G4double SumOverHC(const G4THitsMap<G4double>* hm);
	
	EventActionParameters _eventActionParameters;
	G4AnalysisManager* analysisManager;

	// Cache hit collections IDs to improve performances
	G4int siliconPM_op_HCID			= -1;
	G4int scint_edep_HCID			= -1;
	G4int scint_muPathLength_HCID	= -1;
	G4int coating_edep_HCID			= -1;

};
