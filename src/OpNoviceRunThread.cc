//
// $Id: OpNoviceRunThread.cc 2020-10-19 10:38:43 $
//
#include "OpNoviceRunThread.hh"

OpNoviceRunThread::OpNoviceRunThread()
	: G4Run(),
	  ReachCounts(0),DetectCounts(0)
{
	  for(int j=0; j<2000; j++)
      { 
		 Energy[j]=0;
	  }
}

OpNoviceRunThread::~OpNoviceRunThread()
{}

/// @brief Void Merge

void OpNoviceRunThread::Merge(const G4Run* run) {
	const OpNoviceRunThread* localRun = static_cast<const OpNoviceRunThread*>(run);
	ReachCounts  += localRun->ReachCounts;
	DetectCounts += localRun->DetectCounts;
	  for(int j=0; j<2000; j++)
      { 
		 Energy[j] += localRun->Energy[j];
	  }
	G4Run::Merge(run);
}
/// End of file
