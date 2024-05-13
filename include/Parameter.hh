#ifndef ACTIVATE_CONSTANTS_H
#define ACTIVATE_CONSTANTS_H 1

//存放本程序所使用到的各个变参数

#include "globals.hh"
#include "G4SystemOfUnits.hh"
namespace myConsts {

struct Detect{
    Detect():Edep(0.),scinYield(0),IntoWater(0),PMTA(0),PMTB(0),PMTA_water(0), PMTB_water(0){}
    Detect(G4double edep, G4int yield, G4int intoWater, G4int pmtA, G4int pmtB, G4int pmtA_water, G4int pmtB_water):
        Edep(edep), scinYield(yield), IntoWater(intoWater), PMTA(pmtA), PMTB(pmtB), PMTA_water(pmtA_water), PMTB_water(pmtB_water){}
    G4double Edep; //本次事件的趁机能量
    G4int scinYield; // 闪烁光子的产额
    G4int IntoWater; // 穿过水体的闪烁光子
    G4int PMTA; //PMTA收集的粒子数
    G4int PMTB; //PMTB收集的粒子数
    G4int PMTA_water; //PMTA收集的粒子数
    G4int PMTB_water; //PMTB收集的粒子数
};

struct PhotonGroup{
    PhotonGroup():scinYield(0),IntoWaterCount(0),ratio(0.0){ ratio = IntoWaterCount/scinYield; }
    PhotonGroup(G4int a, G4int b):scinYield(a),IntoWaterCount(b){ratio = IntoWaterCount/scinYield;}
    G4int scinYield; //闪烁光子产额
    G4int IntoWaterCount; //穿过水体的光子数
    G4double ratio; //进入水体的光子数占总光产额的比例
};

static constexpr G4double gGountBeginTime = 10.*CLHEP::s; // 统计沉积能量最小起始时刻，若小于该时刻的沉积能量，则不统计
static constexpr G4int gEnChannel = 2000; // 统计沉积能量最小起始时刻，若小于该时刻的沉积能量，则不统计
static constexpr G4double gEnbin = 0.1*keV; // 统计沉积能量道宽
}

#endif