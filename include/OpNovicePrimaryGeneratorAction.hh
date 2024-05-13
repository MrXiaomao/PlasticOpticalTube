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
/// \file OpNovice/include/OpNovicePrimaryGeneratorAction.hh
/// \brief Definition of the OpNovicePrimaryGeneratorAction class
//
//
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#ifndef OpNovicePrimaryGeneratorAction_h
#define OpNovicePrimaryGeneratorAction_h 1

#include "G4VUserPrimaryGeneratorAction.hh"
#include "globals.hh"

class G4ParticleGun;
class G4Event;
class OpNovicePrimaryGeneratorMessenger;
class G4Tubs;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class OpNovicePrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
{
  public:
    OpNovicePrimaryGeneratorAction();
    virtual ~OpNovicePrimaryGeneratorAction();

  public:
    virtual void GeneratePrimaries(G4Event*);

    void SetOptPhotonPolar();
    void SetOptPhotonPolar(G4double);
    void GetEnergySpectrum(const char *filename);
    G4int CountLines(const char *filename);

  private:
    G4ParticleGun* fParticleGun;
    OpNovicePrimaryGeneratorMessenger* fGunMessenger;
    
    void GetGeometrySize();                                                // 注意：GetGeometrySize()函数只能在构造函数完成之后进行，
    G4int  GetVolumeID(); // 抽样确定电子的发射位置所在光纤编号
    
    G4bool firstFlag;
    G4Tubs* fWaterShellTubs;
    G4Tubs* fOpticalTubs; 
    G4double waterRadius;
    G4double waterLength;
    G4double opticalRadius;
    G4double opticalDistance;        // 相邻两个光纤之间的间距
    static const int EnergyNum = 100;
    G4double EnergySpectrum[EnergyNum][2];  // 最大可容纳100个能量点，如更多，请进一步改动，但请注意
    static const G4int c_colums = 17;   
    static  G4int columNum[17];     // 存放每一列中对应的光纤数目，从左往右数起
    G4int columDistribution[17]; // 对colum[]数组累积求和，相当于分布函数，方便快速定位列号。
    G4int   SumFiber;            // 总光纤根数
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif /*OpNovicePrimaryGeneratorAction_h*/
