#pragma once

#include "G4UserTrackingAction.hh"


struct TrackingActionParameters {};

class TrackingAction : public G4UserTrackingAction
{
public:
	TrackingAction(TrackingActionParameters trackingActionParameters);
	~TrackingAction();

	void PreUserTrackingAction(const G4Track* track) override;

private:
	TrackingActionParameters _trackingActionParameters;
};