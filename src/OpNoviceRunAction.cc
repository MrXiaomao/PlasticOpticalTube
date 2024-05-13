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

#include "G4ios.hh"
#include <fstream>
#include <stdio.h>
#include "G4SystemOfUnits.hh"

using namespace std;
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

OpNoviceRunAction::OpNoviceRunAction()
	: G4UserRunAction(),
	  fTimer(0)
{
	fTimer = new G4Timer;
		
	//每次新的运行都删除旧文件
	if(remove("result.txt")==0)
		G4cout<<"The 'result.txt' has been deleted!"<<G4endl;
	else
		G4cout<<"The 'result.txt' is failed to be deleted!"<<G4endl;
		
	if(remove("EnergySpectrum.txt")==0)
		G4cout<<"The 'EnergySpectrum.txt' has been deleted!"<<G4endl;
	else
		G4cout<<"The 'EnergySpectrum.txt' is failed to be deleted!"<<G4endl;

	if(remove("ReachNumber.txt")==0)
		G4cout<<"The 'ReachNumber.txt' has been deleted!"<<G4endl;
	else
		G4cout<<"The 'ReachNumber.txt' is failed to be deleted!"<<G4endl;

	if(remove("DetectNumber.txt")==0)
		G4cout<<"The 'DetectNumber.txt' has been deleted!"<<G4endl;
	else
		G4cout<<"The 'DetectNumber.txt' is failed to be deleted!"<<G4endl;
		
	if(remove("opticalProduction.txt")==0)
		G4cout<<"The 'opticalProduction.txt' has been deleted!"<<G4endl;
	else
		G4cout<<"The 'opticalProduction.txt' is failed to be deleted!"<<G4endl;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

OpNoviceRunAction::~OpNoviceRunAction() {
	delete fTimer;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4Run* OpNoviceRunAction::GenerateRun() {
	return new OpNoviceRunThread;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

//~ void OpNoviceRunAction::BeginOfRunAction(const G4Run* aRun) {
//~ }

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void OpNoviceRunAction::EndOfRunAction(const G4Run* aRun) {
	fTimer->Stop();
// G4cout << "number of event = " << aRun->GetNumberOfEvent()
	//  << " " << *fTimer << G4endl;		
	const OpNoviceRunThread* dERun = static_cast<const OpNoviceRunThread*>(aRun);
	G4int counter1 = dERun->GetReachCounts();   // 到达PMT的光子数，也就是当作PMT有100%的量子效率
    G4int counter2 = dERun->GetDetectCounts();  // 考虑PMT量子效率后的探测粒子数目
    
	if (IsMaster()) {
		fstream datafile("result.txt", ios::out|ios::app);   
		G4bool signal = datafile.is_open();
		if(signal) {
			datafile<<counter1<<" "<<counter2<<endl;
		}
		datafile.close();
		
		fstream datafile2("EnergySpectrum.txt", ios::out|ios::trunc);   
		signal = datafile2.is_open();
		if(signal) {
			for(int j=0; j<2000; j++)
             { 
			   G4int  counts = dERun->GetSpectrum(j);
		       datafile2<<j*10<<" "<<counts<<endl;  // 每一道10eV
	         }
		}
		datafile2.close();
		
		G4cout<< G4endl<< "--------------------End of Global Run-----------------------"<<G4endl;
		G4cout<<*fTimer<<G4endl;
		G4cout<< "ReachCounts = "<<counter1<<", DetectCount = "<<counter2<<G4endl;
	} else {
		G4cout<< G4endl << "--------------------End of Local Run------------------------"<<G4endl;
		G4cout<<*fTimer<<G4endl;
		G4cout<< "ReachCounts = "<<counter1<<", DetectCount = "<<counter2<<G4endl;
		//~ fstream datafile;
		//~ datafile.open("output.txt", ios::out|ios::app);
		//~ datafile.open("output.txt",  ios::out|ios::trunc); ///如果文件存在则先删除文件
	}
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
