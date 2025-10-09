#pragma once

#include "G4UserSteppingAction.hh"

struct SteppingActionParameters {};

class SteppingAction : public G4UserSteppingAction
{
public:
	SteppingAction(SteppingActionParameters steppingActionParameters);
	~SteppingAction();
	void UserSteppingAction(const G4Step* step) override;

private:
	SteppingActionParameters _steppingActionParameters;
};