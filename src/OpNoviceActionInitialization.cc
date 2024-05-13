
// $Id: OpNoviceActionInitialization.cc 68058 2013-03-13 14:47:43Z gcosmo $
//
/// \file OpNoviceActionInitialization.cc
/// \brief Implementation of the OpNoviceActionInitialization class

#include "OpNoviceActionInitialization.hh"
#include "OpNovicePrimaryGeneratorAction.hh"
#include "OpNoviceRunAction.hh"
#include "OpNoviceEventAction.hh"
#include "OpNoviceTrackingAction.hh"
#include "OpNoviceSteppingAction.hh"

#include "OpNoviceStackingAction.hh"
#include "OpNoviceSteppingVerbose.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

OpNoviceActionInitialization::OpNoviceActionInitialization()
	: G4VUserActionInitialization()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

OpNoviceActionInitialization::~OpNoviceActionInitialization()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void OpNoviceActionInitialization::BuildForMaster() const {
	SetUserAction(new OpNoviceRunAction());
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void OpNoviceActionInitialization::Build() const {
	
	SetUserAction(new OpNovicePrimaryGeneratorAction());
	
	  OpNoviceRunAction* RunAct = new OpNoviceRunAction;
	  SetUserAction(RunAct);
      
	  OpNoviceEventAction* EvAct =new OpNoviceEventAction(RunAct);
	  SetUserAction( EvAct );
	  //~ 
      SetUserAction(new OpNoviceStackingAction());              // 获取总的发光粒子数
      
      SetUserAction(new OpNoviceTrackingAction(EvAct));     // 获取到达PMT的光子数 被PMT探测的粒子数
      
	  SetUserAction(new OpNoviceSteppingAction( EvAct ));  // 获取能量沉积
	  
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VSteppingVerbose*
OpNoviceActionInitialization::InitializeSteppingVerbose() const {
	return new OpNoviceSteppingVerbose();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
