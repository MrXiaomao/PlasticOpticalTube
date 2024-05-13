/// \file OpNovice/src/OpNovicePrimaryGeneratorAction.cc
/// \brief Implementation of the OpNovicePrimaryGeneratorAction class
//
//
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "OpNovicePrimaryGeneratorAction.hh"
#include "OpNovicePrimaryGeneratorMessenger.hh"
#include "OpNoviceDetectorConstruction.hh"

#include "G4LogicalVolumeStore.hh"
#include "G4LogicalVolume.hh"
#include "G4Tubs.hh"
#include "G4RunManager.hh"

#include "G4Event.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"

#include "G4SystemOfUnits.hh"
#include "Randomize.hh"
#include <stdio.h>
using namespace std;

#include <iostream>
#include <fstream>

G4int OpNovicePrimaryGeneratorAction::columNum[19] = {7, 11, 13, 15, 17, 17, 19, 19, 19, 19, 19, 19, 19, 17, 17, 15, 13, 11, 7};   // 每一列中对应的光纤数目，从左往右数起
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

OpNovicePrimaryGeneratorAction::OpNovicePrimaryGeneratorAction():
	G4VUserPrimaryGeneratorAction(),
	fParticleGun(0), firstFlag(true), fWaterShellTubs(0),
	fOpticalTubs(0), waterRadius(0.0),
	opticalRadius(0.0), opticalDistance(0.0) {
	SumFiber = 0;
	for(G4int i=0; i<c_colums; i++) {
		SumFiber+=columNum[i];                          // 计算光线总数目，SumFiber
		if(i>0) columDistribution[i] = columDistribution[i-1] + columNum[i];    // 对数组columDistribution[]初始化
		else    columDistribution[i]  = columNum[i];
	}

	// 判断能谱文件的行数是否小于预置数组长度
	G4int Lines = CountLines("../Spectrum.txt"); // 从上一级目录中Spectrum.txt文件获取能谱
	if(Lines==0) {
		G4ExceptionDescription msg;
		msg << "The Spectrum.txt is a empty file.\n";
		msg << "Please check this file";
		G4Exception("OpNovicePrimaryGeneratorAction::CountLines()",
		            "MyCode0001",FatalException,msg);
	}
	if(Lines>EnergyNum) { // 总概率为零，能谱文件大于预置能谱长度，需要用户重新增大EnergyNum的值，
		G4ExceptionDescription msg;
		msg << "The Total Lines of EnergySpectrum is more than 'EnergyNum'.\n";
		msg << "Please increase 'EnergyNum' according to your Spectrum.txt\n in file OpNovicePrimaryGeneratorAction.hh";
		msg << "the default of EnergyNum is100. Cannot continue!!!";
		G4Exception("OpNovicePrimaryGeneratorAction::EnergyNum",
		            "MyCode0004",FatalException,msg);
	}

	for(int i = 0; i<EnergyNum; i++) {
		EnergySpectrum[i][0] = 0.0;
		EnergySpectrum[i][1] = 0.0;
	}

	GetEnergySpectrum("../Spectrum.txt");    // 从上一级目录中Spectrum.txt文件获取能谱

	G4int n_particle = 1;
	fParticleGun = new G4ParticleGun(n_particle);
	fGunMessenger = new OpNovicePrimaryGeneratorMessenger(this);

	G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
	G4ParticleDefinition* particle = particleTable->FindParticle("e-");    // alpha  proton   mu+  e-  gamma

	fParticleGun->SetParticleDefinition(particle);
	fParticleGun->SetParticleEnergy(500.0*keV);                                      // 氚水衰变的平均能量5.682*keV，这条语句会被GeneratePrimaries(G4Event* anEvent)中新的粒子能量所覆盖
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

OpNovicePrimaryGeneratorAction::~OpNovicePrimaryGeneratorAction() {
	delete fParticleGun;
	delete fGunMessenger;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void OpNovicePrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent) {
	// 获取探测器部分的几何尺寸
	if(firstFlag)  GetGeometrySize();  // firstFlag确保只会在第一次进入时运行，注意：GetGeometrySize()函数只能在构造函数完成之后进行，

	// 发射位置
	G4int  VolumeID = GetVolumeID();       // 抽样决定发射粒子属于那一个光纤外围，抽出编号。
	G4int  Colum = 0;                      // 发射位置所在列数，这里初始化为0，防止未初始化。
	for(G4int i=0; i<c_colums; i++) {
		if(VolumeID<columDistribution[i]) { // 确定光纤的列号
			Colum = i;
			break;
		}
	}
    
	G4int Line = VolumeID-columDistribution[Colum]+columNum[Colum];  // 确定光纤的行号，行号从下往上数，0~columNum[i];
	//~ G4cout<<"VolumeID = "<<VolumeID<<"Colum = "<<Colum<<", Line = "<<Line<<G4endl;

	G4double CenterX = (opticalRadius*2 + opticalDistance)*(Colum-8); // VolumeID光纤的中心X坐标
	G4double CenterY = (opticalRadius*2 + opticalDistance)*(Line - (columNum[Colum]-1)*0.5); // VolumeID光纤的中心Y坐标
	
	//~ G4cout<<"CenterX = "<<CenterX/(opticalRadius*2 + opticalDistance)<<G4endl;
	//~ G4cout<<"CenterY = "<<CenterY/(opticalRadius*2 + opticalDistance)<<G4endl;
    
	// ----------------------------------------环内抽样-------------------------------------------------------
	G4double OuterRadius = opticalRadius+5.0*um;                   // 在光纤外部5um的厚度范围内抽样
	G4double OuterRadiusPow = OuterRadius*OuterRadius;    // 外环半径平方
	G4double InnerRadiusPow = opticalRadius*opticalRadius; // 内环半径平方

	// 圆环内均抽样,该抽样方法来自韩纪峰老师课件，请读者自己用简单画图软件检验，
	// 在该工程项目中已经有一个采用matlab代码编写的sampling_circularRing.m文件，用于验证抽样均匀性。
	G4double theta = G4UniformRand()*2*3.14159;
	G4double Radius = sqrt((OuterRadiusPow-InnerRadiusPow)*G4UniformRand()+InnerRadiusPow);
	//~ G4cout<<"Radius = "<<Radius<<G4endl;
	G4double x0 = Radius*sin(theta) + CenterX;
	G4double y0 = Radius*cos(theta) + CenterY;
	G4double z0 = (G4UniformRand()-0.5)*waterLength;

	// 粒子发射方向 4pi均匀
	G4double phi,costheta,sintheta;
	phi = G4UniformRand()*360*deg;
	costheta = G4UniformRand()*2-1; //0~180deg
	sintheta = sqrt(1-costheta*costheta);
	G4double u = sintheta*cos(phi);
	G4double v = sintheta*sin(phi);
	G4double w = costheta;

	// 粒子能量
	G4double Rand = G4UniformRand();
	for(G4int i=0; i<EnergyNum; i++) { //注意这里从1开始
		if(Rand<EnergySpectrum[i][1]) {
			fParticleGun->SetParticleEnergy(EnergySpectrum[i][0]);
			//~ G4cout<<"Energy = "<<EnergySpectrum[i][0]<<G4endl;
			break;
		}
	}

	fParticleGun->SetParticlePosition(G4ThreeVector(x0,y0,z0));
	fParticleGun->SetParticleMomentumDirection(G4ThreeVector(u,v,w));
	fParticleGun->GeneratePrimaryVertex(anEvent);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void OpNovicePrimaryGeneratorAction::GetGeometrySize() {
	firstFlag = false;  // 确保只进入一次该函数

	//--------------------------------------获取 logicWater 的半径和长度 ------------------------------------------------//
	if (!fWaterShellTubs) {
		G4LogicalVolume* waterLV
		= G4LogicalVolumeStore::GetInstance()->GetVolume("logicWaterShell");      // 由于logicWater是一个布尔逻辑体，所以不能直接获取他的几何参数， 改为获取logicWaterShell
		if ( waterLV ) fWaterShellTubs = dynamic_cast<G4Tubs*>(waterLV->GetSolid());
	}

	if ( fWaterShellTubs ) {
		waterRadius = fWaterShellTubs->GetInnerRadius();
		waterLength = fWaterShellTubs->GetZHalfLength()*2.;
	} else  {
		G4ExceptionDescription msg;
		msg << "logicWater volume of Tubs shape not found.\n";
		msg << "Perhaps you have changed geometry.\n";
		msg << "Cannot continue.";
		G4Exception("OpNovicePrimaryGeneratorAction::GetGeometrySize()",
		            "MyCode0001",FatalException,msg);
	}

	//--------------------------------------获取 logicOpticaltube 的半径 ------------------------------------------------//
	if (!fOpticalTubs) {
		G4LogicalVolume* OpticalShellLV
		= G4LogicalVolumeStore::GetInstance()->GetVolume("logicOpticaltube");
		if ( OpticalShellLV ) fOpticalTubs = dynamic_cast<G4Tubs*>(OpticalShellLV->GetSolid());
	}

	if ( fOpticalTubs ) {
		opticalRadius = fOpticalTubs->GetOuterRadius();
	} else  {
		G4ExceptionDescription msg;
		msg << "logicOpticalshell volume of Tubs shape not found.\n";
		msg << "Perhaps you have changed geometry.\n";
		msg << "The gun will be place at the center.";
		G4Exception("OpNovicePrimaryGeneratorAction::GetGeometrySize()",
		            "MyCode0002", FatalException, msg);
	}

	//--------------------------------------获取每两根 Opticalshell 的间距 ------------------------------------------------//
	const OpNoviceDetectorConstruction* detectorConstruction
	= static_cast<const OpNoviceDetectorConstruction*>
	  (G4RunManager::GetRunManager()->GetUserDetectorConstruction());
	if(detectorConstruction) {
		opticalDistance = detectorConstruction->GetOpticalDistance();
	}
	if( opticalDistance == 0.0) {
		G4ExceptionDescription msg;
		msg << "opticalTubeDistance = 0.0. ; This is an warning.\n";
		msg << "Perhaps you have set a wrong opticalTubeDistance for logicOpticalshell.\n";
		G4Exception("OpNovicePrimaryGeneratorAction::GetGeometrySize()",
		            "MyCode0003", JustWarning, msg);
	}
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
///@ 从文件中读取能谱，请注意，该文件只能是n*2类型的数据，也就是两列数据
///@ 然后将能谱进行归一化
void OpNovicePrimaryGeneratorAction::GetEnergySpectrum(const char *filename) {
	//---------------------------------------------- 读取文件 ------------------------------------------//
	fstream ReadData(filename,ios::in);
	if(ReadData.fail()) { //文件打开失败:返回0
		G4ExceptionDescription msg;
		msg << "Can not found Spectrum.txt file. '.\n";
		msg << "You may forget to set Spectrum.txt file or you have set in wrong fiel path.";
		msg << "Please put the  'Spectrum.txt' in the same path with CMakeLists.txt";
		G4Exception("OpNovicePrimaryGeneratorAction::GetEnergySpectrum()",
		            "MyCode0005",FatalException,msg);
	} else {
		for(G4int i=0; i<EnergyNum; i++) {
			if (!ReadData.eof() ) { //判断文件是否读到尾部
				ReadData>>EnergySpectrum[i][0];
				ReadData>>EnergySpectrum[i][1];
			} else {  // 文件已读完，则对数组剩余部分用0.0填充
				EnergySpectrum[i][0] = 0.;
				EnergySpectrum[i][1] = 0.;
			}
		}
	}
	ReadData.close();

	/// 测试能谱是否读出成功
	for(int i = 0; i<EnergyNum; i++) {
		G4cout<<"EnergySpectrum";
		G4cout<<i<<" Array "<<EnergySpectrum[i][0]<<",  "<<EnergySpectrum[i][1]<<G4endl;
	}

	//----------------------------------------------对概率进行归一化------------------------------------------//
	//
	/// 1)、求出总概率
	G4double sum = 0;
	for (int i =0; i<EnergyNum; i++) {
		sum += EnergySpectrum[i][1];
	}
	if(sum == 0) { // 总概率为零，能谱文件内容异常，抛出异常，停止运行
		G4ExceptionDescription msg;
		msg << "The Total probability of EnergySpectrum is equal to zero.\n";
		msg << "Please check is somthing wrong with your Spectrum.txt.\n";
		msg << "Cannot continue.";
		G4Exception("OpNovicePrimaryGeneratorAction::GetEnergySpectrum(const char *filename)",
		            "MyCode0004",FatalException,msg);
	}

	/// 2）、概率归一
	for (G4int i = 0; i<EnergyNum; i++) {
		EnergySpectrum[i][1] = EnergySpectrum[i][1]/sum;    //归一化
	}

	for (G4int i = 1; i<EnergyNum; i++) {
		EnergySpectrum[i][1] = EnergySpectrum[i][1] + EnergySpectrum[i-1][1] ;    //作概率累加替换成新的数组，等同于数学中的概率函数P=F（x<x0）
	}
	for(int i = 0; i<EnergyNum; i++) {
		G4cout<<"normalization "<<i<<"  "<<EnergySpectrum[i][0]<<",  "<<EnergySpectrum[i][1]<<G4endl;
	}

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
G4int OpNovicePrimaryGeneratorAction::CountLines(const char *filename) {
	ifstream ReadFile;
	G4int n=0;
	G4String temp;
	ReadFile.open(filename,ios::in);//ios::in 表示以只读的方式读取文件
	if(ReadFile.fail()) { //文件打开失败:返回0
		G4ExceptionDescription msg;
		msg << "Can not found Spectrum.txt file. '.\n";
		msg << "You may forget to set Spectrum.txt file or you have set in wrong fiel path.";
		msg << "Please put the  'Spectrum.txt' in the same path with CMakeLists.txt";
		G4Exception("OpNovicePrimaryGeneratorAction::GetEnergySpectrum()",
		            "MyCode0005",FatalException,msg);
		return 0;
	} else { //文件存在
		while(getline(ReadFile,temp)) {
			n++;
		}
	}
	ReadFile.close();
	return n;
}
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4int OpNovicePrimaryGeneratorAction::GetVolumeID() {
	G4int Num = 0;
	G4double Rand = G4UniformRand();                               // 产生一个随机数
	G4double cellRatio = 1./SumFiber;  // 单个光纤占总光纤数量的份额

	// 抽样出几何体编号
	for(int i = 0; i<SumFiber; i++) {
		G4double probability_i = cellRatio*(i+1);  // 第i行j列光纤对应的分布函数概率。
		if(Rand<probability_i) {
			Num = i;
			return Num;
		}
	}
	return Num;  // 防止意外发射。也就是在上面的for循环中没有return Num.
}
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
