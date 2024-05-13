//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#ifndef OpNoviceStackingAction_H
#define OpNoviceStackingAction_H 1

#include "globals.hh"
#include "G4UserStackingAction.hh"
class OpNoviceEventAction;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class OpNoviceStackingAction : public G4UserStackingAction
{
  public:
    OpNoviceStackingAction(OpNoviceEventAction *evt);
    virtual ~OpNoviceStackingAction();

  public:
    virtual G4ClassificationOfNewTrack ClassifyNewTrack(const G4Track* aTrack);
    virtual void NewStage();
    virtual void PrepareNewEvent();

  private:
    G4int fScintillationCounter;
    G4int fCerenkovCounter;
    OpNoviceEventAction* fEvent;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
