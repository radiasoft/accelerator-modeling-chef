// 
#include <vector> 
#include <SteerAnalysis.h>

Steering::MonitData::MonitData():BPM(999999), z(0.), y(0.), yp(0.), 
                       epsilY(0.), epsilNDY(0.), epsilYM(0.), 
		       epsilNDYM(0.), Disp(0.), DispP(0.), BpmYOff(0.), 
		       QuadOff(0.), DipVal(0.), final_y(0.),
		       final_epsilY(0.), final_epsilNDY(0.), final_epsilYM(0.), 
		       final_epsilNDYM(0.) {;}

Steering::DFSRun::DFSRun(const std::string &fDirLabel, 
               int macroIterSel, int fDipole, bool finalPulse):
label("D"), jobNum(0), procNum(0), macroIter(macroIterSel), firstDipole(fDipole), 
           finalResult(finalPulse), data(0) 

{

// Should be static somewhere.. oh well. 
  char* ilcTestDataChr = getenv("ILCLINACTESTDATA");
  std::string defDataDir("./data");
  if (ilcTestDataChr == 0) ilcTestDataChr = (char *) defDataDir.c_str();    
  std::string topDir(ilcTestDataChr);
  topDir += std::string("/Test4/");
  
  size_t iUnderLast = fDirLabel.rfind("_");
  if (iUnderLast == std::string::npos) {
    std::cerr << 
     " SteerAnalysis, DFSRun::DFSRun, expect at least one _ in directory "  << std::endl;
     return;
  }
  size_t iUnderFirst = fDirLabel.find("_");
  if (iUnderFirst != iUnderLast) { 
    std::string jobNumStr(fDirLabel, iUnderFirst+1); 
    std::istringstream jobNumStrStr(jobNumStr); jobNumStrStr >> jobNum;
  }
  label = std::string(fDirLabel, 0, iUnderLast); 
  std::string procNumStr(fDirLabel, iUnderLast+1); 
  std::istringstream procNumStrStr(procNumStr); procNumStrStr >> procNum;
  
  std::ostringstream fNameStrStr; fNameStrStr << topDir << "StPf_" << fDirLabel << "_" 
                                              << label  << "_g" <<  macroIter;
  if (finalPulse) { 
    fNameStrStr  << "final.txt";
  } else {
    fNameStrStr  << "_d" << firstDipole << "StPf_OK.txt";
  }  
  
  std::string fNameStr = fNameStrStr.str();		      
  this->fillFromFile(fNameStr, finalPulse);
  
}
Steering::DFSRun::DFSRun(const std::string &fNameStr, bool finalPulse):
label("D"), jobNum(0), procNum(0), macroIter(0), firstDipole(0), 
           finalResult(finalPulse), data(0) 

{

  this->fillFromFile(fNameStr, finalPulse);

}

void Steering::DFSRun::fillFromFile(const std::string &fName, bool finalPulse) {

  std::ifstream inFile;
  inFile.open(fName.c_str());
   
   if (!inFile.is_open()) {
       if (finalPulse)
          std::cerr << " Steering::DFSRun::DFSRun: No final result " << fName.c_str() << std::endl;
       else   
          std::cerr << " Steering::DFSRun::DFSRun: No Local DMS convergence" << fName.c_str() << std::endl;
       return;
   }
  size_t il = 0;
  while (inFile.good()) {
      char lineC[1024];
      inFile.getline(lineC, 1024);
      std::string line(lineC);
      if (line.length() < 2) continue;
      if ((il == 0) && 
         (line.find("BPM z y yp epsilY epsilNDY") == std::string::npos)) {
         std::cerr << " Steering::DFSRun::DFSRun: Wrong file-stub  " << fName << std::endl;
         std::cerr << " Top line  " << line << std::endl;
         return;
      } 
      if (il == 0) {
	il++;
        continue;
      }
      std::istringstream lldStr(line);
      MonitData d;
      double ddPrev;
       lldStr >> d.BPM >> d.z >>  d.y >> d.yp >>
            d.epsilY >> d.epsilNDY >> d.epsilYM >>  d.epsilNDYM >>  d.Disp >>
	    d.DispP >>  d.BpmYOff >>  d.QuadOff >>  d.DipVal >> ddPrev;
      data.push_back(d); 
      il++; 
    } // on line of the NTuple
  inFile.close();
  std::cerr << " Got data from file " << fName << " num BPMs " << data.size() << std::endl;
}

