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
// $Id: OpNoviceSteppingAction.hh 69469 2013-05-05 21:42:35Z ihrivnac $
//
/// \file OpNoviceSteppingAction.hh
/// \brief Definition of the OpNoviceSteppingAction class

#ifndef OpNoviceSteppingAction_h
#define OpNoviceSteppingAction_h 1

#include "G4UserSteppingAction.hh"
#include "globals.hh"
//~ #include "OpNoviceEventAction.hh"
class G4OpBoundaryProcess;//定义了一个类
class OpNoviceEventAction;//定义了一个类
/// Stepping action class
/// 
class G4VPhysicalVolume;//dch
class G4Track;//dch
class G4Step;//dch
class OpNoviceSteppingAction : public G4UserSteppingAction//类是继承自G4内的一个类
{
  public:
    OpNoviceSteppingAction();
    OpNoviceSteppingAction(OpNoviceEventAction* eventAction);//eventAction 是OpNoviceEventAction 的指针
    virtual ~OpNoviceSteppingAction();

    // method from the base class
    virtual void UserSteppingAction(const G4Step*);

  private:
    G4int fScintillationCounter;
    G4int fCerenkovCounter;
    G4int fEventNumber;
    G4OpBoundaryProcess* fOpProcess; 
    OpNoviceEventAction* evt_action; 
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
