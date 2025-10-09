#pragma once

#include "G4VUserActionInitialization.hh"
#include "PrimaryGeneratorAction.hh"
#include "RunAction.hh"
#include "EventAction.hh"


// I defined the following structs in the relative header file to make it easier to pass parameters 
// directly from the action initialization that happens in main.cc
// struct PrimaryGeneratorActionParameter
// struct RunActionParameters
// struct EventActionParameters


class ActionInitialization : public G4VUserActionInitialization 
{
public:

    ActionInitialization(
        PrimaryGeneratorActionParameters primaryGeneratorActionParameters,
        RunActionParameters runActionParameters,
        EventActionParameters eventActionParameters
    );
    ~ActionInitialization();

	void BuildForMaster() const override;
    void Build() const override;

private:

    PrimaryGeneratorActionParameters _primaryGeneratorActionParameters;
    RunActionParameters _runActionParameters;
    EventActionParameters _eventActionParameters;

};
