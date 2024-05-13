
/// \file optical/wls/src/OpNoviceTrackingAction.cc
/// \brief Implementation of the OpNoviceTrackingAction class
//
//
#include "OpNoviceTrackingAction.hh"
#include "globals.hh"
#include "G4RunManager.hh"
#include "OpNoviceEventAction.hh"

#include "G4Track.hh"
#include "G4ParticleTypes.hh"
#include "G4TrackingManager.hh"

#include "G4ParticleDefinition.hh"
#include "G4ParticleTable.hh"
#include "OpNoviceEventAction.hh"

#include "G4ProcessManager.hh"
#include "G4OpBoundaryProcess.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
OpNoviceTrackingAction::OpNoviceTrackingAction(OpNoviceEventAction *evt)
	:G4UserTrackingAction(),
	 fEventAction(evt),flag(false)
{}
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void OpNoviceTrackingAction::PreUserTrackingAction(const G4Track* aTrack) {
	flag = false;
 }

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
// this method is invoked after every track
void OpNoviceTrackingAction::PostUserTrackingAction(const G4Track* aTrack) {
	
	//------------------------------------------------------获取达到PMT的光子数目----------------------------------------------------------/
	// 获取粒子下一步所在的物理体名称
	G4VPhysicalVolume* nextPhyVolume = aTrack->GetNextVolume();
	G4String nextPhyVolumeName = "physWorld";
	if( nextPhyVolume)	nextPhyVolumeName = nextPhyVolume->GetName();
	
	if( nextPhyVolumeName == "physiPMTGlass1") {                // check if we hit the scoring volume
		flag = true;
		G4ParticleDefinition* optical_particle  =
		    G4ParticleTable::GetParticleTable()->FindParticle("opticalphoton"); // opticalphoton
		if(aTrack->GetDefinition() == optical_particle) {
			fEventAction->AddPMTCounter(0);                 // AddCounter(PMT_ID)
		 }
	 }
	else if( nextPhyVolumeName == "physiPMTGlass2") {               // check if we hit the scoring volume
		flag = true;
		G4ParticleDefinition* optical_particle  =
		    G4ParticleTable::GetParticleTable()->FindParticle("opticalphoton"); // opticalphoton
		    
		if(aTrack->GetDefinition() == optical_particle) {
			fEventAction->AddPMTCounter(1);
		 }
	 }
	
   //------------------------------------------------------------------获取PMT探测粒子数目--------------------------------------------------------------/
   if(!flag) return; // 粒子未到达PMT，返回
   
   // Retrieve the status of the photon
  G4OpBoundaryProcessStatus theStatus = Undefined;
  G4ProcessManager* OpManager =
                      G4OpticalPhoton::OpticalPhoton()->GetProcessManager();
 G4OpBoundaryProcess* fOpProcess = NULL; 
 
  if (OpManager) {
     G4int MAXofPostStepLoops = 
              OpManager->GetPostStepProcessVector()->entries();
     G4ProcessVector* fPostStepDoItVector =
              OpManager->GetPostStepProcessVector(typeDoIt);

     for ( G4int i=0; i<MAXofPostStepLoops; i++) {
         G4VProcess* fCurrentProcess = (*fPostStepDoItVector)[i];
         fOpProcess = dynamic_cast<G4OpBoundaryProcess*>(fCurrentProcess);
         if (fOpProcess) { theStatus = fOpProcess->GetStatus(); break;}
     }
  }
  
  	switch (theStatus) {
	case Detection:	  
        //~ G4cout<<"theStatus = Detection"<<G4endl;
	   if(nextPhyVolumeName == "physiPMTGlass1") fEventAction->AddDetectCounter(0);      // AddDetectCounter(PMT_ID)
	   else if(nextPhyVolumeName == "physiPMTGlass2") fEventAction->AddDetectCounter(1);
	    break;
     default: break;
   }	
}
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
