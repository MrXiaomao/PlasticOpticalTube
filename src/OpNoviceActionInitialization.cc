
// $Id: OpNoviceActionInitialization.cc 68058 2013-03-13 14:47:43Z gcosmo $
//
/// \file OpNoviceActionInitialization.cc
/// \brief Implementation of the OpNoviceActionInitialization class

#include "OpNoviceActionInitialization.hh"
#include "OpNovicePrimaryGeneratorAction.hh"
#include "OpNoviceRunAction.hh"
#include "OpNoviceEventAction.hh"
#include "OpNoviceTrackingAction.hh"
#include "OpNoviceSteppingAction.hh"

#include "OpNoviceStackingAction.hh"
#include "OpNoviceSteppingVerbose.hh"

//<unistd.h>中的access可以判断文件是否存在，<>中的可以创建文件。
#include <unistd.h>

#include <sys/stat.h>
#include <sys/types.h>

#include <stdio.h>
using namespace std;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

OpNoviceActionInitialization::OpNoviceActionInitialization()
	: G4VUserActionInitialization()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

OpNoviceActionInitialization::~OpNoviceActionInitialization()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void OpNoviceActionInitialization::BuildForMaster() const {
  
  // 清除历史输出文件
  const char *path = "../OutPut/";
  G4String G4path = path;
  //判断文件夹是否存在，不存在则创建
  if (access(path, 0) == -1){   //如果文件夹不存在
    mkdir(G4path,0777);
  }
  /*
  else{
    // 删除目录中的所有内容
    std::string command = "rm -r " + std::string(path);  // 构造删除命令
    int result = std::system(command.c_str());  // 调用系统命令
    if (result == 0) {
        G4cout << "成功删除文件夹：" << path << G4endl;
    } else {
        G4cout << "删除文件夹失败：" << path << G4endl;
    }
  }
  mkdir(G4path,0777);
  */

	SetUserAction(new OpNoviceRunAction());
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void OpNoviceActionInitialization::Build() const {
	
	SetUserAction(new OpNovicePrimaryGeneratorAction());
	
	  OpNoviceRunAction* RunAct = new OpNoviceRunAction;
	  SetUserAction(RunAct);
      
	  OpNoviceEventAction* EvAct =new OpNoviceEventAction(RunAct);
	  SetUserAction( EvAct );
	  //~ 
      SetUserAction(new OpNoviceStackingAction(EvAct));              // 获取总的发光粒子数
      
      SetUserAction(new OpNoviceTrackingAction(EvAct));     // 获取到达PMT的光子数 被PMT探测的粒子数
      
	  SetUserAction(new OpNoviceSteppingAction( EvAct ));  // 获取能量沉积
	  
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VSteppingVerbose*
OpNoviceActionInitialization::InitializeSteppingVerbose() const {
	return new OpNoviceSteppingVerbose();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
