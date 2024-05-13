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
/// \file OpNovice/src/OpNovicePhysicsList.cc
/// \brief Implementation of the OpNovicePhysicsList class
//
//
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "globals.hh"
#include "OpNovicePhysicsList.hh"
#include "OpNovicePhysicsListMessenger.hh"

#include "G4ParticleDefinition.hh"
#include "G4ParticleTypes.hh"
#include "G4ParticleTable.hh"

#include "G4BosonConstructor.hh"
#include "G4LeptonConstructor.hh"
#include "G4MesonConstructor.hh"
#include "G4BaryonConstructor.hh"
#include "G4IonConstructor.hh"
#include "G4ShortLivedConstructor.hh"

#include "G4ProcessManager.hh"

#include "G4Cerenkov.hh"
#include "G4Scintillation.hh"
#include "G4OpAbsorption.hh"
#include "G4OpRayleigh.hh"
#include "G4OpMieHG.hh"
#include "G4OpBoundaryProcess.hh"

#include "G4LossTableManager.hh"
#include "G4EmSaturation.hh"

#include "G4EmLivermorePhysics.hh" 
#include "G4VPhysicsConstructor.hh"

G4ThreadLocal G4int OpNovicePhysicsList::fVerboseLevel = 0;  // 控制输出物理过程信息
G4ThreadLocal G4int OpNovicePhysicsList::fMaxNumPhotonStep = 20;
G4ThreadLocal G4Cerenkov* OpNovicePhysicsList::fCerenkovProcess = 0;
G4ThreadLocal G4Scintillation* OpNovicePhysicsList::fScintillationProcess = 0;
G4ThreadLocal G4OpAbsorption* OpNovicePhysicsList::fAbsorptionProcess = 0;
G4ThreadLocal G4OpRayleigh* OpNovicePhysicsList::fRayleighScatteringProcess = 0;
G4ThreadLocal G4OpMieHG* OpNovicePhysicsList::fMieHGScatteringProcess = 0;
G4ThreadLocal G4OpBoundaryProcess* OpNovicePhysicsList::fBoundaryProcess = 0;
 
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

