//
// $Id: OpNoviceRunThread.cc 2020-10-19 10:38:43 $
//
#include "OpNoviceRunThread.hh"
#include "Hdf5Function.h"

OpNoviceRunThread::OpNoviceRunThread()
	: G4Run(),
	  ReachCounts(0)
{
	  for(int j=0; j<2000; j++)
    { 
      MultiEnergy[j]=0;
	  }
}

OpNoviceRunThread::~OpNoviceRunThread()
{}

/// @brief Void Merge

void OpNoviceRunThread::Merge(const G4Run* run) {
	const OpNoviceRunThread* localRun = static_cast<const OpNoviceRunThread*>(run);
	ReachCounts  += localRun->ReachCounts;
  for(int j=0; j<2000; j++)
  {
    MultiEnergy[j] += localRun->MultiEnergy[j];
  }
  
  //合并容器
  ftotalCounts.insert(ftotalCounts.end(), localRun->ftotalCounts.begin(), localRun->ftotalCounts.end());

	G4Run::Merge(run);
}

void OpNoviceRunThread::EndOfRun(){
  int size = ftotalCounts.size();
  if(size>0){
    vector<G4int> DetetCount;
    DetetCount.resize(size*2);
    int i=0;
    for ( const auto& count : ftotalCounts ) {
      DetetCount[i++] = count.PMTA;
      DetetCount[i++] = count.PMTB;
    }
    G4cout<<"TimeEdep size = "<<size<<G4endl;
    
    // 写入到HDF5文件中
    Hdf5WriteValue write;
    // G4String outPutPath = "../OutPut/";
    write.CreateNewFile("../OutPut/DetectOptical.h5");
    write.CreateGroup("Data");
    write.CreateDataspace(2, 2, size);
    write.CreateIntDataset("EventEdepTime");
    write.WriteIntValue(DetetCount.data());
    write.CloseFile();
  }
}