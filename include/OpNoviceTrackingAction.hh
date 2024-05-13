
/// \file optical/wls/include/OpNoviceTrackingAction.hh
/// \brief Definition of the OpNoviceTrackingAction class
//

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

#ifndef OpNoviceTrackingAction_h
#define OpNoviceTrackingAction_h 1

#include "G4UserTrackingAction.hh"
class OpNoviceEventAction;

class OpNoviceTrackingAction : public G4UserTrackingAction {

public:
	OpNoviceTrackingAction(OpNoviceEventAction *evt);
	virtual ~OpNoviceTrackingAction() { };
	virtual void PreUserTrackingAction(const G4Track*);
	virtual void PostUserTrackingAction(const G4Track* aTrack);
    
private:
	OpNoviceEventAction* fEventAction;
	bool flag;   // 若光子到达了PMT，则flag变为true.
};

#endif
