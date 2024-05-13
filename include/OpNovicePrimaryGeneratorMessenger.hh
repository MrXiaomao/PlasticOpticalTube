
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#ifndef OpNovicePrimaryGeneratorMessenger_h
#define OpNovicePrimaryGeneratorMessenger_h 1

#include "G4UImessenger.hh"
#include "globals.hh"

class OpNovicePrimaryGeneratorAction;
class G4UIdirectory;
class G4UIcmdWithADoubleAndUnit;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class OpNovicePrimaryGeneratorMessenger: public G4UImessenger
{
  public:
    OpNovicePrimaryGeneratorMessenger(OpNovicePrimaryGeneratorAction* );
    virtual ~OpNovicePrimaryGeneratorMessenger();
 
    // virtual void SetNewValue(G4UIcommand*, G4String);
 
  private:
    OpNovicePrimaryGeneratorAction* fOpNoviceAction;
    G4UIdirectory*                  fGunDir;
    G4UIcmdWithADoubleAndUnit*      fPolarCmd;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
