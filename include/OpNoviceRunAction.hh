//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#ifndef OpNoviceRunAction_h
#define OpNoviceRunAction_h 1

#include "globals.hh"
#include "G4UserRunAction.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class G4Timer;
class G4Run;
class OpNoviceRunThread;

class OpNoviceRunAction : public G4UserRunAction
{
  public:
    OpNoviceRunAction();
    virtual ~OpNoviceRunAction();
    virtual G4Run* GenerateRun();

  public:
    virtual void BeginOfRunAction(const G4Run*);
    virtual void EndOfRunAction(const G4Run*);

  private:
    G4Timer* fTimer;
	  OpNoviceRunThread* fRun;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif /*OpNoviceRunAction_h*/
