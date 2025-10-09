#include "SteppingAction.hh"

#include "G4Step.hh"
#include "G4OpticalPhoton.hh"
#include "G4OpBoundaryProcess.hh"
#include "G4ProcessManager.hh"

#include "OpticalPhotonTrackInfo.hh"

SteppingAction::SteppingAction(SteppingActionParameters steppingActionParameters) 
{
	_steppingActionParameters = steppingActionParameters;
};

SteppingAction::~SteppingAction() {};

void SteppingAction::UserSteppingAction(const G4Step* step) 
{
	const auto* track = step->GetTrack();
	auto preStep = step->GetPreStepPoint();

	// Filter out non optical photons 
	// and for optical photons filter out steps that are not at a boundary
	if (track->GetDefinition() != G4OpticalPhoton::Definition()) return;
	if (preStep->GetStepStatus() != fGeomBoundary) return;
	
	static thread_local G4OpBoundaryProcess* boundary = nullptr;

	if (!boundary)
	{
		auto* processManager = track->GetDefinition()->GetProcessManager();

		auto* processVector = processManager->GetPostStepProcessVector(typeDoIt);

		for (size_t i = 0; i < processVector->size(); i++)
		{
			if (auto* b = dynamic_cast<G4OpBoundaryProcess*>((*processVector)[i]))
			{
				boundary = b;
				break;
			}
		}
		// This shouldnt happen but just in case...
		if (!boundary) return;
	}

	const auto status = boundary->GetStatus();

	// To determine the enumerator i just checked the source code of Geant4
	const bool isReflection = 
		(status == G4OpBoundaryProcessStatus::TotalInternalReflection) ||
		(status == G4OpBoundaryProcessStatus::FresnelReflection) ||
		(status == G4OpBoundaryProcessStatus::LobeReflection) ||
		(status == G4OpBoundaryProcessStatus::SpikeReflection) ||
		(status == G4OpBoundaryProcessStatus::LambertianReflection) ||
		(status == G4OpBoundaryProcessStatus::BackScattering);

	if (!isReflection) return;
	
	auto trackInfo = static_cast<OpticalPhotonTrackInfo*>(track->GetUserInformation());
	
	if (!trackInfo) return;

	trackInfo->nReflections++;
	// Some logic can be added to check the volumes and determine if the reflection happened at a coating
	// but since the simulation setup is trivial (the result is the same), for now i'll just skip it
	trackInfo->nReflectionsAtCoating++;

};