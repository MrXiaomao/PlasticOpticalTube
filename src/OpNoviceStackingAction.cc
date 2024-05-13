//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "OpNoviceStackingAction.hh"

#include "G4VProcess.hh"

#include "G4ParticleDefinition.hh"
#include "G4ParticleTypes.hh"
#include "G4Track.hh"

#include "OpNoviceEventAction.hh"
#include "G4EventManager.hh"
#include "G4Event.hh"

// #include "G4AutoLock.hh"  // 文件锁，防止访问文件异常
// namespace { G4Mutex productiontex = G4MUTEX_INITIALIZER; }

#include "G4ios.hh"
#include <fstream>
#include <stdio.h>
using namespace std;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

OpNoviceStackingAction::OpNoviceStackingAction(OpNoviceEventAction *evt)
  : G4UserStackingAction(),
    fScintillationCounter(0), fCerenkovCounter(0),
    fEvent(evt)
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

OpNoviceStackingAction::~OpNoviceStackingAction()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4ClassificationOfNewTrack
OpNoviceStackingAction::ClassifyNewTrack(const G4Track * aTrack)
{
  if(aTrack->GetDefinition() == G4OpticalPhoton::OpticalPhotonDefinition())
  { // particle is optical photon
    if(aTrack->GetParentID()>0)
    { // particle is secondary
      if(aTrack->GetCreatorProcess()->GetProcessName() == "Scintillation")
        fScintillationCounter++;
      if(aTrack->GetCreatorProcess()->GetProcessName() == "Cerenkov")
        fCerenkovCounter++;
    }
  }
  return fUrgent;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void OpNoviceStackingAction::NewStage()
{
  /*if(fScintillationCounter>0)
  {
        G4cout << "Number of Scintillation photons produced in this event : "
         << fScintillationCounter << G4endl;
         
         const G4Event* currentEvent = G4EventManager::GetEventManager()->GetConstCurrentEvent();
         G4int eventID = currentEvent->GetEventID();
         
         //protect file reading via autolock
         G4AutoLock filelock(&productiontex);
         filelock.lock();
         
        //输出总发光数到文件
        fstream datafile("opticalProduction.txt", ios::out|ios::app);   // 追加
		G4bool signal = datafile.is_open();
		if(signal) datafile<<eventID<<" "<<fScintillationCounter<<endl;
		datafile.close();
		filelock.unlock();
  }*/
  
  if(fScintillationCounter>0){
    G4cout << "Number of fScintillationCounter photons produced in this event : "
         << fScintillationCounter << G4endl;
    fEvent->AddScinYield(fScintillationCounter);
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void OpNoviceStackingAction::PrepareNewEvent()
{
  fScintillationCounter = 0;
  fCerenkovCounter = 0;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
