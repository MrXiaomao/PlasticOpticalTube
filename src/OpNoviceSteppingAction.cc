
/// \file OpNoviceSteppingAction.cc
/// \brief Implementation of the OpNoviceSteppingAction class

#include "OpNoviceSteppingAction.hh"
#include "OpNoviceDetectorConstruction.hh"
#include "G4Step.hh"
#include "G4Track.hh"
#include "G4OpticalPhoton.hh"

#include "G4Event.hh"
#include "G4RunManager.hh"

#include "G4StepPoint.hh"
#include "G4ThreeVector.hh"
#include "G4SystemOfUnits.hh"
#include "G4VPhysicalVolume.hh"

#include "G4ProcessManager.hh"
#include "G4OpBoundaryProcess.hh"
#include "OpNoviceEventAction.hh"

#include <math.h>
#include <stdio.h>
using namespace std; 
extern G4double gPMTCathodePosZ ;
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

OpNoviceSteppingAction::OpNoviceSteppingAction()
: G4UserSteppingAction(),fScintillationCounter(0),
fCerenkovCounter(0), fEventNumber(-1),
fOpProcess(NULL), evt_action(NULL)
{ }

OpNoviceSteppingAction::OpNoviceSteppingAction(OpNoviceEventAction* eventAction)
: G4UserSteppingAction(), fScintillationCounter(0),
fCerenkovCounter(0), fEventNumber(-1),
fOpProcess(NULL), evt_action(eventAction)
{ }
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

OpNoviceSteppingAction::~OpNoviceSteppingAction()
{ ; }

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void OpNoviceSteppingAction::UserSteppingAction(const G4Step* step)
{
	///////////////////////////////////////获取闪烁体中的沉积能量///////////////////////////////////////////////////
   //Get the particlename
  G4Track* fTrack = step->GetTrack();
  G4String particleName = fTrack->GetDefinition()->GetParticleName();
  
  // 防止粒子进入死循环
  if(fTrack->GetCurrentStepNumber()>10000){
    fTrack->SetTrackStatus(fStopAndKill); // 杀死粒子
    return;
  }

  G4LogicalVolume* Logicvolume = 
    step->GetPreStepPoint()->GetTouchableHandle()->GetVolume()->GetLogicalVolume();
  if(Logicvolume!=nullptr) {
    G4String VolumeName = Logicvolume->GetName();
    if(particleName == "e-") 
    {
      if(VolumeName == "logicOpticaltube")
      {
        // collect energy deposited in this step
        G4double edepStep = step->GetTotalEnergyDeposit();
        evt_action->AddDepositedEnergy(edepStep);
      }
    }
    if((particleName == "opticalphoton") && (VolumeName == "logicWaterShell")){
      evt_action->AddTrackIDThroughWater(fTrack->GetTrackID());
    }
  }
}
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
