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
#include <map>

/// Event action class
///

class OpNoviceEventAction : public G4UserEventAction {
public:
	OpNoviceEventAction(OpNoviceRunAction* run);
	virtual ~OpNoviceEventAction();

	virtual void BeginOfEventAction(const G4Event* event);
	virtual void EndOfEventAction(const G4Event* event);

	inline void AddScinYield(G4int count) { fopYield += count;}
    inline void AddDepositedEnergy(G4double Energy) { TotalEnergy += Energy;}
	inline void AddTrackIDThroughWater(G4int trackID) { fIntoWaterOphonton[trackID] = true;}
    //到达PMT的光子
	void AddPMTCounter(G4int VolumeID)    { reach_counter[VolumeID]++;}
	//被PMT捕获的光子
	void AddDetectCounter(G4int PMT_ID)    { detect_counter[PMT_ID]++;}
	//穿过水体并最后被PMT捕获的光子
	void AddDetectWaterCounter(G4int PMT_ID)    { fDetCounterWater[PMT_ID]++;}

	OpNoviceRunAction* theRun;
	std::map<G4int, G4bool> fIntoWaterOphonton; // 记录穿越水体的可见光光子ID

private:
    std::time_t fstart_time;
	G4int reach_counter[2];            // 统计到达PMT A/B的光子数目
	G4int fDetCounterWater[2];         // 统计PMT A/B探测的光子数目，这些光子中途穿过水体
	G4int detect_counter[2];           // 统计PMT A/B探测到的光子数目
	G4double fopYield; 				   // 光产额
	G4double TotalEnergy;              // 收集一次事件中在闪烁体里面沉积的能量
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
#endif
