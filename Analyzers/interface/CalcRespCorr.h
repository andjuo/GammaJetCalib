#ifndef _HCALCLOSURETEST_ANALYZERS_CALCRESPCORR_H_
#define _HCALCLOSURETEST_ANALYZERS_CALCRESPCORR_H_

//
// CalcRespCorr.h
//
//    description: Makes plots to calculate the response correction using 50 GeV pions.
//
//    author: J.P. Chou, Brown
//
//

// system include files
#include <memory>
#include <string>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "HcalClosureTest/DataFormat/interface/SingleParticleCluster.h"

// forward declarations
class TH1D;
class TH2D;
class TFile;

//
// class declaration
//

class CalcRespCorr : public edm::EDAnalyzer {
 public:
  explicit CalcRespCorr(const edm::ParameterSet&);
  ~CalcRespCorr();
  
  
 private:
  virtual void beginJob();
  virtual void analyze(const edm::Event&, const edm::EventSetup&);
  virtual void endJob();

  // parameters
  edm::EDGetTokenT<SingleParticleClusterCollection>   tok_SPC_;
  std::string clstrCollName_;    // label for the particle cluster collection
  std::string rootHistFilename_; // name of the histogram file
  double maxDeltaR_;             // maximum deltaR between particle and cluster
  double maxModifiedEMF_;        // maximum modified EMF of cluster (before corrections)
  std::vector<double> respCorr_; // response corrections ordered by ieta (-29 to 29)

  // root file/histograms
  TFile* rootfile_;  

  // Generated particle plots before selection
  TH1D* hGenpE_;
  TH1D* hGenpEta_;
  TH1D* hGenpPhi_;
  TH2D* hGenpEtaPhi_;

  // Cluster plots before selection
  TH1D* hClstrE_;
  TH1D* hClstrEta_;
  TH1D* hClstrPhi_;
  TH2D* hClstrEtaPhi_;
  TH1D* hClstrEMF_;
  TH1D* hClstrModifiedEMF_;
  TH1D* hClstrDeltaR_;

  // cluster plots after selection
  TH1D* hAfterClstrE_;
  TH1D* hAfterClstrEoverP_;
  TH1D* hAfterClstrEta_;
  TH1D* hAfterClstrPhi_;
  TH2D* hAfterClstrEEta_;
  TH2D* hAfterClstrEPhi_;
  TH2D* hAfterClstrEoverPEta_;
  TH2D* hAfterClstrEoverPPhi_;

  // response corrections
  TH2D* hRespIetaHighest_;  // use only the highest energy tower in the cluster
  TH2D* hRespIetaCentral_;  // use only the central tower in the cluster
  TH2D* hRespIetaAll_;      // use all the towers in the cluster
  
  // corrected cluster plots after selection
  TH1D* hCorrClstrE_;
  TH1D* hCorrClstrEoverP_;
  TH2D* hCorrClstrEEta_;
  TH2D* hCorrClstrEPhi_;
  TH2D* hCorrClstrEoverPEta_;
  TH2D* hCorrClstrEoverPPhi_;

};


#endif
