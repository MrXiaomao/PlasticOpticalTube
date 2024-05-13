// $Id: OpNoviceRunThread.hh  2020-10-19 10:38:43 $
#ifndef OpNoviceRunThread_h
#define OpNoviceRunThread_h 1

#include "G4Run.hh"

class OpNoviceRunThread : public G4Run {
public:
	OpNoviceRunThread();
	virtual ~OpNoviceRunThread();

	virtual void Merge(const G4Run*);
	void AddCounterReach() {ReachCounts++;}
	void AddCounterDetect() {DetectCounts++;}
    void AddSpectrum(G4int ch) { Energy[ch]++;}   // 统计能量沉积，按道址统计
    
	G4int GetReachCounts() const { return ReachCounts;	}
	G4int GetDetectCounts() const { return DetectCounts;	}
	
    G4int GetSpectrum(int ch)  const
    { 
		if(ch<2000) return Energy[ch];   // 防止指针越界
		else return 0;
	}
	
private:
	G4int ReachCounts;
	G4int DetectCounts;
	G4int Energy[2000];  // 每个道址的能量为10eV
};

/// End of file

#endif

