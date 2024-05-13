// $Id: OpNoviceRunThread.hh  2020-10-19 10:38:43 $
#ifndef OpNoviceRunThread_h
#define OpNoviceRunThread_h 1

#include "G4Run.hh"
#include <vector>
// #include <map>
#include <stdio.h>
#include "Parameter.hh"
using namespace myConsts;
using namespace std;

class OpNoviceRunThread : public G4Run {
public:
	OpNoviceRunThread();
	virtual ~OpNoviceRunThread();

	virtual void Merge(const G4Run*);
	void AddCounterReach() {ReachCounts++;}
	void AddCounterDetect(Detect count) {ftotalCounts.push_back(count);}
  void AddSpectrum(G4int ch) { MultiEnergy[ch]++;}  // 统计能量沉积，按道址统计

	void EndOfRun();

private:
	G4int ReachCounts;
	vector<Detect> ftotalCounts;
	G4int MultiEnergy[gEnChannel];  // 每个道址的能量为10eV
};

/// End of file

#endif

