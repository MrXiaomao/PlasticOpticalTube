
#include "DetectorMessenger.hh"

#include "OpNoviceDetectorConstruction.hh"
#include "G4UIdirectory.hh"
// #include "G4UIcommand.hh"
// #include "G4UIparameter.hh"
// #include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
// #include "G4UIcmdWithoutParameter.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

DetectorMessenger::DetectorMessenger(OpNoviceDetectorConstruction* det)
: fDetector(det)
{
  fDetDir = new G4UIdirectory("/Detector/");
  fDetDir->SetGuidance("commands specific to optical tube");

  fDetLengthCmd = new G4UIcmdWithADoubleAndUnit("/Detector/length",this);
  fDetLengthCmd->SetGuidance("Set length of the Detector, for water Tube");
  fDetLengthCmd->SetGuidance("Unit of the time can be :");
  fDetLengthCmd->SetGuidance("mm, cm, cm (default)");
  fDetLengthCmd->SetParameterName("length",false);
  fDetLengthCmd->SetRange("length>0.");
  fDetLengthCmd->SetUnitCategory("Length");
  fDetLengthCmd->SetDefaultUnit("cm");
  fDetLengthCmd->AvailableForStates(G4State_PreInit, G4State_Idle);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

DetectorMessenger::~DetectorMessenger()
{
  delete fDetLengthCmd;
  delete fDetDir;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorMessenger::SetNewValue(G4UIcommand* command,G4String newValue)
{ 
  if( command == fDetLengthCmd )
  { 
    G4double length;
    G4String unt;
    std::istringstream is(newValue);
    is >> length >> unt;
    length *= G4UIcommand::ValueOf(unt);
    fDetector->SetDetectorLength(length);
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
