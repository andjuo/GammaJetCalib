#ifndef _HCALCLOSURETEST_ANALYZERS_CALCRESPCORRDIJETS_H_
#define _HCALCLOSURETEST_ANALYZERS_CALCRESPCORRDIJETS_H_

//
// CalcRespCorrDiJets.h
//
//    description: Makes plots to calculate the response correction using dijets.
//
//    author: J.P. Chou, Brown
//
//    updated: David G. Sheffield, Rutgers
//
//

// system include files
#include <memory>
#include <string>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/Framework/interface/ESHandle.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "DataFormats/JetReco/interface/CaloJetCollection.h"
#include "DataFormats/JetReco/interface/PFJetCollection.h"
#include "DataFormats/HcalRecHit/interface/HBHERecHit.h"
#include "DataFormats/HcalRecHit/interface/HFRecHit.h"
#include "DataFormats/HcalRecHit/interface/HORecHit.h"
#include "DataFormats/ParticleFlowReco/interface/PFBlockFwd.h"
#include "DataFormats/ParticleFlowReco/interface/PFBlock.h"
#include "DataFormats/ParticleFlowReco/interface/PFCluster.h"
#include "DataFormats/ParticleFlowReco/interface/PFRecHit.h"
#include "DataFormats/ParticleFlowReco/interface/PFRecHitFwd.h"
#include "DataFormats/HcalDetId/interface/HcalDetId.h"
#include "DataFormats/TrackReco/interface/TrackFwd.h"
#include "DataFormats/JetReco/interface/GenJetCollection.h"
#include "Geometry/CaloGeometry/interface/CaloGeometry.h"
#include "Geometry/CaloGeometry/interface/CaloSubdetectorGeometry.h"
#include "Geometry/Records/interface/CaloGeometryRecord.h"
#include "DataFormats/HepMCCandidate/interface/GenParticle.h"
#include "SimDataFormats/GeneratorProducts/interface/GenEventInfoProduct.h"

// forward declarations
class TH1D;
class TH2D;
class TFile;
class TTree;

//
// class declarations
//

class CaloJetCorretPair : protected std::pair<const reco::CaloJet*, double> {
 public:
  CaloJetCorretPair() {
    first=0;
    second=1.0;
  }
  CaloJetCorretPair(const reco::CaloJet* j, double s) {
    first=j;
    second=s;
  }
  ~CaloJetCorretPair() {}

  inline const reco::CaloJet* jet(void) const { return first; }
  inline void jet(const reco::CaloJet* j) { first=j; return; }
  inline double scale(void) const { return second; }
  inline void scale(double d) { second=d; return; }

 private:
  
};

class PFJetCorretPair : protected std::pair<const reco::PFJet*, double> {
 public:
  PFJetCorretPair() {
    first=0;
    second=1.0;
  }
  PFJetCorretPair(const reco::PFJet* j, double s) {
    first=j;
    second=s;
  }
  ~PFJetCorretPair() {}

  inline const reco::PFJet* jet(void) const { return first; }
  inline void jet(const reco::PFJet* j) { first=j; return; }
  inline double scale(void) const { return second; }
  inline void scale(double d) { second=d; return; }

 private:
  
};

class CalcRespCorrDiJets : public edm::EDAnalyzer {
 public:
  explicit CalcRespCorrDiJets(const edm::ParameterSet&);
  ~CalcRespCorrDiJets();
  
  
 private:
  virtual void beginJob();//(const edm::EventSetup&);
  virtual void analyze(const edm::Event&, const edm::EventSetup&);
  virtual void endJob();

  
  // parameters
  bool debug_;                      // print debug statements
  std::string caloJetCollName_;     // label for the calo jet collection
  std::string caloJetCorrName_;     // label for the calo jet correction service
  std::string pfJetCollName_;       // label for the PF jet collection
  std::string pfJetCorrName_;       // label for the PF jet correction service
  std::string genJetCollName_;      // label for the genjet collection
  std::string genParticleCollName_; // label for the genparticle collection
  std::string genEventInfoName_;    // label for the generator event info collection
  std::string hbheRecHitName_;      // label for HBHERecHits collection
  std::string hfRecHitName_;        // label for HFRecHit collection
  std::string hoRecHitName_;        // label for HORecHit collection
  std::string rootHistFilename_;    // name of the histogram file
  double maxDeltaEta_;              // maximum delta-|Eta| between Jets
  double minTagJetEta_;             // minimum |eta| of the tag jet
  double maxTagJetEta_;             // maximum |eta| of the tag jet
  double minSumJetEt_;              // minimum Sum of the tag and probe jet Et
  double minJetEt_;                 // minimum Jet Et
  double maxThirdJetEt_;            // maximum 3rd jet Et
  double maxJetEMF_;                // maximum EMF of the tag and probe jets
  bool doCaloJets_;                 // use CaloJets
  bool doPFJets_;                   // use PFJets
  bool doGenJets_;                  // use GenJets

