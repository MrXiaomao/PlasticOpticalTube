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
  // fopticalPhoton.insert(fopticalPhoton.end(), localRun->fopticalPhoton.begin(), localRun->fopticalPhoton.end());

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
  G4int NumberEvent = GetNumberOfEventToBeProcessed();
  // 生成以变参数为后缀的文件名
	std::ostringstream os;
	os << "DetectOptical_";
	os << opticalLength ;
  os << "cm_";
  os << NumberEvent;
	os << ".h5" ;
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

  // 写入到HDF5文件中
  Hdf5WriteValue write;
  write.CreateNewFile(wholepath);

  //切割数据
  int size = ftotalCounts.size();
  int validCounter = 0;
  if(size>0){
    // 记录有效事件中的物理量
    vector<G4double> EnDep;
    vector<G4int> yield_DetetCount;

    //记录光产额以及穿越水体的光子。
    vector<G4int> PhotonNum;
    vector<G4double> ratio;

    for ( const auto& count : ftotalCounts ) {
      if(count.PMTA>0 && count.PMTB>0) {
        validCounter++;
        EnDep.push_back(count.Edep/keV);
        yield_DetetCount.push_back(count.scinYield);
        yield_DetetCount.push_back(count.IntoWater);
        yield_DetetCount.push_back(count.PMTA);
        yield_DetetCount.push_back(count.PMTB);
        yield_DetetCount.push_back(count.PMTA_water);
        yield_DetetCount.push_back(count.PMTB_water);
      }
      PhotonNum.push_back(count.scinYield);
      PhotonNum.push_back(count.IntoWater);
      ratio.push_back(count.IntoWater*1.0/count.scinYield);
    }

    if(validCounter>0){
      G4cout<<"Detect Counter size = "<<size<<G4endl;

      write.CreateGroup("Data1");
      write.CreateDataspace(1, 1, validCounter);
      write.CreateDoubleDataset("EventEdep");
      write.WriteDoubleValue(EnDep.data());

      write.CreateDataspace(2, 6, validCounter);
      write.CreateIntDataset("EventDetect");
      write.WriteIntValue(yield_DetetCount.data());
      //清空容器,及时释放系统内存
      vector<G4double>().swap(EnDep);
      vector<G4int>().swap(yield_DetetCount);
    }
    write.CreateGroup("Data2");
    write.CreateDataspace(2, 2, size);
    write.CreateIntDataset("Yield_water");
    write.WriteIntValue(PhotonNum.data());
    
    write.CreateDataspace(1, 1, validCounter);
    write.CreateDoubleDataset("ratio");
    write.WriteDoubleValue(ratio.data());
    //清空容器,及时释放系统内存
    vector<G4double>().swap(ratio);
    vector<G4int>().swap(PhotonNum);
  }
  write.CloseFile();
}