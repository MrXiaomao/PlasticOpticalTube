//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
/// \file OpNovice/src/OpNoviceStackingAction.cc
/// \brief Implementation of the OpNoviceStackingAction class
//
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "OpNoviceStackingAction.hh"

#include "G4VProcess.hh"

#include "G4ParticleDefinition.hh"
#include "G4ParticleTypes.hh"
#include "G4Track.hh"

#include "G4EventManager.hh"
#include "G4Event.hh"

#include "G4AutoLock.hh"  // 文件锁，防止访问文件异常
namespace { G4Mutex productiontex = G4MUTEX_INITIALIZER; }

#include "G4ios.hh"
#include <fstream>
#include <stdio.h>
using namespace std;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

OpNoviceStackingAction::OpNoviceStackingAction()
  : G4UserStackingAction(),
    fScintillationCounter(0), fCerenkovCounter(0)
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
  if(fScintillationCounter>0)
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
  }
  
  if(fScintillationCounter>0)
  G4cout << "Number of fScintillationCounter photons produced in this event : "
         << fScintillationCounter << G4endl;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void OpNoviceStackingAction::PrepareNewEvent()
{
  fScintillationCounter = 0;
  fCerenkovCounter = 0;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
