#include <cmath>
#include <iostream>
#include <MLBPMs.h>
#include <basic_toolkit/Distribution.h>

#ifdef USEROOT
#include "TCanvas.h"
#include "TGraphErrors.h"
#endif

using std::cout;
using std::cerr;
using std::endl;
using std::ostringstream;


MLBPMs::MLBPMs(): vector<MLBPMPtr>(0){
  theGaussDist = 0;
}

MLBPMs::~MLBPMs() {
 // The BPM are owned by the beamline in CHEF
}

void MLBPMs::setResolution(double res) {
    for(vector<MLBPMPtr>::iterator iEl= this->begin(); 
             iEl!=this->end(); ++iEl) 
	  (*iEl)->SetResolution(res);      
}

void MLBPMs::setGaussianOffsets(double sigma, double sCut, int nB) {

 Gaussian* myDistG = theGaussDist;
 if (myDistG == 0) myDistG = new Gaussian();
 int n = 0;
 for (vector<MLBPMPtr>::iterator it = this->begin(); it != this->end(); ++it) {
   if (n > nB) {
     double rr =  1.e10;
     while (std::abs(rr) > sCut) rr =  myDistG->getValue(); 
     (*it)->SetOffset(rr*sigma);
   }
   n++;
 }
 if (theGaussDist == 0) delete myDistG;
}

void MLBPMs::setGaussianScales(double sigma, double sCut, int nB) {

 Gaussian* myDistG = theGaussDist;
 if (myDistG == 0) myDistG = new Gaussian();
 int n = 0;
 for (vector<MLBPMPtr>::iterator it = this->begin(); it != this->end(); ++it) {
   if (n > nB) {
     double rr =  1.e10;
     while (std::abs(rr) > sCut) rr =  myDistG->getValue(); 
     (*it)->SetScale(1.0+rr*sigma);
    }
    n++;
 }
 if (theGaussDist == 0) delete myDistG;
}

void MLBPMs::setAnOffset(int index, double value) {
 if ((unsigned int) index >= this->size()) return;
 MLBPMPtr theBPM = (*this)[index];
 theBPM->SetOffset(value);
}

double MLBPMs::getOffset(int index) {
 if ((unsigned int) index >= this->size()) return 0.;
 MLBPMPtr theBPM = (*this)[index];
 return theBPM->GetOffset();
}

#ifdef USEROOT
void MLBPMs::PlotIt(const std::string keyName, double zM) {
   if (bpmList.size() <2 ) return;
   size_t nPts = 0;
   for (vector<MLBPMPtr>::const_iterator it = this->begin(); it != this->end(); ++it) {
      const MLBPMPtr abp = *it;
      if (abp->GetZPosition() > zM) break;
      nPts++;
    }
    double zp[nPts];
    double zpErr[nPts];
    double yCoord[nPts];
    double cTau[nPts];
    double yCoordErr[nPts];
    int i = 0;
    for (vector<MLBPMPtr>::const_iterator it = this->begin(); it != this->end(); ++it) {
      const MLBPMPtr abp = *it;
      if (abp->GetZPosition() > zM) break;
      cTau[i] = abp->GetCTime() - abp->GetZPosition();
      zp[i] = abp->GetZPosition();
      zpErr[i] = 0.03; // Assume 3 cm.. somewhat arbitrary..
      yCoord[i] = abp->GetMeasurement()*1.e6;; // in microns
      yCoordErr[i] = abp->GetError()*1.e6;
      if (std::abs(cTau[i]/zp[i]) > 0.1) 
        cout << " Bad ZP for BPM at " << zp[i] << endl;
      i++;
    }
    TCanvas* aCanvas = new TCanvas("BPM-Merlin Plots ", "Merlin Plots");
    TGraphErrors *tg = new TGraphErrors(nPts,zp, yCoord, zpErr, yCoordErr );
    TGraph *tgCTau = new TGraph(nPts,zp, cTau);
    std::string title(" BPM, case  ");
    title += keyName;
    aCanvas->Clear();
//    aCanvas->Divide(1,2);
    aCanvas->cd(1);
    tg->SetTitle(title.c_str());
    tg->SetMarkerColor(4);
    tg->SetMarkerStyle(21);
    tg->Draw("ALP");
/*
    aCanvas->cd(2);
    tgCTau->SetTitle(" CTau-ZP, same horizontal scale");
    tgCTau->SetMarkerColor(3);
    tgCTau->SetMarkerStyle(22);
    tgCTau->Draw("ALP");
*/
    aCanvas->Modified();
    aCanvas->Update();
    std::string fName("BPM-");
    fName += keyName;
    fName += ".eps";
    aCanvas->Print(fName.c_str());
    delete tg;
    delete aCanvas;
}
#endif

void MLBPMs::DumpIt(std::ostream& out, double zMax) {
   
//    cout << " Dump of my BPM values, with " << this->size() 
//         << " BPMs " << endl;
//    cout << " Z Position  Y-mear   Y-error  " << endl;
    int i = 0; 
    for (vector<MLBPMPtr>::const_iterator it = this->begin(); it != this->end(); ++it) {
      const MLBPMPtr abp = *it;
      if (abp->GetZPosition() > zMax) break;
      out << " " << abp->GetZPosition() << "         "   
           << abp->GetMeasurement() << "           "  
	   << abp->GetError() << endl;
      i++;
    }
}

