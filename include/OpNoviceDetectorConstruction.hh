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
// $Id: B1DetectorConstruction.hh 69565 2013-05-08 12:35:31Z gcosmo $
//
/// \file B1DetectorConstruction.hh
/// \brief Definition of the B1DetectorConstruction class

#ifndef B1DetectorConstruction_h
#define B1DetectorConstruction_h 1

#include "G4VUserDetectorConstruction.hh"
#include "globals.hh"

class DetectorMessenger;
class G4VPhysicalVolume;
class G4LogicalVolume;
extern G4double gPMTCathodePosZ ;
/// Detector construction class to define materials and geometry.

class OpNoviceDetectorConstruction : public G4VUserDetectorConstruction
{
  public:
    OpNoviceDetectorConstruction();
    virtual ~OpNoviceDetectorConstruction();

    virtual G4VPhysicalVolume* Construct();
    G4LogicalVolume* GetScoringVolume() const { return fScoringVolume; }
    G4double GetOpticalDistance() const { return  fiberGap;}
    
    //设置探测器水体长度（注意光纤比水体更长）
    void SetDetectorLength(G4double);
    inline G4double GetDetectorLength() const {return fwaterShell_length;}

  protected:
    G4String  GetphyOpticalShellName(G4int i, G4int j); 
    G4String  GetphyOpticaltubeName(G4int i, G4int j); 
    G4String  GetSurfaceName(G4String phycialNameA, G4String phycialNameB);
    G4LogicalVolume*  fScoringVolume;
    G4double fiberGap; 
    G4double fwaterShell_length; 
    DetectorMessenger* fDetectorMessenger = nullptr;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif

