
#include "OpNoviceDetectorConstruction.hh"

#include "G4NistManager.hh"
#include "G4Material.hh"
#include "G4Element.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4RunManager.hh"
#include "G4VisAttributes.hh"
#include "G4Colour.hh"

#include "G4ios.hh"
#include "G4RotationMatrix.hh"
#include "G4LogicalSkinSurface.hh"
#include "G4LogicalBorderSurface.hh"

#include "G4OpticalSurface.hh"
#include "G4SystemOfUnits.hh"
#include "G4SubtractionSolid.hh"
G4double gPMTCathodePosZ = 10.0*cm;  //初始化
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

OpNoviceDetectorConstruction::OpNoviceDetectorConstruction(G4double water_length)
	: fiberGap(1.*mm), 
	fwaterShell_length(water_length)
	{
	// default parameter values of the calorimeter
	// create commands for interactive definition of the calorimeter
	//detectorMessenger = new CsITlDetectorMessenger(this);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

OpNoviceDetectorConstruction::~OpNoviceDetectorConstruction() {
//delete detectorMessenger;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VPhysicalVolume* OpNoviceDetectorConstruction::Construct() {
	G4NistManager* nist = G4NistManager::Instance();

	///////////////////////////////Element//////////////////////////////
	G4Element* el_H = new G4Element("Hydro","H" , 1., 1.01*g/mole);
	G4Element* el_C = new G4Element("Carbon","C" , 6., 12.011*g/mole);
	G4Element* el_O = new G4Element("Oxy","O" , 8., 15.999*g/mole);
    G4Element* el_Si=new G4Element("Silicon", "Si", 14.,  28.0855*g/mole);
    
	///////////////////////////////Material//////////////////////////////
	// Air
	G4Material* air_mat = nist->FindOrBuildMaterial("G4_AIR");  // 0.001204g/cm3

	// Pb
	G4Material* Pb_mat = nist->FindOrBuildMaterial("G4_Pb");  // 11.35g/cm3

	// water
	G4Material* water_mat = new G4Material("H2O_mat", 1.00*g/cm3, 2);
	water_mat->AddElement(el_H, 2);
	water_mat->AddElement(el_O, 1);

	// 聚苯乙烯, 闪烁光纤
	G4Material* polystyrene_mat = new G4Material("polystyrene_mat", 1.06*g/cm3, 2);
	polystyrene_mat->AddElement(el_H, 0.078);
	polystyrene_mat->AddElement(el_C, 0.922);

	// 丙烯酸, 闪烁光纤外壳
	G4Material* opticalShell_mat = new G4Material("opticalShell_mat", 1.0611*g/cm3, 3);
	opticalShell_mat->AddElement(el_H, 0.0556);
	opticalShell_mat->AddElement(el_C, 0.5);
	opticalShell_mat->AddElement(el_O, 0.4444);

	// PMMA ,M1, 原本用作水膜外壳，当前建模不考虑该部分
	G4Material* PMMA_mat = new G4Material("PMMA", 1.19*g/cm3, 3);
	PMMA_mat->AddElement(el_O, 0.319614);
	PMMA_mat->AddElement(el_H, 0.080538);
	PMMA_mat->AddElement(el_C, 0.599848);
	
	//silicone oil
	G4Material* silicone_oil_mat=new G4Material("silicone_oil", 0.963*g/cm3, 4);
	silicone_oil_mat->AddElement(el_C, 2);
	silicone_oil_mat->AddElement(el_H, 6);
	silicone_oil_mat->AddElement(el_Si, 1);
	silicone_oil_mat->AddElement(el_O, 1);

	// PMT material
	//由于PMT仅仅是作为可见光光子的探测器，因此这里材料随便设置，他并不会影响氚的衰变粒子（电子）的输运。
	G4Material* PMT_mat = opticalShell_mat;

//---------------------------------------------- 材料光学性质 ----------------------------------------------//

	const double hc=6.62606876*2.99792458*100./1.602176462;
//
// 闪烁光纤的光学特性 滨松公司闪烁光纤BCF-10
//
	const G4int NUMENTRIES_PS = 30;
	G4double photonEnergy_water_PS[NUMENTRIES_PS] = {
		hc/536.*eV, hc/519.*eV, hc/506.*eV, hc/498.*eV, hc/482.*eV, hc/478.*eV,
		hc/472.*eV, hc/467.*eV, hc/462.*eV, hc/458.*eV, hc/452.*eV, hc/449.*eV,
		hc/445.*eV, hc/443.*eV, hc/441.*eV, hc/438.*eV, hc/435.*eV, hc/430.*eV,
		hc/428.*eV, hc/425.*eV, hc/423.*eV, hc/420.*eV, hc/419.*eV, hc/418.*eV,
		hc/417.*eV, hc/416.*eV, hc/414.*eV, hc/413.*eV, hc/408.*eV, hc/405.*eV
	};
	G4double Scnt_FAST_BCF[NUMENTRIES_PS] = {
		0.00268, 0.03776, 0.05065, 0.09075, 0.16163, 0.21032,
		0.31629, 0.39147, 0.47525, 0.55043, 0.59912, 0.65210,
		0.69578, 0.75807, 0.83755, 0.91703, 0.98290, 0.92562,
		0.85545, 0.80175, 0.75378, 0.67430, 0.56332, 0.49315,
		0.40007, 0.30698, 0.22321, 0.13514, 0.09075, 0.00697
	};
	G4double Scnt_RefractiveIndex_BCF[NUMENTRIES_PS] = {
		1.69584, 1.59884, 1.59884, 1.60611, 1.60611, 1.60611,
		1.60611, 1.60611, 1.60611, 1.60611, 1.60611, 1.60611,
		1.60611, 1.60611, 1.60611, 1.60611, 1.60611, 1.60611,
		1.61741, 1.61741, 1.61741, 1.61741, 1.61741, 1.61741,
		1.62536, 1.62536, 1.62536, 1.62536, 1.62536, 1.62536
	};
	G4double Scnt_AbsorptionLength_BCF[NUMENTRIES_PS] = {
		4.77230*m, 4.77230*m, 4.77230*m, 4.77230*m, 4.78304*m, 4.78304*m,
		4.78304*m, 4.78304*m, 4.78304*m, 4.78304*m, 4.78304*m, 4.78304*m,
		4.78304*m, 4.78304*m, 4.78304*m, 4.78304*m, 4.78304*m, 4.78304*m,
		4.78304*m, 4.78304*m, 4.78304*m, 4.78304*m, 4.78304*m, 4.78304*m,
		4.62907*m, 4.62907*m, 4.62907*m, 4.62907*m, 4.62907*m, 4.62907*m
	};

	G4MaterialPropertiesTable* scnt_PS_MPT = new G4MaterialPropertiesTable();
	scnt_PS_MPT->AddProperty("FASTCOMPONENT", photonEnergy_water_PS, Scnt_FAST_BCF, NUMENTRIES_PS)->SetSpline(true);
	scnt_PS_MPT->AddProperty("RINDEX", photonEnergy_water_PS, Scnt_RefractiveIndex_BCF, NUMENTRIES_PS)->SetSpline(true);
	scnt_PS_MPT->AddProperty("ABSLENGTH", photonEnergy_water_PS, Scnt_AbsorptionLength_BCF, NUMENTRIES_PS)->SetSpline(true);
	scnt_PS_MPT->AddConstProperty("SCINTILLATIONYIELD",5000./MeV); //
	scnt_PS_MPT->AddConstProperty("RESOLUTIONSCALE", 1.0);
	scnt_PS_MPT->AddConstProperty("FASTTIMECONSTANT", 2.7*ns);
	scnt_PS_MPT->AddConstProperty("SLOWTIMECONSTANT", 2.7*ns);
	scnt_PS_MPT->AddConstProperty("YIELDRATIO", 1.0);    //快成分占总发光量的份额
	polystyrene_mat->SetMaterialPropertiesTable(scnt_PS_MPT);

//
//  silicone oil 的光学特性
//
// 硅油之后需要寻找合适的参数，现在暂时用着该参数，该参数来自于清华天格计划
// 目前硅油的吸收长度直接用的水的参数，这个后续需要修正
	const G4int NUMENTRIES = 11;
	G4double Scnt_PP_GAGG[NUMENTRIES] = {hc/540.*eV, hc/530.*eV, hc/520.*eV,hc/510.*eV,hc/495.*eV, hc/475.*eV, hc/460.*eV, hc/445.*eV, hc/430.*eV, hc/415.*eV, hc/400.*eV };
	G4double RIndex_sil_oil[NUMENTRIES] = { 1.406, 1.406, 1.406, 1.406, 1.406, 1.406, 1.406, 1.406, 1.406, 1.406, 1.406 }; //{ 1.58, 1.58, 1.58 };//
	G4double AbsorptionLength_oil[NUMENTRIES] = { 4.77230*m, 4.77230*m, 4.77230*m, 4.77230*m, 4.78304*m, 4.78304*m,
		4.78304*m, 4.78304*m, 4.78304*m, 4.78304*m, 4.78304*m};

	G4MaterialPropertiesTable* Scnt_MPT_silicone_oil = new G4MaterialPropertiesTable();
	Scnt_MPT_silicone_oil->AddProperty("RINDEX", Scnt_PP_GAGG, RIndex_sil_oil, NUMENTRIES);
	Scnt_MPT_silicone_oil->AddProperty("ABSLENGTH", Scnt_PP_GAGG, AbsorptionLength_oil, NUMENTRIES)->SetSpline(true);
	silicone_oil_mat->SetMaterialPropertiesTable(Scnt_MPT_silicone_oil);

//
// PMT的光学特性,主要用于侧面的光学性质，让他对光子全吸收但又没有光电子的转换
//	
    G4MaterialPropertiesTable* PMT_MP_Volume = new G4MaterialPropertiesTable();
    const G4int Energy_PMT = 7;
    //~ G4double EphotonPMTVolume[Energy_PMT] = { hc/547.*eV, hc/528.*eV, hc/505.*eV, hc/473.*eV, hc/439.*eV, hc/382.*eV};  
    //~ G4double EfficiencyPMTVolume[Energy_PMT]    = {0.042, 0.0654, 0.093, 0.136, 0.184, 0.246}; // PMT表面的光电转换效率
    G4double EphotonPMTVolume[Energy_PMT] =  { hc/536.*eV, hc/500.*eV,  hc/475.*eV,  hc/451.*eV,  hc/430.*eV, hc/411.*eV, hc/387.*eV};
    G4double EfficiencyPMTVolume[Energy_PMT] = { 15.83*0.01,  15.83*0.01,  18.78*0.01,  21.58*0.01,  24.40*0.01, 25.54*0.01, 24.40*0.01}; //量子效率
    G4double ReflectivityPMTVolume[Energy_PMT] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};  // PMT表面的反射率 1-吸收率
    
	PMT_MP_Volume->AddProperty("REFLECTIVITY",  EphotonPMTVolume, ReflectivityPMTVolume, Energy_PMT);
	PMT_MP_Volume->AddProperty("EFFICIENCY"  ,  EphotonPMTVolume, EfficiencyPMTVolume  ,  Energy_PMT);
    PMT_mat->SetMaterialPropertiesTable(PMT_MP_Volume);

//
// PMMA的光学特性
//	
   G4MaterialPropertiesTable* PMMA_MP = new G4MaterialPropertiesTable();    
   const G4int NUMENTRIES_PMMA = 8;
   G4double PMMA_Energy[NUMENTRIES_PMMA] = { hc/594.*eV, hc/536.*eV, hc/486.*eV, hc/470.*eV, hc/449.*eV, hc/427.*eV, hc/408.*eV, hc/392.*eV};
   G4double PMMA_RIndex[NUMENTRIES_PMMA] = {    1.49208,    1.49472,    1.49834,    1.49938,    1.50133,    1.50431,    1.50661,    1.50925};
   G4double PMMA_Absoption[NUMENTRIES_PMMA] = {4.43213*m, 4.75976*m,  4.80989*m,   4.7723*m,   4.7723*m,   4.7723*m,  4.75976*m,  4.75976*m};
   PMMA_MP->AddProperty("RINDEX",    PMMA_Energy,    PMMA_RIndex,  NUMENTRIES_PMMA);
   PMMA_MP->AddProperty("ABSLENGTH", PMMA_Energy, PMMA_Absoption,  NUMENTRIES_PMMA);
   PMMA_mat->SetMaterialPropertiesTable(PMMA_MP);
   
//
// Water的光学特性
//
	G4double photonEnergy_water[] = {
		2.034*eV, 2.068*eV, 2.103*eV, 2.139*eV,
		2.177*eV, 2.216*eV, 2.256*eV, 2.298*eV,
		2.341*eV, 2.386*eV, 2.433*eV, 2.481*eV,
		2.532*eV, 2.585*eV, 2.640*eV, 2.697*eV,
		2.757*eV, 2.820*eV, 2.885*eV, 2.954*eV,
		3.026*eV, 3.102*eV, 3.181*eV, 3.265*eV,
		3.353*eV, 3.446*eV, 3.545*eV, 3.649*eV,
		3.760*eV, 3.877*eV, 4.002*eV, 4.136*eV
	};

	const G4int nEntries = sizeof(photonEnergy_water)/sizeof(G4double);

	G4double waterReIndex[] = {
		1.3435, 1.344,  1.3445, 1.345,  1.3455,
		1.346,  1.3465, 1.347,  1.3475, 1.348,
		1.3485, 1.3492, 1.35,   1.3505, 1.351,
		1.3518, 1.3522, 1.3530, 1.3535, 1.354,
		1.3545, 1.355,  1.3555, 1.356,  1.3568,
		1.3572, 1.358,  1.3585, 1.359,  1.3595,
		1.36,   1.3608
	};

	assert(sizeof(waterReIndex) == sizeof(photonEnergy_water));

	G4double absorption[] = {
		3.448*m,  4.082*m,  6.329*m,  9.174*m, 12.346*m, 13.889*m,
		15.152*m, 17.241*m, 18.868*m, 20.000*m, 26.316*m, 35.714*m,
		45.455*m, 47.619*m, 52.632*m, 52.632*m, 55.556*m, 52.632*m,
		52.632*m, 47.619*m, 45.455*m, 41.667*m, 37.037*m, 33.333*m,
		30.000*m, 28.500*m, 27.000*m, 24.500*m, 22.000*m, 19.500*m,
		17.500*m, 14.500*m
	};

	assert(sizeof(absorption) == sizeof(photonEnergy_water));


	G4MaterialPropertiesTable* myMPT1 = new G4MaterialPropertiesTable();

	myMPT1->AddProperty("RINDEX",       photonEnergy_water, waterReIndex,nEntries)
	->SetSpline(true);
	myMPT1->AddProperty("ABSLENGTH",    photonEnergy_water, absorption,     nEntries)
	->SetSpline(true);
	
	G4double energy_water[] = {
		1.56962*eV, 1.58974*eV, 1.61039*eV, 1.63157*eV,
		1.65333*eV, 1.67567*eV, 1.69863*eV, 1.72222*eV,
		1.74647*eV, 1.77142*eV, 1.7971 *eV, 1.82352*eV,
		1.85074*eV, 1.87878*eV, 1.90769*eV, 1.93749*eV,
		1.96825*eV, 1.99999*eV, 2.03278*eV, 2.06666*eV,
		2.10169*eV, 2.13793*eV, 2.17543*eV, 2.21428*eV,
		2.25454*eV, 2.29629*eV, 2.33962*eV, 2.38461*eV,
		2.43137*eV, 2.47999*eV, 2.53061*eV, 2.58333*eV,
		2.63829*eV, 2.69565*eV, 2.75555*eV, 2.81817*eV,
		2.88371*eV, 2.95237*eV, 3.02438*eV, 3.09999*eV,
		3.17948*eV, 3.26315*eV, 3.35134*eV, 3.44444*eV,
		3.54285*eV, 3.64705*eV, 3.75757*eV, 3.87499*eV,
		3.99999*eV, 4.13332*eV, 4.27585*eV, 4.42856*eV,
		4.59258*eV, 4.76922*eV, 4.95999*eV, 5.16665*eV,
		5.39129*eV, 5.63635*eV, 5.90475*eV, 6.19998*eV
	};

	const G4int numentries_water = sizeof(energy_water)/sizeof(G4double);

	//assume 100 times larger than the rayleigh scattering for now.
	G4double mie_water[] = {
		167024.4*m, 158726.7*m, 150742  *m,
		143062.5*m, 135680.2*m, 128587.4*m,
		121776.3*m, 115239.5*m, 108969.5*m,
		102958.8*m, 97200.35*m, 91686.86*m,
		86411.33*m, 81366.79*m, 76546.42*m,
		71943.46*m, 67551.29*m, 63363.36*m,
		59373.25*m, 55574.61*m, 51961.24*m,
		48527.00*m, 45265.87*m, 42171.94*m,
		39239.39*m, 36462.50*m, 33835.68*m,
		31353.41*m, 29010.30*m, 26801.03*m,
		24720.42*m, 22763.36*m, 20924.88*m,
		19200.07*m, 17584.16*m, 16072.45*m,
		14660.38*m, 13343.46*m, 12117.33*m,
		10977.70*m, 9920.416*m, 8941.407*m,
		8036.711*m, 7202.470*m, 6434.927*m,
		5730.429*m, 5085.425*m, 4496.467*m,
		3960.210*m, 3473.413*m, 3032.937*m,
		2635.746*m, 2278.907*m, 1959.588*m,
		1675.064*m, 1422.710*m, 1200.004*m,
		1004.528*m, 833.9666*m, 686.1063*m
	};

	assert(sizeof(mie_water) == sizeof(energy_water));

	// gforward, gbackward, forward backward ratio
	G4double mie_water_const[3]= {0.99,0.99,0.8};

	myMPT1->AddProperty("MIEHG",energy_water,mie_water,numentries_water)
	->SetSpline(true);
	myMPT1->AddConstProperty("MIEHG_FORWARD",mie_water_const[0]);
	myMPT1->AddConstProperty("MIEHG_BACKWARD",mie_water_const[1]);
	myMPT1->AddConstProperty("MIEHG_FORWARD_RATIO",mie_water_const[2]);

	G4cout << "Water G4MaterialPropertiesTable" << G4endl;
	myMPT1->DumpTable();

	water_mat->SetMaterialPropertiesTable(myMPT1);

	// Set the Birks Constant for the Water scintillator
	//water->GetIonisation()->SetBirksConstant(0.126*mm/MeV);

//
// Air
//
    const G4int EnergyNumAIR = 2;
    G4double photonEnergyAir[2]  = { hc/540.*eV, hc/400.*eV}; 
	G4double refractiveIndexAir[2] = {1.00, 1.00,};

	G4MaterialPropertiesTable* myMPT_Air = new G4MaterialPropertiesTable();
	myMPT_Air->AddProperty("RINDEX", photonEnergyAir, refractiveIndexAir, EnergyNumAIR);

	G4cout << "Air G4MaterialPropertiesTable" << G4endl;
	myMPT_Air->DumpTable();

	air_mat->SetMaterialPropertiesTable(myMPT_Air);

//
// PTFE Add ,水壳内部采用特氟龙，外部包裹Pb皮
//

   const G4int NUMENTRIES_PTFE = 2;
   G4double ptfe_PP[NUMENTRIES_PTFE] = {hc/540.*eV, hc/400.*eV };
   G4double ptfe_RefractiveIndex[NUMENTRIES_PTFE] = { 1, 1 };
   G4double ptfe_AbsorptionLength[NUMENTRIES_PTFE] = { 2*um, 2*um };
   G4MaterialPropertiesTable* PTFE_MPT = new G4MaterialPropertiesTable();
   PTFE_MPT->AddProperty("RINDEX", ptfe_PP, ptfe_RefractiveIndex, NUMENTRIES_PTFE);
   PTFE_MPT->AddProperty("ABSLENGTH", ptfe_PP, ptfe_AbsorptionLength, NUMENTRIES_PTFE);
   Pb_mat->SetMaterialPropertiesTable(PTFE_MPT);

//---------------------------------------------- World ------------------------------------------------//
	// World
	//
	G4double   world_xy = 500.0*cm ;//
	G4double   world_z = 500.0*cm ;//mm
	G4Box* solidWorld = new G4Box( "World", world_xy, world_xy,world_z);
	G4LogicalVolume* logicWorld = new G4LogicalVolume( solidWorld, air_mat ,"logicWorld");
	G4PVPlacement* physWorld =
	    new G4PVPlacement( 0, G4ThreeVector( ),logicWorld,"physWorld", 0, false, 0, true ) ;
	logicWorld->SetVisAttributes(G4VisAttributes::Invisible);

//---------------------------------------------- 水体外壳 ------------------------------------------------//
	//
	// 水体外壳(容器) ,采用铅作屏蔽，目的在于降低本底信号
	//
	G4double   water_radius = 2.5*cm ;                 // 水体的半径 2.8*cm
	G4double   waterShell_thickness = 0.15*cm ;    // 外壳厚度
	G4double   waterShell_radius = waterShell_thickness + water_radius;    // 外壳半径
	//~ G4double   fwaterShell_length   = 40.0*cm ;
    G4double   PMMA_thickness = 1.5*cm;
    
	G4Tubs* solidWaterShell = new G4Tubs("solidWaterShell", water_radius, waterShell_radius, fwaterShell_length*0.5, 0, 360.0*deg);
	G4LogicalVolume* logicWaterShell = new G4LogicalVolume( solidWaterShell, Pb_mat ,"logicWaterShell");
	G4PVPlacement* physWaterShell =
	    new G4PVPlacement( 0, G4ThreeVector( ), logicWaterShell, "physWaterShell", logicWorld, false, 0, true ) ;

//---------------------------------------------- 水体部分 ------------------------------------------------//
    const G4int c_colums = 19;   
    G4int colum[c_colums] = {7, 11, 13, 15, 17, 17, 19, 19, 19, 19, 19, 19, 19, 17, 17, 15, 13, 11, 7};  // 每一列中对应的光纤数目，从左往右数起
    G4int myCount = 0;
	for(int i=0; i<19; i++) myCount += colum[i];
	G4cout<<"myCount = "<<myCount<<G4endl;
	// exit(0);
	// BCF 含有极少量氚的水，由于含量极少，不影响粒子输运，这里不考虑氚
	// 当电子能量超过263keV时,在水中就能产生切伦科夫辐射
	G4double optical_Ridus = 0.75*mm; // 光纤半径	0.75*mm
	G4double optical_Length = PMMA_thickness*2 + fwaterShell_length;  // 光纤长度
    
	G4Tubs* solidWater = new G4Tubs("solidWater", 0.0, water_radius, fwaterShell_length*0.5, 0, 360.0*deg);
	G4Tubs* solidoptical = new G4Tubs("solidoptical", 0.0, optical_Ridus, optical_Length*0.5, 0, 360.0*deg);
	G4SubtractionSolid* pSolidCut(NULL);

	fiberGap = 1.*mm;                           // 两根光纤外壳之间的距离,而非轴心距离
	G4double fiberDistance = fiberGap + optical_Ridus*2;

	///由于光纤的长度超出了水体，因此水体部分采用布尔逻辑扣除光纤的位置，留下空位放置光纤
	G4double cell_x = -9.*fiberDistance;  // 第零列坐标位置
	for(int i=0; i<c_colums; i++) {
        G4int Line = colum[i];
		G4double cell_y = -(Line-1)*0.5*fiberDistance;  // 第零行坐标位置
		for(int j=0; j<Line; j++) {
			G4ThreeVector zTrans(cell_x, cell_y, 0.0);
			if(pSolidCut) {
				pSolidCut = new  G4SubtractionSolid(" subtraction ", pSolidCut, solidoptical, 0, zTrans);
			} 
            else {
				pSolidCut = new  G4SubtractionSolid(" subtraction ", solidWater, solidoptical, 0, zTrans);
			}
			cell_y += fiberDistance;
		}
		cell_x += fiberDistance;
	}
    
	G4LogicalVolume* logicWater = new G4LogicalVolume( pSolidCut, water_mat ,"logicWater");
	G4PVPlacement* physWater =
	    new G4PVPlacement( 0, G4ThreeVector( ), logicWater, "physWater", logicWorld, false, 0, true ) ;              // Place physWater

//---------------------------Scintillation OpticalFiber without opticalShell----------------------------//
	G4LogicalVolume* logicOpticaltube = new G4LogicalVolume(solidoptical, polystyrene_mat,"logicOpticaltube");

	G4PVPlacement* physOpticalTube[293]; // 请注意这个参数是由colum[17]中所有元素相加得来的。
	cell_x = -9.*fiberDistance;          // 第零列坐标位置
    G4int tubeID = 0;                    // 摆放时每个物理体编号
	for(int i=0; i<c_colums; i++) {
        G4int Line = colum[i];
		G4double cell_y = -(Line-1)*0.5*fiberDistance; // 第零行坐标位置
            
		for(int j=0; j<Line; j++) {
			G4String name = GetphyOpticaltubeName(i, j);
			physOpticalTube[tubeID] = new G4PVPlacement(0,G4ThreeVector(cell_x, cell_y, 0.), logicOpticaltube, name, logicWorld, false, i*5+j, true);      
			cell_y += fiberDistance;
            tubeID++;
		}
		cell_x += fiberDistance;
	}

//---------------------------------------------- PMMA -------------------------------------------//

	G4double PMMA_Radius = waterShell_radius;
	// G4double PMMA_thickness = 1.5*cm;
	G4Tubs* PMMA_box = new G4Tubs("si_oil_box", 0.0, PMMA_Radius, PMMA_thickness*0.5,0.0*deg, 360.0*deg);  // 与PMT的直径相同，长度不同
	G4SubtractionSolid* PMMA_SolidCut(NULL);
	
	///由于光纤穿过PMMA，因此PMMA部分采用布尔逻辑扣除光纤的位置，留下空位放置光纤
	cell_x = -9.*fiberDistance;  // 第零列坐标位置
	for(int i=0; i<c_colums; i++) {
        G4int Line = colum[i];
		G4double cell_y = -(Line-1)*0.5*fiberDistance;  // 第零行坐标位置
        
		for(int j=0; j<Line; j++) {
			G4ThreeVector zTrans(cell_x, cell_y, 0.0);
			if(PMMA_SolidCut) {
				PMMA_SolidCut = new  G4SubtractionSolid("subtraction", PMMA_SolidCut, solidoptical, 0, zTrans);
			} 
            else {
				PMMA_SolidCut = new  G4SubtractionSolid("subtraction", PMMA_box, solidoptical, 0, zTrans);
			}
			cell_y += fiberDistance;
		}
		cell_x += fiberDistance;
	}

    G4LogicalVolume* PMMA_log = new G4LogicalVolume(PMMA_SolidCut, PMMA_mat, "PMMA_log");
    
    G4double PMMA_centerZ = (fwaterShell_length+PMMA_thickness)*0.5;    // PMMA中心与原点之间的距离
	G4PVPlacement* physPMMA_1 =
	    new G4PVPlacement(0, G4ThreeVector(0., 0.,  PMMA_centerZ), PMMA_log, "physPMMA_1", logicWorld, false, 0, true ) ;              // Place physPMMA_1
    G4PVPlacement* physPMMA_2 =
	    new G4PVPlacement(0, G4ThreeVector(0., 0., -PMMA_centerZ), PMMA_log, "physPMMA_2", logicWorld, false, 1, true ) ;            // Place physPMMA_2
    
//--------------------------------------------si_oil_box -----------------------------------------//
	G4double pmtRadius = 2.5*cm;
	G4double si_oilRadius = pmtRadius;
	G4double si_oil_thickness = 0.05*mm;

	G4Tubs* si_oil_box = new G4Tubs("si_oil_box", 0.0, si_oilRadius, si_oil_thickness*0.5,0.0*deg, 360.0*deg);  // 与PMT的直径相同，长度不同
	G4LogicalVolume* si_oil_log = new G4LogicalVolume(si_oil_box,silicone_oil_mat,"si_oil_log");

    G4double si_oil_centerZ = (fwaterShell_length+si_oil_thickness)*0.5+PMMA_thickness;    // si_oil中心与原点之间的距离
	G4VPhysicalVolume* physSil_oil_1 =
	    new G4PVPlacement(0, G4ThreeVector(0., 0.,  si_oil_centerZ), si_oil_log,"physSil_oil_1",logicWorld,false,0, true);
	G4VPhysicalVolume* physSil_oil_2 =
	    new G4PVPlacement(0, G4ThreeVector(0., 0., -si_oil_centerZ), si_oil_log,"physSil_oil_2",logicWorld,false,1, true);

//------------------------------------------------PMT -----------------------------------//
	//G4double pmtRadius = 2.8*cm;
    G4double PMTlenth = pmtRadius*2;
	G4Tubs* solidPMTGlass = new G4Tubs("PMTGlass", 0.0, pmtRadius, PMTlenth*0.5,0.0*deg, 360.0*deg);  // PMT的长度不重要，所以这里随便设置
	G4LogicalVolume* logicPMTGlass =  new G4LogicalVolume(solidPMTGlass, PMT_mat, "logicPMTGlass");

    G4double PMT_centerZ = (fwaterShell_length+PMTlenth)*0.5+si_oil_thickness+PMMA_thickness;    // si_oil中心与原点之间的距离
	G4PVPlacement* physiPMTGlass1 =
	    new G4PVPlacement(0, G4ThreeVector(0., 0.,  PMT_centerZ), logicPMTGlass, "physiPMTGlass1", logicWorld, false, 0, true ) ;
	G4PVPlacement* physiPMTGlass2 =
	    new G4PVPlacement(0, G4ThreeVector(0., 0., -PMT_centerZ), logicPMTGlass, "physiPMTGlass2", logicWorld, false, 1, true ) ;
    gPMTCathodePosZ = PMT_centerZ-PMTlenth*0.5;
//**********************************************surface property********************************************************
// Boundary Definition
// More gudaince can be seen from G4OpticalSurface.hh  G4SurfaceProperty.hh
          
//------------------------------------opticalTube & siliconeOil-----------------------------------//
  
	G4OpticalSurface* OpSurface_sct_oil = new G4OpticalSurface("OpSurface_sct_oil");
	OpSurface_sct_oil->SetType(dielectric_dielectric);
	OpSurface_sct_oil->SetModel(unified);
	OpSurface_sct_oil->SetFinish(polished); // polished surface with optical grease (for DAVIS model)
        
    tubeID = 0;    // 光纤的物理体编号
	for(int i=0; i<c_colums; i++) {
        G4int Line = colum[i];
		for(int j=0; j<Line; j++) {
			G4String strphysA = GetphyOpticaltubeName(i, j);
			G4String strphysB1 = "oilA";
			G4String strphysB2 = "oilB";

			G4String surfaceName1 = GetSurfaceName(strphysA,strphysB1);
			G4String surfaceName2 = GetSurfaceName(strphysA,strphysB2);
			//~ Surface_sct_OilA_LBS[i][j] = 
			new G4LogicalBorderSurface(surfaceName1, physOpticalTube[tubeID], physSil_oil_1, OpSurface_sct_oil);
			//~ Surface_sct_OilB_LBS[i][j] = 
			new G4LogicalBorderSurface(surfaceName2, physOpticalTube[tubeID], physSil_oil_2, OpSurface_sct_oil);

			surfaceName1 = GetSurfaceName(strphysB1,strphysA);
			surfaceName2 = GetSurfaceName(strphysB2,strphysA);
			//~ Surface_OilA_sct_LBS[i][j] = 
			new G4LogicalBorderSurface(surfaceName1, physSil_oil_1, physOpticalTube[tubeID], OpSurface_sct_oil);
			//~ Surface_OilB_sct_LBS[i][j] = 
			new G4LogicalBorderSurface(surfaceName2, physSil_oil_2, physOpticalTube[tubeID], OpSurface_sct_oil);
            tubeID++;
		}
	}

//If you neglect to specify one of the two, then the neglected direction will simply be a perfectly smooth dielectric_dielectric interface
//with only the index of refraction specified on both sides

//-----------------------------------------------opticalTube & water-----------------------------------//
	G4OpticalSurface* OpSurface_sct_water = new G4OpticalSurface("OpSurface_sct_water");
	OpSurface_sct_water->SetType(dielectric_dielectric);
	OpSurface_sct_water->SetModel(unified);
	OpSurface_sct_water->SetFinish(polished); 

	tubeID = 0;    // 光纤的物理体编号
	for(int i=0; i<c_colums; i++) {
        G4int Line = colum[i];
		for(int j=0; j<Line; j++) {
			G4String strphysA = GetphyOpticaltubeName(i, j);
			G4String strphysB = "Water";
			G4String surfaceName1 = GetSurfaceName(strphysA,strphysB);
			G4String surfaceName2 = GetSurfaceName(strphysB,strphysA);
			
			new G4LogicalBorderSurface(surfaceName1, physOpticalTube[tubeID], physWater, OpSurface_sct_water);
			new G4LogicalBorderSurface(surfaceName2, physWater, physOpticalTube[tubeID], OpSurface_sct_water);
            tubeID++;
		}
	}

//-----------------------------------------------opticalTube & PMMA-----------------------------------//

	G4OpticalSurface* OpSurface_sct_PMMA = new G4OpticalSurface("OpSurface_sct_PMMA");
	//-----------------------------------------------------------------------------------------
    OpSurface_sct_PMMA->SetType(dielectric_LUTDAVIS);   // dielectric-Look-Up-Table DAVIS interface 
	OpSurface_sct_PMMA->SetModel(DAVIS);
	OpSurface_sct_PMMA->SetFinish(PolishedTeflon_LUT);  // polished surface wrapped in Teflon tape,  can be used under DAVIS model
	
    tubeID = 0;    // 光纤的物理体编号
	for(int i=0; i<c_colums; i++) {
        G4int Line = colum[i];
		for(int j=0; j<Line; j++) {
			G4String strphysA = GetphyOpticaltubeName(i, j);
			G4String strphysB = "PMMA1";
			G4String surfaceName1 = GetSurfaceName(strphysA,strphysB);
			G4String surfaceName2 = GetSurfaceName(strphysB,strphysA);
			
			new G4LogicalBorderSurface(surfaceName1, physOpticalTube[tubeID], physPMMA_1, OpSurface_sct_PMMA);
			new G4LogicalBorderSurface(surfaceName2, physPMMA_1, physOpticalTube[tubeID], OpSurface_sct_PMMA);
			
			strphysB = "PMMA2";
			surfaceName1 = GetSurfaceName(strphysA,strphysB);
			surfaceName2 = GetSurfaceName(strphysB,strphysA);
			new G4LogicalBorderSurface(surfaceName1, physOpticalTube[tubeID], physPMMA_2, OpSurface_sct_PMMA);
			new G4LogicalBorderSurface(surfaceName2, physPMMA_2, physOpticalTube[tubeID], OpSurface_sct_PMMA);
            tubeID++;
		}
	}

//-----------------------------------------------water & WaterShell-----------------------------------//
	//表面为特氟龙，抛光处理
	G4OpticalSurface* OpSurface_water_waterShell = new G4OpticalSurface("OpSurface_water_waterShell");
	OpSurface_water_waterShell ->SetType(dielectric_LUTDAVIS);   // dielectric-Look-Up-Table DAVIS interface 
	OpSurface_water_waterShell ->SetModel(DAVIS);
	OpSurface_water_waterShell ->SetFinish(PolishedTeflon_LUT);  // polished surface wrapped in Teflon tape,  can be used under DAVIS model

	new G4LogicalBorderSurface("Surface_water_waterShell_LBS", physWater, physWaterShell,OpSurface_water_waterShell);
    new G4LogicalBorderSurface("Surface_waterShell_water_LBS", physWaterShell, physWater,OpSurface_water_waterShell);
    
//-----------------------------------------------water & PMMA-----------------------------------//

    G4OpticalSurface* OpSurface_water_PMMA = new G4OpticalSurface("OpSurface_water_PMMA");
	OpSurface_water_PMMA->SetType(dielectric_dielectric);
	OpSurface_water_PMMA->SetModel(unified);
	OpSurface_water_PMMA->SetFinish(polished); 
    
    new G4LogicalBorderSurface("Surface_water_PMMA1_LBS", physWater, physPMMA_1,OpSurface_water_PMMA);
    new G4LogicalBorderSurface("Surface_PMMA1_water_LBS", physPMMA_1, physWater,OpSurface_water_PMMA);
    new G4LogicalBorderSurface("Surface_water_PMMA2_LBS", physWater, physPMMA_2,OpSurface_water_PMMA);
    new G4LogicalBorderSurface("Surface_PMMA2_water_LBS", physPMMA_2, physWater,OpSurface_water_PMMA);
 
 //-----------------------------------------------PMMA & silicone oil-----------------------------------//

    G4OpticalSurface* OpSurface_PMMA_oil = new G4OpticalSurface("OpSurface_PMMA_oil");
	OpSurface_PMMA_oil->SetType(dielectric_dielectric);
	OpSurface_PMMA_oil->SetModel(unified);
	OpSurface_PMMA_oil->SetFinish(polished); 
	
    new G4LogicalBorderSurface("Surface_PMMA1_oil_LBS", physPMMA_1, physSil_oil_1,OpSurface_PMMA_oil);
    new G4LogicalBorderSurface("Surface_oil_PMMA1_LBS", physSil_oil_1, physPMMA_1,OpSurface_PMMA_oil);
    new G4LogicalBorderSurface("Surface_PMMA2_oilr_LBS", physPMMA_2, physSil_oil_2,OpSurface_PMMA_oil);
    new G4LogicalBorderSurface("Surface_oil_PMMA2_LBS", physSil_oil_2, physPMMA_2,OpSurface_PMMA_oil);
 
  //-----------------------------------------------PMMA & Air 光导四周特氟龙反射模-----------------------------------//
    G4OpticalSurface* OpSurface_PMMA_Air = new G4OpticalSurface("OpSurface_PMMA_Air");
	OpSurface_PMMA_Air->SetType(dielectric_LUTDAVIS);
	OpSurface_PMMA_Air->SetModel(DAVIS);
	OpSurface_PMMA_Air->SetFinish(PolishedTeflon_LUT); // polished surface wrapped in Teflon tape
	
    new G4LogicalBorderSurface("Surface_PMMA1_Air_LBS", physPMMA_1, physWorld,OpSurface_PMMA_Air);
    new G4LogicalBorderSurface("Surface_Air_PMMA1_LBS", physWorld, physPMMA_1,OpSurface_PMMA_Air);
    new G4LogicalBorderSurface("Surface_PMMA2_Air_LBS", physPMMA_2, physWorld,OpSurface_PMMA_Air);
    new G4LogicalBorderSurface("Surface_Air_PMMA2_LBS", physWorld, physPMMA_2,OpSurface_PMMA_Air);
    
//-----------------------------------------------silicone oil & PMT-----------------------------------//
	G4OpticalSurface*  OpOilPMTSurface = new G4OpticalSurface("OpScntPMTSurface");
	OpOilPMTSurface->SetType(dielectric_metal);
	OpOilPMTSurface->SetFinish(polished);
	OpOilPMTSurface->SetModel(glisur);

	G4MaterialPropertiesTable* PMT_MaterialProperties = new G4MaterialPropertiesTable();
    const G4int numentries_PMT = 7;
    //~ G4double EphotonPMT[numentries_PMT] = { hc/547.14008*eV,  hc/528.70139*eV,  hc/505.71983*eV,  hc/473.38531*eV, hc/439.44743*eV, hc/382.79521*eV};
    //~ G4double Efficiency[numentries_PMT]       = { 4.23985*0.01   ,  6.53938*0.01   ,  9.31229*0.01   ,  13.62893*0.01  , 18.41625*0.01  , 24.60302*0.01  }; //量子效率
    G4double EphotonPMT[numentries_PMT] = { hc/536.*eV, hc/500.*eV,  hc/475.*eV,  hc/451.*eV,  hc/430.*eV, hc/411.*eV, hc/387.*eV};
    G4double Efficiency[numentries_PMT]       = {  15.83*0.01,  15.83*0.01,  18.78*0.01,   21.58*0.01,    24.40*0.01,  25.54*0.01, 24.40*0.01}; //量子效率
    G4double reflectivity = 0.02; // PMT表面的反射率
    G4double Reflectivity[numentries_PMT] = { reflectivity   ,  reflectivity   ,  reflectivity   ,  reflectivity   , reflectivity   , reflectivity, reflectivity};
	PMT_MaterialProperties->AddProperty("REFLECTIVITY", EphotonPMT, Reflectivity, numentries_PMT);
	PMT_MaterialProperties->AddProperty("EFFICIENCY"  , EphotonPMT, Efficiency, numentries_PMT);

	OpOilPMTSurface->SetMaterialPropertiesTable(PMT_MaterialProperties);
	
	G4String surfaceName = GetSurfaceName("oilA", "PMT1");
	new G4LogicalBorderSurface(surfaceName, physSil_oil_1, physiPMTGlass1, OpOilPMTSurface); //光子只能其他介质进到金属，没有从金属到电介质
	surfaceName = GetSurfaceName("oilB", "PMT2");
	new G4LogicalBorderSurface(surfaceName, physSil_oil_2, physiPMTGlass2, OpOilPMTSurface);

	return physWorld;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4String OpNoviceDetectorConstruction::GetphyOpticalShellName(G4int i, G4int j) {
	std::ostringstream os;
	os << "phyOpticalFibershell_";
	os << i ;
	os << "_" ;
	os << j ;
	G4String name = os.str();
	return name;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4String OpNoviceDetectorConstruction::GetphyOpticaltubeName(G4int i, G4int j) {
	std::ostringstream os;
	os << "phyOpticalFiber_";
	os << i ;
	os << "_" ;
	os << j ;
	G4String name = os.str();
	return name;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4String OpNoviceDetectorConstruction::GetSurfaceName(G4String phycialNameA, G4String phycialNameB) {
	std::ostringstream os;
	os << "Surface_";
	os << phycialNameA ;
	os << "_" ;
	os << phycialNameB ;
	G4String name = os.str();
	return name;
}
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
