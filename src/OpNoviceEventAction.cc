// $Id: B1EventAction.cc 75117 2013-10-28 09:38:37Z gcosmo $
//
/// \file B1EventAction.cc
/// \brief Implementation of the B1EventAction class

#include "G4Event.hh"
#include "G4RunManager.hh"
#include "G4UnitsTable.hh"
#include <math.h>
#include "Randomize.hh"

#include "G4AutoLock.hh"  // 文件锁，防止访问文件异常
namespace { G4Mutex detectiontex = G4MUTEX_INITIALIZER; }

#include "G4EventManager.hh"
#include "OpNoviceRunThread.hh"
#include "OpNoviceEventAction.hh"

#include <fstream>
#include "globals.hh"
#include <stdio.h>
#include "G4ios.hh"
using namespace std; //用于输出文件

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

OpNoviceEventAction::OpNoviceEventAction(OpNoviceRunAction* run )
	:G4UserEventAction(),
	 theRun(run),
	 TotalEnergy(0.)
{
	reach_counter[0] = 0; detect_counter[0] = 0;
	reach_counter[1] = 0; detect_counter[1] = 0;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

OpNoviceEventAction::~OpNoviceEventAction() { }

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void OpNoviceEventAction::BeginOfEventAction(const G4Event*) {
	reach_counter[0] = 0; detect_counter[0] = 0;
	reach_counter[1] = 0; detect_counter[1] = 0;
	TotalEnergy  = 0.0;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void OpNoviceEventAction::EndOfEventAction(const G4Event*) {

	OpNoviceRunThread* run =
	    static_cast<OpNoviceRunThread*>(G4RunManager::GetRunManager()->GetNonConstCurrentRun()); // accumulate statistics in Run
	
//------------------------------------------------------输出进度条----------------------------------------------------------/
      const G4Event* currentEvent = G4EventManager::GetEventManager()->GetConstCurrentEvent();
      G4int eventID = currentEvent->GetEventID();
      if(eventID%10 == 0) 
      {
		  G4int NumberEvent = run->GetNumberOfEventToBeProcessed();
		  G4cout<<"............................."<<eventID*1.0/NumberEvent*100<<"%........................"<<G4endl; 
	  }
	  
//------------------------------------------------------统计沉积能量----------------------------------------------------------/
	G4int channel = floor(TotalEnergy/(10*eV));
	run->AddSpectrum(channel);
	
//------------------------------------------------------获取达到PMT的光子数目----------------------------------------------------------/
   if(reach_counter[0]||reach_counter[1])  //任何一个PMT探测到光子便输出到文件中
   {
       // protect file reading via autolock
       G4AutoLock filelock(&detectiontex);
       filelock.lock();
         
       // 输出总发光数到文件
       fstream datafile("ReachNumber.txt", ios::out|ios::app);   // 追加
	   G4bool signal = datafile.is_open();
	   if(signal) datafile<<eventID<<" "<<reach_counter[0]<<" "<<reach_counter[1]<<endl;
	   datafile.close();
	   filelock.unlock();
	}
	
//------------------------------------------------------------------获取PMT探测粒子数目--------------------------------------------------------------/
  if(detect_counter[0]||detect_counter[1])  //任何一个PMT探测到光子便输出到文件中
   {
       // protect file reading via autolock
       G4AutoLock filelock(&detectiontex);
       filelock.lock();
         
      // 输出总发光数到文件
      fstream datafile("DetectNumber.txt", ios::out|ios::app);   // 追加
      G4bool signal = datafile.is_open();
      if(signal) datafile<<eventID<<" "<<detect_counter[0]<<" "<<detect_counter[1]<<endl;
	  datafile.close();
	  filelock.unlock();
   }
	    
	if(reach_counter[0] && reach_counter[1])  	run->AddCounterReach(); //必须PMT A/B同时接受到光子才算有效事件
	if(detect_counter[0]&&detect_counter[1])  	run->AddCounterDetect(); //必须PMT A/B同时接受到光子才算有效事件
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
