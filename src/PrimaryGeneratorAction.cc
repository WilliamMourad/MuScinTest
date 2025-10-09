#include "PrimaryGeneratorAction.hh"

#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4SystemOfUnits.hh"
#include "G4GeneralParticleSource.hh"


PrimaryGeneratorAction::PrimaryGeneratorAction(PrimaryGeneratorActionParameters primaryGeneratorActionParameters) {
    
	_primaryGeneratorActionParameters = primaryGeneratorActionParameters;

    int n_particle = 1;

    particleGun = new G4ParticleGun(n_particle);

	G4ParticleDefinition* particle = G4ParticleTable::GetParticleTable()->FindParticle("mu-");
    particleGun->SetParticleDefinition(particle);
    particleGun->SetParticlePosition(G4ThreeVector(0., 0., -25 * cm));
    particleGun->SetParticleMomentumDirection(G4ThreeVector(0., 0., 1.));
    particleGun->SetParticleEnergy(20 * MeV);
}

PrimaryGeneratorAction::~PrimaryGeneratorAction() {
    delete particleGun;
}

void PrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent) {
    particleGun->GeneratePrimaryVertex(anEvent);
}