  edm::EDGetTokenT<reco::CaloJetCollection>         tok_CaloJet_;
  edm::EDGetTokenT<reco::PFJetCollection>           tok_PFJet_;
  edm::EDGetTokenT<std::vector<reco::GenJet> >      tok_GenJet_;
  edm::EDGetTokenT<std::vector<reco::GenParticle> > tok_GenPart_;
  edm::EDGetTokenT<GenEventInfoProduct>             tok_GenEvInfo_; 
  edm::EDGetTokenT<edm::SortedCollection<HBHERecHit,edm::StrictWeakOrdering<HBHERecHit> > > tok_HBHE_;
  edm::EDGetTokenT<edm::SortedCollection<HFRecHit,edm::StrictWeakOrdering<HFRecHit> > >     tok_HF_;
  edm::EDGetTokenT<edm::SortedCollection<HORecHit,edm::StrictWeakOrdering<HORecHit> > >     tok_HO_;

  // root file/histograms
  TFile* rootfile_;

  TH1D* hPassSelCalo_;
  TH1D* hPassSelPF_;
  TH1D* h_types_;
  TH1D* h_ntypes_;
  TH1D* h_ietaHCAL_;
  TH1D* h_etaHFHAD_;
  TH1D* h_etaHFEM_;
  TH1D* h_ietaHO_;
  TH1D* h_HFHAD_n_;
  TH1D* h_HFEM_n_;
  TH1D* h_HFHAD_type_;
  TH1D* h_HFEM_type_;
  TH1D* h_HBHE_n_;
  TH1D* h_HF_n_;
  TH1D* h_HO_n_;
  TH1D* h_twrietas_;
  TH2D* h_rechitspos_;
  TH1D* h_hbherecoieta_;
  TTree* calo_tree_;
  TTree* pf_tree_;
  
  float tcalojet_pt_, tcalojet_p_, tcalojet_eta_, tcalojet_phi_, tcalojet_emf_, tcalojet_scale_;
  float tcalojet_gendr_, tcalojet_genpt_, tcalojet_genp_;
  float tcalojet_EBE_, tcalojet_EEE_, tcalojet_HBE_, tcalojet_HEE_, tcalojet_HFE_;
  int tcalojet_ntwrs_;
  int tcalojet_twr_ieta_[100];
  float tcalojet_twr_eme_[100], tcalojet_twr_hade_[100];
  float pcalojet_pt_, pcalojet_p_, pcalojet_eta_, pcalojet_phi_, pcalojet_emf_, pcalojet_scale_;
  float pcalojet_gendr_, pcalojet_genpt_, pcalojet_genp_;
  float pcalojet_EBE_, pcalojet_EEE_, pcalojet_HBE_, pcalojet_HEE_, pcalojet_HFE_;
  int pcalojet_ntwrs_;
  int pcalojet_twr_ieta_[100];
  float pcalojet_twr_eme_[100], pcalojet_twr_hade_[100];
  float calo_dijet_deta_, calo_dijet_dphi_, calo_dijet_balance_;
  float calo_thirdjet_px_, calo_thirdjet_py_;
  int calo_Event_;

