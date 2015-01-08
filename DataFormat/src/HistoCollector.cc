#ifdef __localRun
#  include "../interface/HistoCollector.h"
#else
#  include "HcalClosureTest/DataFormat/interface/HistoCollector.h"
#endif

#include <TFile.h>
#include <TObjString.h>
#include <TSystem.h>
#include <TDirectory.h>
#include <TList.h>
#include <TKey.h>
#include <TClass.h>

#ifdef __localRun
#ifdef __CINT__
#pragma link C++ class HistoCollector_t;
#endif
#endif

// ----------------------------------------------------------------

std::vector<TString> packMessages(int msgLineCount, ...)
{
  std::vector<TString> knownMessages;
  if (msgLineCount>0) {
    knownMessages.reserve(msgLineCount);
    va_list vl;
    va_start(vl,msgLineCount);
    for (int i=0; i<msgLineCount; ++i) {
      typedef const char *constCharPtr;
      TString temp= TString(va_arg(vl,constCharPtr));
      knownMessages.push_back(temp);
    }
    va_end(vl);
  }
  return knownMessages;
}

// ----------------------------------------------------------------

void SaveCanvas(TCanvas* canv, const TString &canvName, TString destDir)
{
  gSystem->mkdir(destDir,kTRUE);
  gSystem->mkdir(destDir+TString("/png"),kTRUE);
  gSystem->mkdir(destDir+TString("/pdf"),kTRUE);
  gSystem->mkdir(destDir+TString("/root"),kTRUE);

  TString saveName=destDir+TString("/png/");
  saveName+=canvName;
  saveName+=".png";
  canv->SaveAs(saveName);
  saveName.ReplaceAll("png","pdf");
  canv->SaveAs(saveName);
  saveName.ReplaceAll("pdf","root");
  canv->SaveAs(saveName);
  return;
}

// ----------------------------------------------------------------

//******************************************//
// author: Chris Seez
//
//  This function evaluates the effective
//  sigma of a histogram.
//
//******************************************//

Double_t calc_effSigma(TH1 * hist)
{

  TAxis *xaxis = hist->GetXaxis();
  Int_t nb = xaxis->GetNbins();
  if(nb < 10) {
    std::cout << "effsigma: Not a valid histo. nbins = " << nb << std::endl;
    return 0.;
  }

  Double_t bwid = hist->GetBinWidth(1);
  if(bwid == 0) {
    std::cout << "effsigma: Not a valid histo. bwid = " << bwid << std::endl;
    return 0.;
  }
  //Double_t xmax = xaxis->GetXmax(); // not used
  Double_t xmin = xaxis->GetXmin();
  Double_t ave = hist->GetMean();
  Double_t rms = hist->GetRMS();

  Double_t total=0.;
  for(Int_t i=0; i<nb+2; i++) {
    total+=hist->GetBinContent(i);
  }
  if(total < 10.) {
    std::cout << "effsigma: Too few entries " << total << std::endl;
    return 0.;
  }
  Int_t ierr=0;
  Int_t ismin=999;

  Double_t rlim=0.683*total;
  Int_t nrms= Int_t(2.0*rms/(bwid));    // Set scan size to +/- rms
  //if(nrms > nb/10) nrms=nb/10; // Could be tuned...

  Double_t widmin=9999999.;
  for(Int_t iscan=-nrms;iscan<nrms+1;iscan++) { // Scan window centre
    Int_t ibm=Int_t((ave-xmin)/bwid+1+iscan);
    Double_t x=(ibm-0.5)*bwid+xmin;
    Double_t xj=x;
    Double_t xk=x;
    Int_t jbm=ibm;
    Int_t kbm=ibm;

    // ibm -- curent bin value
    // x, xj, xk -- value of the x-axis
    // jbm, kbm -- bin values

    Double_t bin=hist->GetBinContent(ibm);
    total=bin;
    for(Int_t j=1;j<nb;j++){
      if(jbm < nb) {
        jbm++;
        xj+=bwid;
        bin=hist->GetBinContent(jbm);
        total+=bin;
        if(total > rlim) break;
      }
      else ierr=1;

      if(kbm > 0) {
        kbm--;
        xk-=bwid;
        bin=hist->GetBinContent(kbm);
        total+=bin;
        if(total > rlim) break;
      }
      else ierr=1;
    }
    Double_t dxf=(total-rlim)*bwid/bin;
    Double_t wid=(xj-xk+bwid-dxf)*0.5;

    if(wid < widmin) {
      widmin=wid;
      ismin=iscan;
    }
  }
  if(ismin == nrms || ismin == -nrms) {
    std::cout << std::endl << std::endl << "CRAP!!!" << std::endl;
    std::cout << ismin << '\t' << nrms << std::endl;
    ierr=3;
  }
  if(ierr != 0 && ierr != 1) std::cout << "effsigma: Error of type " << ierr <<std::endl;

  return widmin;

}

// ----------------------------------------------------------------
// ----------------------------------------------------------------

void HistoCollector_t::Clear()
{
  fHistos1Dv.clear();
  fHistos2Dv.clear();
  fCanvasV.clear();
  fMsgV.clear();
  fTag.Clear();
}

// ----------------------------------------------------------------

TH1D* HistoCollector_t::GetH1D(TString name) {
  TH1D *h= NULL;
  for (unsigned int i=0; !h && (i<fHistos1Dv.size()); i++) {
    if (TString(fHistos1Dv[i]->GetName()) == name) {
      h=fHistos1Dv[i];
      //std::cout << "located at i=" << i << "\n";
    }
  }
  return h;
}

// ----------------------------------------------------------------

