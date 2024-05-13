/// \file OpNovice/src/OpNoviceRunAction.cc
/// \brief Implementation of the OpNoviceRunAction class
//
//
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

// Make this appear first!
#include "G4Timer.hh"

#include "OpNoviceRunAction.hh"
#include "OpNoviceRunThread.hh"
#include "G4Run.hh"

#include <stdio.h>
#include "G4SystemOfUnits.hh"

using namespace std;
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

OpNoviceRunAction::OpNoviceRunAction()
	: G4UserRunAction(),
	  fTimer(0),fRun(0)
{
	fTimer = new G4Timer;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

OpNoviceRunAction::~OpNoviceRunAction() {
	delete fTimer;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4Run* OpNoviceRunAction::GenerateRun() {
	fRun = new OpNoviceRunThread();
	return fRun;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void OpNoviceRunAction::BeginOfRunAction(const G4Run*) {
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void OpNoviceRunAction::EndOfRunAction(const G4Run*) {
	fTimer->Stop();
	fRun->EndOfRun();
    
	if (IsMaster()) {
		G4cout<< G4endl<< "--------------------End of Global Run-----------------------"<<G4endl;
		G4cout<<*fTimer<<G4endl;
	} 
	else {
		G4cout<< G4endl << "--------------------End of Local Run------------------------"<<G4endl;
		G4cout<<*fTimer<<G4endl;
	}
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
