#include "TrackingAction.hh"
#include "G4Track.hh"
#include "G4OpticalPhoton.hh"

#include "OpticalPhotonTrackInfo.hh"

TrackingAction::TrackingAction(TrackingActionParameters trackingActionParameters) 
{
	_trackingActionParameters = trackingActionParameters;
};

TrackingAction::~TrackingAction() {};

void TrackingAction::PreUserTrackingAction(const G4Track* track) 
{
	// I want to only track optical photons to sample the number of reflections they undergo
	if (track->GetDefinition() != G4OpticalPhoton::Definition()) return;
	

	if (!track->GetUserInformation())
	{
		const_cast<G4Track*>(track)->SetUserInformation(new OpticalPhotonTrackInfo());
	}
};