TH2D* HistoCollector_t::GetH2D(TString name) {
  TH2D *h2= NULL;
  for (unsigned int i=0; !h2 && (i<fHistos2Dv.size()); i++) {
    if (TString(fHistos2Dv[i]->GetName()) == name) {
      h2=fHistos2Dv[i];
      //std::cout << "located at i=" << i << "\n";
    }
  }
  return h2;
}

// ----------------------------------------------------------------

void HistoCollector_t::Add(const std::vector<TString> &vec) {
  fMsgV.reserve(fMsgV.size() + vec.size());
  for (unsigned int i=0; i<vec.size(); ++i) {
    fMsgV.push_back(vec[i]);
  }
  return;
}

// ----------------------------------------------------------------

int HistoCollector_t::Add(int msgLineCount, ... )
{
  int count=0;
  if (msgLineCount>0) {
    fMsgV.reserve(fMsgV.size() + msgLineCount);
    va_list vl;
    va_start(vl,msgLineCount);
    for (int i=0; i<msgLineCount; ++i) {
      typedef const char *constCharPtr;
      TString temp= TString(va_arg(vl,constCharPtr));
      if (temp.Length()) {
	count++;
	fMsgV.push_back(temp);
      }
    }
    va_end(vl);
  }
  return count;
}

// ----------------------------------------------------------------

int HistoCollector_t::SaveToFile(TString fName) 
{
  TFile fout(fName,"recreate");
  if (!fout.IsOpen()) {
    std::cout << "SaveVectorsToFile(" << fName
	      << "): failed to create the file\n";
    return 0;
  }
  fout.cd();

  for (unsigned int i=0; i<fHistos1Dv.size(); ++i) fHistos1Dv[i]->Write();
  for (unsigned int i=0; i<fHistos2Dv.size(); ++i) fHistos2Dv[i]->Write();
  for (unsigned int i=0; i<fCanvasV.size(); ++i) fCanvasV[i]->Write();

  if (fMsgV.size()) {
    TObjString ostr;
    ostr.Write("info_fields_follow");

    for (unsigned int i=0; i<fMsgV.size(); ++i) {
      ostr.Write(fMsgV[i]);
    }
  }

  // put time stamp
  time_t ltime;
  ltime=time(NULL);
  TString str = TString(asctime(localtime(&ltime)));
  if (str[str.Length()-1]=='\n') str.Remove(str.Length()-1,1);
  TObjString date(str);
  date.Write(str.Data());

  fout.Close();
  std::cout << "File <" << fout.GetName() << "> created\n";
  return 1;

}

// ----------------------------------------------------------------

int HistoCollector_t::SaveCanvases(TString outFileNameTag) const
{
  TString destDir="plots_" + outFileNameTag;
  for (unsigned int i=0; i<fCanvasV.size(); ++i) {
    TString figName="fig-" + outFileNameTag + TString("-") +
      TString(fCanvasV[i]->GetName());
    SaveCanvas(fCanvasV[i],figName,destDir);
  }
  return int(fCanvasV.size());
}

// ----------------------------------------------------------------

int HistoCollector_t::LoadFromFile(TString fName, int loadHistos1D,
				   int loadHistos2D, int loadCanvases,
				   TString prependToNames)
{
  this->Clear();

  TFile fin(fName,"read");
  if (!fin.IsOpen()) {
    std::cout << "LoadFromFile(" << fName
	      << "): failed to open the file\n";
    return 0;
  }
  fin.cd();

  int verb=1;
  TDirectory *topDir = gDirectory;
  TDirectory *sourceDir=topDir;
  TList *subDirList=sourceDir->GetListOfKeys();
  int count=subDirList->GetEntries();
  std::cout << "There are " << count
	    << " keys in main directory of file <" << fName << ">\n";
  TIter subDirs(subDirList);
  TKey *key=NULL;
  int debug_count=-1;
  while ( (key = (TKey*)subDirs()) ) {
    const TString candName=key->GetName();
    if (verb) std::cout << "candName=" << candName << "\n";
    TObject *obj= key->ReadObj();
    if ( loadHistos1D && obj->IsA()->InheritsFrom(TH1D::Class()) ) {
      TH1D *h=(TH1D*)obj;
      h->SetDirectory(0);
      if (prependToNames.Length()) {
	TString tmp=h->GetName();
	h->SetName(prependToNames + tmp);
      }
      fHistos1Dv.push_back(h);
      debug_count--; if (debug_count==0) break;
    }
    else if ( loadHistos2D && obj->IsA()->InheritsFrom(TH2D::Class()) ) {
      TH2D *h2=(TH2D*)obj;
      h2->SetDirectory(0);
      if (prependToNames.Length()) {
	TString tmp=h2->GetName();
	h2->SetName(prependToNames + tmp);
      }
      fHistos2Dv.push_back(h2);
      debug_count--; if (debug_count==0) break;
    }
    else if ( loadCanvases && obj->IsA()->InheritsFrom(TCanvas::Class()) ) {
      TCanvas *canv=(TCanvas*)obj;
      if (prependToNames.Length()) {
	TString tmp=canv->GetName();
	canv->SetName(prependToNames + tmp);
      }
      fCanvasV.push_back(canv);
      debug_count--; if (debug_count==0) break;
    }
    else {
      delete obj;
    }
  }
  fin.Close();

  std::cout << "from file <" << fin.GetName() << "> got:\n";
  if (loadHistos1D) std::cout << " " << fHistos1Dv.size() << " TH1Ds\n";
  if (loadHistos2D) std::cout << " " << fHistos2Dv.size() << " TH2Ds\n";
  if (loadCanvases) std::cout << " " << fCanvasV.size() << " canvases\n";

  return 1;
}

// ----------------------------------------------------------------
