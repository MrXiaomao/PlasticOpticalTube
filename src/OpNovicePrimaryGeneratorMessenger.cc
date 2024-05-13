
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "OpNovicePrimaryGeneratorMessenger.hh"

#include "OpNovicePrimaryGeneratorAction.hh"
#include "G4UIdirectory.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4SystemOfUnits.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

OpNovicePrimaryGeneratorMessenger::
  OpNovicePrimaryGeneratorMessenger(OpNovicePrimaryGeneratorAction* OpNoviceGun)
  : G4UImessenger(),
    fOpNoviceAction(OpNoviceGun)
{
  fGunDir = new G4UIdirectory("/OpNovice/gun/");
  fGunDir->SetGuidance("PrimaryGenerator control");

  fPolarCmd =
           new G4UIcmdWithADoubleAndUnit("/OpNovice/gun/optPhotonPolar",this);
  fPolarCmd->SetGuidance("Set linear polarization");
  fPolarCmd->SetGuidance("  angle w.r.t. (k,n) plane");
  fPolarCmd->SetParameterName("angle",true);
  fPolarCmd->SetUnitCategory("Angle");
  fPolarCmd->SetDefaultValue(-360.0);
  fPolarCmd->SetDefaultUnit("deg");
  fPolarCmd->AvailableForStates(G4State_Idle);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

OpNovicePrimaryGeneratorMessenger::~OpNovicePrimaryGeneratorMessenger()
{
  delete fPolarCmd;
  delete fGunDir;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
/*
void OpNovicePrimaryGeneratorMessenger::SetNewValue(
                                        G4UIcommand* command, G4String newValue)
{
  if( command == fPolarCmd ) {
    G4double angle = fPolarCmd->GetNewDoubleValue(newValue);
    if ( angle == -360.0*deg ) {
      fOpNoviceAction->SetOptPhotonPolar();
    } 
    else {
      fOpNoviceAction->SetOptPhotonPolar(angle);
    }
  }
}*/

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