void Steering::DFSRun::loadCorrectors(SteerUtil* myUtility) {
  
  for (size_t iDip=0; iDip != data.size(); iDip++) {
    double v = data[iDip].DipVal;
    std::cout << " Previous Corrector = " << 
      myUtility->GetOneDipoleSetting(iDip) << " new " << 1.0e-3*v << std::endl;
    myUtility->SetOneDipole(iDip, 1.0e-3*v); 

  }
}
void Steering::StudyDispersionDMSAA1(SteerUtil* myUtility) 
{  
 
  myTwissParam aP = myUtility->getInputTwissParam();
  
  std::string fNameDispOut("./DispersionNominal_Default.txt");
    
  myUtility->fillReferenceDispersion(0.999,
                                     true,
				     fNameDispOut, aP.DY, aP.DYp);

  std::string okConv("MLStN1_GFC_1169-0");
  Steering::DFSRun aRun(okConv, 0, 60, false);  
  size_t nn = aRun.getNumberBPMS();
  for (size_t i = 0; i != nn; i++) {
    double d = 1.0e-3*aRun.getDipoleValue(i);
    double dPrev = myUtility->GetOneDipoleSetting(i); 
    std::cerr << " Setting dipole " << i << " from " << dPrev << " to " << d << std::endl;
    myUtility->SetOneDipole(i, d);
  }
  std::string fNameDispOut2("./Dispersion_MLStN1_GFC_1169-0_g0_D60.txt");
    
  myUtility->fillReferenceDispersion(0.999,
                                     true,
				     fNameDispOut2, aP.DY, aP.DYp);
				     
  std::string badConv("MLStN1_GFC_1167-0");
  Steering::DFSRun aRun2(badConv, 0, 60, false);  
  nn = aRun2.getNumberBPMS();
  for (size_t i = 0; i != nn; i++) {
    double d = 1.0e-3*aRun2.getDipoleValue(i);
    myUtility->SetOneDipole(i, d);
  }
  
  std::string fNameDispOut3("./Dispersion_MLStN1_GFC_1167-0_g0_D60.txt");
    
  myUtility->fillReferenceDispersion(0.999,
                                     true,
				     fNameDispOut3, aP.DY, aP.DYp);
  return;				     
}
void Steering::StudyDispersionDMSAA2(SteerUtil* myUtility)  {

  // In this study, we will tilt a cryo-module  and measure Dispersion vs Delta E/E
  // Choose to do this at Cryo-Module after BPM 7, such that we have enough room to make a Dispersion 
  // to try to correct. This 6*3*8 for the v0 lattice.. 
  // Define the input Twiss parameters. 
   BmlPtr theModel = myUtility->getTheModelPtr();
  myTwissParam aP;
  aP.defined = true;
  aP.betaX = 35.3226; // in meters 
  aP.alphaX = -0.7486; // in meters 
  aP.betaY = 97.7362; // in meters 
  aP.alphaY = 1.65062; // in meters 
  aP.DY = 0.000496979;
//  aP.DY = 0.0006;  // Values tuned to get best performance..
  aP.DYp = -5.93072e-06;
  aP.DX = 0.;
  aP.DXp = 0.;
   

// Just dump the values of dipoles setting ,and quit .

//   for (size_t iDip=0; iDip != 20; iDip++) std::cout 
//      << " Dipole " << iDip << " Val " <<  myUtility->GetOneDipoleSetting(iDip) << std::endl;
   
  
  // First, measure the Dispersion v dP/P for the the perfect lattice, at BPM  
  
  const int bpmCrit = 9;
  std::vector<double> dPoP(0);
  double dpSimple[] ={ 1., 2., 5.};
  for (int k=-5; k !=-1; k++) {
    for (int kk=0; kk!=3; kk++) dPoP.push_back(dpSimple[kk]*std::pow(10., ((double) k))); 
  }
  dPoP.push_back(0.1); dPoP.push_back(0.15); dPoP.push_back(0.2);
  std::vector<double> dispPerf;  // The Dispersion 
  std::vector<double> dispPerfBPM;  // The Dispersion 
  std::vector<double> dispPosPerf;  // The Position at nominal energy
  std::string blank("");
  
  for (size_t kd=0; kd !=  dPoP.size(); kd++) {
      std::cerr << " dPoP " << dPoP[kd] << std::endl;
      std::string ffNow(blank);
      if (std::abs(dPoP[kd] - 0.05) < 0.00001)  
	     ffNow = std::string("./CheckDispersionCalcOK_04_25_dpp05.txt");
      if (std::abs(dPoP[kd] - 0.01) < 0.00001)  
	     ffNow = std::string("./CheckDispersionCalcOK_04_25_dpp01.txt");
      if (std::abs(dPoP[kd] - 0.1) < 0.00001)  
	     ffNow = std::string("./CheckDispersionCalcOK_04_25_dpp1.txt");
      myUtility->fillReferenceDispersion(1.0-dPoP[kd], true, 
                                          ffNow, aP.DY, aP.DYp, 50);
      dispPerf.push_back(myUtility->getDispersionAtBPM(bpmCrit));
      dispPosPerf.push_back(myUtility->getRefPositionDispersionAtBPM(bpmCrit));
      // Also store the perfect Dispersion vs y 
      if (kd == 0) {
         for (int iBpm=0; iBpm != 20; iBpm ++ ) 
	   dispPerfBPM.push_back(myUtility->getDispersionAtBPM(iBpm));
      }
  }
//  cerr << " And quit now " << std::endl;
//  exit(2);
  // Now misalign the cavities.
  size_t iCav = 0;
  size_t iCavBad = 0;
  const size_t nCavBad = 16;
  double cavPitch[] = {-300., 600., -300., 600.,-300., 600.,-300., 600., 
                        600., 600., 500.,  200., 600., 600., 300., 386.};
  size_t nCavBadStart = 6*3*8 - 1;
  LinacCavityPtr aCav159;
  LinacCavityPtr aCavLast;
  BmlPtr aModel = myUtility->getTheModelPtr();
  for(beamline::iterator dbi = aModel->begin(); 
        dbi != aModel->end(); ++dbi) {
    ElmPtr q = *dbi;
    if (LinacCavityPtr cavPtr=boost::dynamic_pointer_cast<LinacCavity> (q)) {
	if (iCav > nCavBadStart) {
	   myUtility->RotateOneCavity(iCav, 1.0e-6*cavPitch[iCavBad]);
	   iCavBad++;
	   if (iCav == 159) aCav159 = cavPtr;
	}
	aCavLast = cavPtr;
	if (iCavBad == nCavBad) break; 
	iCav++;	
    }
  }     		
  std::cerr << "DispersionAA2_1 .... Cavity Rotated... " 
       <<  " Pitch cavity 159 " << myUtility->getCavityPitch(aCav159)   
       <<  " Last touched " << myUtility->getCavityPitch(aCavLast) << std::endl;
  
  std::vector<double> dispBadCav;  // The Dispersion 
  std::vector<double> dispPosBadCav;  
  for (size_t kd=0; kd !=  dPoP.size(); kd++) {
      myUtility->fillReferenceDispersion(1.0-dPoP[kd], true, blank, aP.DY, aP.DYp);
      dispBadCav.push_back(myUtility->getDispersionAtBPM(bpmCrit));
      dispPosBadCav.push_back(myUtility->getRefPositionDispersionAtBPM(bpmCrit));
  } 
  // Dump this on a file 
  std::cerr << "DispersionAA2_1 .... Dispersion measured.... " << std::endl;
  std::ofstream fOut("./DispersionAA2OK_04_25_1.txt");
  fOut << " dPoP yPerf DispPerf yBC DispBC " << std::endl;
  for (size_t kd=0; kd !=  dPoP.size(); kd++)
    fOut << " " << dPoP[kd] << " " << dispPosPerf[kd] << " " << dispPerf[kd]
         << " " << dispPosBadCav[kd] << " " << dispBadCav[kd] << " " << std::endl; 

  fOut.close();  
  // Now scan with the (?) relevant dipoles.
  std::cerr << "DispersionAA2b_1 .... First file generated.... " << std::endl;
//  cerr << " And quit now " << std::endl;
//  exit(2);
  double rangeVal = 5*0.5e-3; 
  unsigned int  nSteps[] = {50, 50, 3, 3};
  const size_t nDipScan = 4;
  size_t nScanMax = 1; for (size_t idd=0; idd != nDipScan; idd++) nScanMax *= nSteps[idd];
  unsigned int dipNum[] = {5, 6, 7, 8};
  std::vector<double> nomValuesDip(nDipScan);
  for (size_t idd=0; idd != nDipScan; idd++) 
     nomValuesDip[idd] = myUtility->GetOneDipoleSetting(dipNum[idd]);
  std::vector<double> nomStepsDip(nDipScan);
  for (size_t idd=0; idd != nDipScan; idd++) 
//     nomStepsDip[idd] = 2*rangeVal*std::abs(nomValuesDip[idd])/nStep;
     nomStepsDip[idd] = 2*rangeVal/nSteps[idd]; // Silly, constant vector.. 
                                          // Plannning to make the steps adaptive 

  nomStepsDip[2] *= 0.1;
  nomStepsDip[3] *= 0.1;
  
  
  unsigned int dipIndex[] = {0, 0, 0, 0};
  
  std::ostringstream fOut2NamAStrStr;
  fOut2NamAStrStr << "./DispersionAA2Sc_" << dipNum[0] << "_" << dipNum[1];
  std::ostringstream fOut2NamBStrStr; 
  fOut2NamBStrStr << fOut2NamAStrStr.str(); 
  for (size_t idd=2; idd != nDipScan; idd++)  fOut2NamBStrStr << "_" << dipIndex[idd];
  fOut2NamBStrStr << ".txt";
  std::string fOut2NamBStr = fOut2NamBStrStr.str();
  std::ofstream fOut2(fOut2NamBStr.c_str());
  for (size_t idd=0; idd != nDipScan; idd++) fOut2 << "  " << "DipVal" << dipNum[idd];
  for (size_t iBpm = bpmCrit-1; iBpm != 20; iBpm++)  fOut2 << " d" << iBpm << " y" << iBpm; 
  fOut2 << " dSqAv " << " ySqAv" << std::endl;  
     
  std::vector<double> dipVV(nDipScan);
  for (size_t idd=0; idd != nDipScan; idd++) 
     dipVV[idd] = nomValuesDip[idd] - (nSteps[idd]/2)*nomStepsDip[idd];
  std::cerr << " dip Vals " <<  1.0e3*dipVV[0] << " ... " << 1.0e3*dipVV[1] << std::endl;
  std::cerr << " Nom dip Vals " <<  1.0e3*nomValuesDip[0] << " ... " << 1.0e3*nomValuesDip[1] << std::endl;
  std::cerr << " Step dip Vals " <<  1.0e3*nomStepsDip[0] << " ... " << 1.0e3*nomStepsDip[1] << std::endl;
  size_t nScans = 0;
  while (nScans != nScanMax) {
      for (size_t idd=0; idd != nDipScan; idd++) {
         myUtility->SetOneDipole(dipNum[idd], dipVV[idd]);
         fOut2 << " " << 1.0e3*dipVV[idd];
      }
      myUtility->fillReferenceDispersion(0.98, true, blank, aP.DY, aP.DYp, bpmCrit+21);
      double ySqAv = 0.;
      double dSqAv = 0.;
      for (size_t iBpm = bpmCrit-1; iBpm != 20; iBpm++) {
         double d = myUtility->getDispersionAtBPM(iBpm) - dispPerfBPM[iBpm];
	 double y = myUtility->getRefPositionDispersionAtBPM(iBpm);
         fOut2 << " " <<  d << " " <<  y;
	 ySqAv += y*y;
	 dSqAv += d*d;
      }
      int nbb = 20 - bpmCrit + 1;
      fOut2 << " " << dSqAv/nbb << " " << ySqAv/nbb << std::endl;
      dipIndex[0]++;
      for (size_t idd=0; idd != nDipScan-1; idd++) {
        if (dipIndex[idd] == nSteps[idd]) {
	   dipIndex[idd] = 0;
	   dipIndex[idd+1] += 1; 
	   if (idd == 1) {
	     fOut2.close();
             std::ostringstream fOut2NamBStrStr; 
             fOut2NamBStrStr << fOut2NamAStrStr.str(); 
             for (size_t id=2; id != nDipScan; id++)  fOut2NamBStrStr << "_" << dipIndex[id];
             fOut2NamBStrStr << ".txt";
             std::string fOut2NamBStr = fOut2NamBStrStr.str();
	     fOut2.open(fOut2NamBStr.c_str());
             for (size_t id=0; id != nDipScan; id++) fOut2 << "  " << "DipVal" << dipNum[id];
             for (size_t iBpm = bpmCrit-1; iBpm != 20; iBpm++)  fOut2 << " d" << iBpm << " y" << iBpm;
             fOut2 << " dSqAv " << " ySqAv" << std::endl;  
	   }
	}
      }
      for (size_t idd=0; idd != nDipScan; idd++) {
	int ii = ((int) dipIndex[idd]) - ((int) nSteps[idd]/2 );
        dipVV[idd] = nomValuesDip[idd] + ii*nomStepsDip[idd];
      }
      nScans++;
      if (nScans%100 == 0) {
        std::cerr << " At dips "; 
	for (size_t idd=0; idd != nDipScan; idd++) 
	     std::cerr << " " << dipIndex[idd] << " v= " << dipVV[idd];
	std::cerr << std::endl;
      
      }
  }
  fOut2.close();  
} 
