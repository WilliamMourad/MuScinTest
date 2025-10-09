#pragma once

#include "G4UserEventAction.hh"
#include "G4Event.hh"

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
	EventActionParameters _eventActionParameters;
	G4AnalysisManager* analysisManager;

};

