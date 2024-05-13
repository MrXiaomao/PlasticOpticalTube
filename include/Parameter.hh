#ifndef ACTIVATE_CONSTANTS_H
#define ACTIVATE_CONSTANTS_H 1

//存放本程序所使用到的各个变参数

#include "globals.hh"
#include "G4SystemOfUnits.hh"
namespace myConsts {

struct Detect{
    Detect():PMTA(0),PMTB(0){}
    Detect(G4int a, G4int b):PMTA(a),PMTB(b){}
    G4int PMTA; //PMTA收集的粒子数
    G4int PMTB; //PMTB收集的粒子数
};

static constexpr G4double gGountBeginTime = 10.*CLHEP::s; // 统计沉积能量最小起始时刻，若小于该时刻的沉积能量，则不统计
static constexpr G4int gEnChannel = 2000; // 统计沉积能量最小起始时刻，若小于该时刻的沉积能量，则不统计
static constexpr G4double gEnbin = 0.1*keV; // 统计沉积能量道宽
}

#endif