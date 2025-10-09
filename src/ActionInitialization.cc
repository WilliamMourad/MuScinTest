#include "ActionInitialization.hh"



ActionInitialization::ActionInitialization(
	PrimaryGeneratorActionParameters primaryGeneratorActionParameters,
	RunActionParameters runActionParameters,
	EventActionParameters eventActionParameters
) : G4VUserActionInitialization() 
{
	_primaryGeneratorActionParameters = primaryGeneratorActionParameters;
	_runActionParameters = runActionParameters;
	_eventActionParameters = eventActionParameters;
}

ActionInitialization::~ActionInitialization() {}

// This method is called only in the master thread in multi-threaded mode,
// omitting this brings issues with data collection.
void ActionInitialization::BuildForMaster() const 
{
	SetUserAction(new RunAction(_runActionParameters));  
}

void ActionInitialization::Build() const 
{
    SetUserAction(new PrimaryGeneratorAction(_primaryGeneratorActionParameters));
    SetUserAction(new RunAction(_runActionParameters));
	SetUserAction(new EventAction(_eventActionParameters));
}
