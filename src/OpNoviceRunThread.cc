//
// $Id: OpNoviceRunThread.cc 2020-10-19 10:38:43 $
//
#include "OpNoviceRunThread.hh"
#include "Hdf5Function.h"
#include "OpNoviceDetectorConstruction.hh"
#include "G4RunManager.hh"

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
  // 获取模拟中的变参数
  G4double opticalLength = 20*cm;
  const OpNoviceDetectorConstruction* detectorConstruction
    = static_cast<const OpNoviceDetectorConstruction*> (G4RunManager::GetRunManager()->GetUserDetectorConstruction());
  if(detectorConstruction) {
    opticalLength = detectorConstruction->GetDetectorLength();
  }
  else{
    G4cout<<"无法找到水体长度"<<G4endl;
    return;
  }

  //转化为cm为单位的数值
  opticalLength /= cm;

  // 生成以变参数为后缀的文件名
	std::ostringstream os;
	os << "DetectOptical_";
	os << opticalLength ;
	os << "cm.h5" ;
	G4String fileName = os.str();
  
  // 若存在旧文件，则先删除
  G4String outPutPath = "../OutPut/";
  G4String wholepath = outPutPath + fileName;
  // const char* file_path = "example.txt"; // 指定文件路径
  if (remove(wholepath) != 0) { // 尝试删除文件
    G4cout << wholepath <<" is not exist." << G4endl;
  } else {
    G4cout << wholepath <<" is deleted successfully." << G4endl;
  }

  //切割数据
  int size = ftotalCounts.size();
  if(size>0){
    vector<G4double> EnDep;
    vector<G4int> yield;
    vector<G4int> DetetCount;
    EnDep.resize(size);
    yield.resize(size);
    DetetCount.resize(size*2);
    int i=0;
    for ( const auto& count : ftotalCounts ) {
      EnDep[i] = count.Edep/keV;
      yield[i] = count.scinYield;
      DetetCount[i*2] = count.PMTA;
      DetetCount[i*2+1] = count.PMTB;
      i++;
    }

    G4cout<<"Counter size = "<<size<<G4endl;

    // 写入到HDF5文件中
    Hdf5WriteValue write;
    write.CreateNewFile(wholepath);
    // write.CreateNewFile("../OutPut/DetectOptical.h5");
    write.CreateGroup("Data");
    write.CreateDataspace(1, 1, size);
    write.CreateDoubleDataset("EventEdep");
    write.WriteDoubleValue(EnDep.data());
    
    write.CreateDataspace(1, 1, size);
    write.CreateIntDataset("EventYield");
    write.WriteIntValue(yield.data());

    write.CreateDataspace(2, 2, size);
    write.CreateIntDataset("EventDetect");
    write.WriteIntValue(DetetCount.data());
    write.CloseFile();
  }
}