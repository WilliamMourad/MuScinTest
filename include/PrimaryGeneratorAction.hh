#pragma once

#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4ParticleGun.hh"
#include "G4ThreeVector.hh"
#include "globals.hh"

struct PrimaryGeneratorActionParameters {};

class PrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction {
public:
    PrimaryGeneratorAction(PrimaryGeneratorActionParameters primaryGeneratorActionParameters);
    ~PrimaryGeneratorAction();

    void GeneratePrimaries(G4Event* anEvent);

private:
	PrimaryGeneratorActionParameters _primaryGeneratorActionParameters;
    G4ParticleGun* particleGun;
};