  float tpfjet_pt_, tpfjet_p_, tpfjet_E_, tpfjet_eta_, tpfjet_phi_, tpfjet_emf_, tpfjet_scale_;
  float tpfjet_gendr_, tpfjet_genpt_, tpfjet_genp_, tpfjet_genE_;
  float tpfjet_EBE_, tpfjet_EEE_, tpfjet_HBE_, tpfjet_HEE_, tpfjet_HFE_;
  float tpfjet_unkown_E_, tpfjet_unkown_px_, tpfjet_unkown_py_, tpfjet_unkown_pz_, tpfjet_unkown_EcalE_;
  float tpfjet_electron_E_, tpfjet_electron_px_, tpfjet_electron_py_, tpfjet_electron_pz_, tpfjet_electron_EcalE_;
  float tpfjet_muon_E_, tpfjet_muon_px_, tpfjet_muon_py_, tpfjet_muon_pz_, tpfjet_muon_EcalE_;
  float tpfjet_photon_E_, tpfjet_photon_px_, tpfjet_photon_py_, tpfjet_photon_pz_, tpfjet_photon_EcalE_;
  int tpfjet_unkown_n_, tpfjet_electron_n_, tpfjet_muon_n_, tpfjet_photon_n_;
  int tpfjet_had_n_;
  std::vector<float> tpfjet_had_E_, tpfjet_had_px_, tpfjet_had_py_, tpfjet_had_pz_, tpfjet_had_EcalE_, tpfjet_had_rawHcalE_, tpfjet_had_emf_, tpfjet_had_E_mctruth_;
  std::vector<int> tpfjet_had_id_, tpfjet_had_candtrackind_, tpfjet_had_mcpdgId_, tpfjet_had_ntwrs_;
  int tpfjet_ntwrs_;
  std::vector<int> tpfjet_twr_ieta_, tpfjet_twr_iphi_, tpfjet_twr_depth_, tpfjet_twr_subdet_, tpfjet_twr_candtrackind_, tpfjet_twr_hadind_, tpfjet_twr_elmttype_, tpfjet_twr_clusterind_;
  std::vector<float> tpfjet_twr_hade_, tpfjet_twr_frac_, tpfjet_twr_dR_;
  int tpfjet_cluster_n_;
  std::vector<float> tpfjet_cluster_eta_, tpfjet_cluster_phi_, tpfjet_cluster_dR_;
  int tpfjet_ncandtracks_;
  std::vector<float> tpfjet_candtrack_px_, tpfjet_candtrack_py_, tpfjet_candtrack_pz_, tpfjet_candtrack_EcalE_;
  float ppfjet_pt_, ppfjet_p_, ppfjet_E_, ppfjet_eta_, ppfjet_phi_, ppfjet_emf_, ppfjet_scale_;
  float ppfjet_gendr_, ppfjet_genpt_, ppfjet_genp_, ppfjet_genE_;
  float ppfjet_EBE_, ppfjet_EEE_, ppfjet_HBE_, ppfjet_HEE_, ppfjet_HFE_;
  float ppfjet_unkown_E_, ppfjet_unkown_px_, ppfjet_unkown_py_, ppfjet_unkown_pz_, ppfjet_unkown_EcalE_;
  float ppfjet_electron_E_, ppfjet_electron_px_, ppfjet_electron_py_, ppfjet_electron_pz_, ppfjet_electron_EcalE_;
  float ppfjet_muon_E_, ppfjet_muon_px_, ppfjet_muon_py_, ppfjet_muon_pz_, ppfjet_muon_EcalE_;
  float ppfjet_photon_E_, ppfjet_photon_px_, ppfjet_photon_py_, ppfjet_photon_pz_, ppfjet_photon_EcalE_;
  int ppfjet_unkown_n_, ppfjet_electron_n_, ppfjet_muon_n_, ppfjet_photon_n_;
  int ppfjet_had_n_;
  std::vector<float> ppfjet_had_E_, ppfjet_had_px_, ppfjet_had_py_, ppfjet_had_pz_, ppfjet_had_EcalE_, ppfjet_had_rawHcalE_, ppfjet_had_emf_, ppfjet_had_E_mctruth_;
  std::vector<int> ppfjet_had_id_, ppfjet_had_candtrackind_, ppfjet_had_mcpdgId_, ppfjet_had_ntwrs_;
  int ppfjet_ntwrs_;
  std::vector<int> ppfjet_twr_ieta_, ppfjet_twr_iphi_, ppfjet_twr_depth_, ppfjet_twr_subdet_, ppfjet_twr_candtrackind_, ppfjet_twr_hadind_, ppfjet_twr_elmttype_, ppfjet_twr_clusterind_;
  std::vector<float> ppfjet_twr_hade_, ppfjet_twr_frac_, ppfjet_twr_dR_;
  int ppfjet_cluster_n_;
  std::vector<float> ppfjet_cluster_eta_, ppfjet_cluster_phi_, ppfjet_cluster_dR_;
  int ppfjet_ncandtracks_;
  std::vector<float> ppfjet_candtrack_px_, ppfjet_candtrack_py_, ppfjet_candtrack_pz_, ppfjet_candtrack_EcalE_;
  float pf_dijet_deta_, pf_dijet_dphi_, pf_dijet_balance_;
  float pf_thirdjet_px_, pf_thirdjet_py_;
  int pf_Run_, pf_Lumi_, pf_Event_;
  float pf_weight_;

  // helper functions
  double deltaR(const reco::Jet* j1, const reco::Jet* j2);
  double deltaR(const double eta1, const double phi1, const double eta2, const double phi2);
  int getEtaPhi(const DetId id);
  int getEtaPhi(const HcalDetId id);

  struct CaloJetCorretPairComp {
    inline bool operator() ( const CaloJetCorretPair& a, const CaloJetCorretPair& b) {
      return (a.jet()->pt()*a.scale()) > (b.jet()->pt()*b.scale());
    }
  };

  struct PFJetCorretPairComp {
    inline bool operator() ( const PFJetCorretPair& a, const PFJetCorretPair& b) {
      return (a.jet()->pt()*a.scale()) > (b.jet()->pt()*b.scale());
    }
  };

};


#endif
