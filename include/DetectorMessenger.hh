
#ifndef DetectorMessenger_h
#define DetectorMessenger_h 1

#include "G4UImessenger.hh"
#include "globals.hh"

class OpNoviceDetectorConstruction;
class G4UIdirectory;
class G4UIcommand;
// class G4UIcmdWithAString;
class G4UIcmdWithADoubleAndUnit;
// class G4UIcmdWithoutParameter;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class DetectorMessenger: public G4UImessenger
{
  public:
  
    DetectorMessenger(OpNoviceDetectorConstruction* );
   ~DetectorMessenger() override;
    
    void SetNewValue(G4UIcommand*, G4String) override;
    
  private:
  
    OpNoviceDetectorConstruction* fDetector = nullptr;
    G4UIdirectory*             fDetDir  = nullptr;
    G4UIcmdWithADoubleAndUnit* fDetLengthCmd = nullptr;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
