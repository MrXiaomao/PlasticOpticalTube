// $Id: B1EventAction.hh 75216 2013-10-29 16:08:11Z gcosmo $
//
/// \file B1EventAction.hh
/// \brief Definition of the B1EventAction class

#ifndef OpNoviceEventAction_h
#define OpNoviceEventAction_h 1

#include "G4UserEventAction.hh"
#include "globals.hh"

#include "OpNoviceRunAction.hh"
#include "G4SystemOfUnits.hh"
#include <ctime>

/// Event action class
///

class OpNoviceEventAction : public G4UserEventAction {
public:
	OpNoviceEventAction(OpNoviceRunAction* run);
	virtual ~OpNoviceEventAction();

	virtual void BeginOfEventAction(const G4Event* event);
	virtual void EndOfEventAction(const G4Event* event);

    void AddDepositedEnergy(G4double Energy) { TotalEnergy += Energy;}
    
	void AddPMTCounter(G4int VolumeID)    { reach_counter[VolumeID]++;     }
	void AddDetectCounter(G4int VolumeID) { detect_counter[VolumeID]++;}

	OpNoviceRunAction* theRun;

private:
    std::time_t fstart_time;
	G4int reach_counter[2];            // 统计到达PMT A/B的光子数目
	G4int detect_counter[2];           // 统计PMT A/B探测到的光子数目
	G4double TotalEnergy;             // 收集一次事件中在闪烁体里面沉积的能量
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
#endif