OpNovicePhysicsList::OpNovicePhysicsList() 
 : G4VUserPhysicsList()
{
  emPhysicsList = new G4EmLivermorePhysics();
  fMessenger = new OpNovicePhysicsListMessenger(this);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

OpNovicePhysicsList::~OpNovicePhysicsList() 
{ 
	delete fMessenger; 
	delete emPhysicsList;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void OpNovicePhysicsList::ConstructParticle()
{
  // In this method, static member functions should be called
  // for all particles which you want to use.
  // This ensures that objects of these particle types will be
  // created in the program.

  G4BosonConstructor bConstructor;
  bConstructor.ConstructParticle();

  G4LeptonConstructor lConstructor;
  lConstructor.ConstructParticle();

  G4MesonConstructor mConstructor;
  mConstructor.ConstructParticle();

  G4BaryonConstructor rConstructor;
  rConstructor.ConstructParticle();

  G4IonConstructor iConstructor;
  iConstructor.ConstructParticle(); 
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void OpNovicePhysicsList::ConstructProcess()
{
  AddTransportation();
  ConstructDecay();
  //~ ConstructEM();
  emPhysicsList -> ConstructProcess();
  ConstructOp();
  //~ ConstructNeutron();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "G4Decay.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void OpNovicePhysicsList::ConstructDecay()
{
  // Add Decay Process
  G4Decay* theDecayProcess = new G4Decay();
  auto particleIterator=GetParticleIterator();
  particleIterator->reset();
  while( (*particleIterator)() ){
    G4ParticleDefinition* particle = particleIterator->value();
    G4ProcessManager* pmanager = particle->GetProcessManager();
    if (theDecayProcess->IsApplicable(*particle)) {
      pmanager ->AddProcess(theDecayProcess);
      // set ordering for PostStepDoIt and AtRestDoIt
      pmanager ->SetProcessOrdering(theDecayProcess, idxPostStep);
      pmanager ->SetProcessOrdering(theDecayProcess, idxAtRest);
    }
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "G4ComptonScattering.hh"
#include "G4GammaConversion.hh"
#include "G4PhotoElectricEffect.hh"

#include "G4eMultipleScattering.hh"
#include "G4MuMultipleScattering.hh"
#include "G4hMultipleScattering.hh"

#include "G4eIonisation.hh"
#include "G4eBremsstrahlung.hh"
#include "G4eplusAnnihilation.hh"

#include "G4MuIonisation.hh"
#include "G4MuBremsstrahlung.hh"
#include "G4MuPairProduction.hh"

#include "G4hIonisation.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void OpNovicePhysicsList::ConstructEM()
{
  auto particleIterator = GetParticleIterator();
  particleIterator->reset();
  while( (*particleIterator)() )
  {
    G4ParticleDefinition* particle = particleIterator->value();
    G4ProcessManager* pmanager = particle->GetProcessManager();
    G4String particleName = particle->GetParticleName();

    if (particleName == "gamma") {
    // gamma
      // Construct processes for gamma
      pmanager->AddDiscreteProcess(new G4GammaConversion());
      pmanager->AddDiscreteProcess(new G4ComptonScattering());
      pmanager->AddDiscreteProcess(new G4PhotoElectricEffect());

    } else if (particleName == "e-") {
    //electron
      // Construct processes for electron
      pmanager->AddProcess(new G4eMultipleScattering(),-1, 1, 1);
      pmanager->AddProcess(new G4eIonisation(),       -1, 2, 2);
      pmanager->AddProcess(new G4eBremsstrahlung(),   -1, 3, 3);

    } else if (particleName == "e+") {
    //positron
      // Construct processes for positron
      pmanager->AddProcess(new G4eMultipleScattering(),-1, 1, 1);
      pmanager->AddProcess(new G4eIonisation(),       -1, 2, 2);
      pmanager->AddProcess(new G4eBremsstrahlung(),   -1, 3, 3);
      pmanager->AddProcess(new G4eplusAnnihilation(),  0,-1, 4);

    } else if( particleName == "mu+" ||
               particleName == "mu-"    ) {
    //muon
     // Construct processes for muon
     pmanager->AddProcess(new G4MuMultipleScattering(),-1, 1, 1);
     pmanager->AddProcess(new G4MuIonisation(),      -1, 2, 2);
     pmanager->AddProcess(new G4MuBremsstrahlung(),  -1, 3, 3);
     pmanager->AddProcess(new G4MuPairProduction(),  -1, 4, 4);

    } else {
      if ((particle->GetPDGCharge() != 0.0) &&
          (particle->GetParticleName() != "chargedgeantino") &&
          !particle->IsShortLived()) {
       // all others charged particles except geantino
       pmanager->AddProcess(new G4hMultipleScattering(),-1,1,1);
       pmanager->AddProcess(new G4hIonisation(),       -1,2,2);
     }
    }
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
#include "G4Threading.hh"

void OpNovicePhysicsList::ConstructOp()
{
  fCerenkovProcess = new G4Cerenkov("Cerenkov");
  fCerenkovProcess->SetMaxNumPhotonsPerStep(fMaxNumPhotonStep);
  fCerenkovProcess->SetMaxBetaChangePerStep(10.0);
  fCerenkovProcess->SetTrackSecondariesFirst(true);
  fScintillationProcess = new G4Scintillation("Scintillation");
  fScintillationProcess->SetScintillationYieldFactor(1.);
  fScintillationProcess->SetTrackSecondariesFirst(true);
  fAbsorptionProcess = new G4OpAbsorption();
  fRayleighScatteringProcess = new G4OpRayleigh();
  fMieHGScatteringProcess = new G4OpMieHG();
  fBoundaryProcess = new G4OpBoundaryProcess();

  fCerenkovProcess->SetVerboseLevel(fVerboseLevel);
  fScintillationProcess->SetVerboseLevel(fVerboseLevel);
  fAbsorptionProcess->SetVerboseLevel(fVerboseLevel);
  fRayleighScatteringProcess->SetVerboseLevel(fVerboseLevel);
  fMieHGScatteringProcess->SetVerboseLevel(fVerboseLevel);
  fBoundaryProcess->SetVerboseLevel(fVerboseLevel);
  
  // Use Birks Correction in the Scintillation process
  if(G4Threading::IsMasterThread())
  {
    G4EmSaturation* emSaturation =
              G4LossTableManager::Instance()->EmSaturation();
      fScintillationProcess->AddSaturation(emSaturation);
  }

  auto particleIterator=GetParticleIterator();
  particleIterator->reset();
  while( (*particleIterator)() ){
    G4ParticleDefinition* particle = particleIterator->value();
    G4ProcessManager* pmanager = particle->GetProcessManager();
    G4String particleName = particle->GetParticleName();
    if (fCerenkovProcess->IsApplicable(*particle)) {
      pmanager->AddProcess(fCerenkovProcess);
      pmanager->SetProcessOrdering(fCerenkovProcess,idxPostStep);
    }
    if (fScintillationProcess->IsApplicable(*particle)) {
      pmanager->AddProcess(fScintillationProcess);
      pmanager->SetProcessOrderingToLast(fScintillationProcess, idxAtRest);
      pmanager->SetProcessOrderingToLast(fScintillationProcess, idxPostStep);
    }
    if (particleName == "opticalphoton") {
      G4cout << " AddDiscreteProcess to OpticalPhoton " << G4endl;
      pmanager->AddDiscreteProcess(fAbsorptionProcess);
      pmanager->AddDiscreteProcess(fRayleighScatteringProcess);
      pmanager->AddDiscreteProcess(fMieHGScatteringProcess);
      pmanager->AddDiscreteProcess(fBoundaryProcess);
    }
  }
}
// Processes

// Processes
/*
#include "G4HadronicProcess.hh"

#include "G4ParticleHPElastic.hh"
#include "G4ParticleHPElasticData.hh"
#include "G4ParticleHPThermalScattering.hh"
#include "G4ParticleHPThermalScatteringData.hh"
*/
#include "G4HadronElasticProcess.hh"
#include "G4NeutronHPElasticData.hh"
#include "G4NeutronHPThermalScatteringData.hh"
#include "G4NeutronHPElastic.hh"
#include "G4NeutronHPThermalScattering.hh"

#include "G4NeutronInelasticProcess.hh"
#include "G4NeutronHPInelasticData.hh"
#include "G4NeutronHPInelastic.hh"

#include "G4HadronCaptureProcess.hh"
#include "G4NeutronHPCaptureData.hh"
#include "G4NeutronHPCapture.hh"

#include "G4HadronFissionProcess.hh"
#include "G4NeutronHPFissionData.hh"
#include "G4NeutronHPFission.hh"

#include "G4SystemOfUnits.hh"
void OpNovicePhysicsList::ConstructNeutron()
{
   auto particleIterator=GetParticleIterator();
   particleIterator->reset();
   while( (*particleIterator)() ){
    G4ParticleDefinition* particle = particleIterator->value();
    G4ProcessManager* pManager = particle->GetProcessManager();
    G4String particleName = particle->GetParticleName();
   if(particleName == "neutron")
   {
	   /*
	// process: elastic
   //
     G4HadronElasticPhysics::ConstructProcess();
     GetNeutronModel()->SetMinEnergy(19.5*MeV);
     G4bool fThermal = true;
	 G4HadronicProcess* process1 = GetNeutronProcess();
     G4ParticleHPElastic* model1 = new G4ParticleHPElastic();
     pManager->AddDiscreteProcess(process1);  
     process1->RegisterMe(model1);
     process1->AddDataSet(new G4ParticleHPElasticData());

    if (fThermal) {
     model1->SetMinEnergy(4*eV);  
     G4ParticleHPThermalScattering* model2 = new G4ParticleHPThermalScattering();
     process1->RegisterMe(model2);
     process1->AddDataSet(new G4ParticleHPThermalScatteringData());  
   }
   */

   // process: elastic
   //
   G4HadronElasticProcess* process1 = new G4HadronElasticProcess();
   pManager->AddDiscreteProcess(process1);   
   G4bool fThermal = true;
   //
   // cross section data set
   G4NeutronHPElasticData* dataSet1a = new G4NeutronHPElasticData();
   G4NeutronHPThermalScatteringData* dataSet1b 
                               = new G4NeutronHPThermalScatteringData();
  process1->AddDataSet(dataSet1a);                               
   if (fThermal) process1->AddDataSet(dataSet1b);
   //
   // models  thermal neutron process
   G4NeutronHPElastic*           model1a = new G4NeutronHPElastic();
   G4NeutronHPThermalScattering* model1b = new G4NeutronHPThermalScattering();
   if (fThermal)  model1a->SetMinEnergy(4*eV);
   process1->RegisterMe(model1a);    
   if (fThermal) process1->RegisterMe(model1b); 

   // process: inelastic
   //
   G4NeutronInelasticProcess* process2 = new G4NeutronInelasticProcess();
   pManager->AddDiscreteProcess(process2);   
   //
   // cross section data set
   G4NeutronHPInelasticData* dataSet2 = new G4NeutronHPInelasticData();
   process2->AddDataSet(dataSet2);                               
   //
   // models
   G4NeutronHPInelastic* model2 = new G4NeutronHPInelastic();
   process2->RegisterMe(model2);    

   // process: nCapture   
   //
   G4HadronCaptureProcess* process3 = new G4HadronCaptureProcess();
   pManager->AddDiscreteProcess(process3);    
   //
   // cross section data set
   G4NeutronHPCaptureData* dataSet3 = new G4NeutronHPCaptureData();
   process3->AddDataSet(dataSet3);                               
   //
   // models
   G4NeutronHPCapture* model3 = new G4NeutronHPCapture();
   process3->RegisterMe(model3);
   
   // process: nFission   
   //
   G4HadronFissionProcess* process4 = new G4HadronFissionProcess();
   pManager->AddDiscreteProcess(process4);    
   //
   // cross section data set
   G4NeutronHPFissionData* dataSet4 = new G4NeutronHPFissionData();
   process4->AddDataSet(dataSet4);                               
   //
   // models
   G4NeutronHPFission* model4 = new G4NeutronHPFission();
   process4->RegisterMe(model4);
  }
 }
}
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void OpNovicePhysicsList::SetVerbose(G4int verbose)
{
  fVerboseLevel = verbose;
  fCerenkovProcess->SetVerboseLevel(fVerboseLevel);
  fScintillationProcess->SetVerboseLevel(fVerboseLevel);
  fAbsorptionProcess->SetVerboseLevel(fVerboseLevel);
  fRayleighScatteringProcess->SetVerboseLevel(fVerboseLevel);
  fMieHGScatteringProcess->SetVerboseLevel(fVerboseLevel);
  fBoundaryProcess->SetVerboseLevel(fVerboseLevel);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void OpNovicePhysicsList::SetNbOfPhotonsCerenkov(G4int MaxNumber)
{
  fMaxNumPhotonStep = MaxNumber;

  fCerenkovProcess->SetMaxNumPhotonsPerStep(fMaxNumPhotonStep);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void OpNovicePhysicsList::SetCuts()
{
  //  " G4VUserPhysicsList::SetCutsWithDefault" method sets
  //   the default cut value for all particle types
  //
  //~ SetCutsWithDefault();
  defaultCutValue = 0.0001 * mm;
  //~ SetCutValue(defaultCutValue, "gamma");
  SetCutValue(defaultCutValue, "e-");
  SetCutValue(defaultCutValue, "e+");
  //~ G4double lowLimit = 250. * eV;
  //~ G4double highLimit = 100. * MeV;
//~ 
  //~ G4ProductionCutsTable::GetProductionCutsTable()->SetEnergyRange(lowLimit,
                                                                //~ highLimit);
  if (verboseLevel>0) DumpCutValuesTable();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
