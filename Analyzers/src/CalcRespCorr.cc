//
// CalcRespCorr.cc
//
//   description: Calculation for single particle response corrections
//
//   author: J.P. Chou, Brown
//
//

#include "HcalClosureTest/Analyzers/interface/CalcRespCorr.h"
#include "FWCore/Utilities/interface/EDMException.h"

#include "TFile.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TMath.h"

#include <vector>

//
// constructors and destructor
//

CalcRespCorr::CalcRespCorr(const edm::ParameterSet& iConfig)
{
  // set parameters
  clstrCollName_    = iConfig.getParameter<std::string>("clstrCollName");
  rootHistFilename_ = iConfig.getParameter<std::string>("rootHistFilename");
  maxDeltaR_        = iConfig.getParameter<double>("maxDeltaR");
  maxModifiedEMF_   = iConfig.getParameter<double>("maxModifiedEMF");
  respCorr_         = iConfig.getParameter<std::vector<double> >("respCorr");

  if(respCorr_.size()!=83) {
    throw edm::Exception(edm::errors::ProductNotFound)
      << " respCorr has " << respCorr_.size() << " elements.  We want 83.";
  }
  tok_SPC_ = consumes<SingleParticleClusterCollection>(edm::InputTag(clstrCollName_));
}
  
  
CalcRespCorr::~CalcRespCorr()
{
}
  
  
//
// member functions
//
  
// ------------ method called to for each event  ------------
void
CalcRespCorr::analyze(const edm::Event& iEvent, const edm::EventSetup&)
{ 
  edm::Handle<SingleParticleClusterCollection> handle;
  iEvent.getByToken(tok_SPC_,handle);
  if(!handle.isValid()) {
    throw edm::Exception(edm::errors::ProductNotFound)
      << " could not find SingleParticleClusterCollection named " << clstrCollName_ << ".\n";
    return;
  }

  for(SingleParticleClusterCollection::const_iterator it=handle->begin(); it!=handle->end(); ++it) {

    // cluster
    const SingleParticleCluster clstr=(*it);

    // cluster 4-vector
    math::PtEtaPhiMLorentzVector clstrP4 = clstr.clstrP4();

    // gen particle reference
    const edm::Ref<reco::GenParticleCollection> genpRef=clstr.genParticle();

    // towers
    const edm::RefVector<CaloTowerCollection> caloTowerRef=clstr.caloTowers();

    // calculate stuff first
    double deltar=clstr.deltaR();
    double eoverp=clstr.EoverP();
    double emf = clstr.clstrEmEnergy()/(clstr.clstrHadEnergy()+clstr.clstrEmEnergy());
    double modifiedeme=0, modifiedhade=0;
    for(edm::RefVector<CaloTowerCollection>::const_iterator ctit=caloTowerRef.begin();
	ctit!=caloTowerRef.end(); ++ctit) {
      if((*ctit)->id().ietaAbs()<=29) modifiedeme += (*ctit)->emEnergy();
      else                            modifiedhade += (*ctit)->emEnergy();
      modifiedhade += (*ctit)->hadEnergy();
    }
    double modifiedemf = modifiedhade+modifiedeme==0 ? -999 : modifiedeme/(modifiedhade+modifiedeme);
    double correction = modifiedhade==0 ? -999 : (genpRef->p()-modifiedeme)/modifiedhade;
    
    // Fill generated particle plots before selection
    hGenpE_->Fill(genpRef->p());
    hGenpEta_->Fill(genpRef->eta());
    hGenpPhi_->Fill(genpRef->phi());
    hGenpEtaPhi_->Fill(genpRef->eta(), genpRef->phi());

    // Fill cluster plots before selection
    hClstrE_->Fill(clstrP4.e());
    hClstrEta_->Fill(clstrP4.eta());
    hClstrPhi_->Fill(clstrP4.phi());
    hClstrEtaPhi_->Fill(clstrP4.eta(),clstrP4.phi());
    hClstrDeltaR_->Fill(deltar);
    hClstrEMF_->Fill(emf);
    hClstrModifiedEMF_->Fill(modifiedemf);


    //////////////////////////////
    // make cuts here
    //////////////////////////////

    if(modifiedemf > maxModifiedEMF_) continue;
    if(deltar > maxDeltaR_) continue;

    // Fill cluster plots after selection
    hAfterClstrE_->Fill(clstrP4.e());
    hAfterClstrEoverP_->Fill(clstr.EoverP());
    hAfterClstrEta_->Fill(clstrP4.eta());
    hAfterClstrPhi_->Fill(clstrP4.phi());
    hAfterClstrEEta_->Fill(clstrP4.e(), clstrP4.eta());
    hAfterClstrEPhi_->Fill(clstrP4.e(), clstrP4.phi());
    hAfterClstrEoverPEta_->Fill(eoverp, clstrP4.eta());
    hAfterClstrEoverPPhi_->Fill(eoverp, clstrP4.phi());
    
    // Fill response corrections
    edm::RefVector<CaloTowerCollection>::const_iterator highestctit=caloTowerRef.begin();
    for(edm::RefVector<CaloTowerCollection>::const_iterator ctit=caloTowerRef.begin();
	ctit!=caloTowerRef.end(); ++ctit) {
      if((*ctit)->hadEnergy()>(*highestctit)->hadEnergy()) highestctit=ctit;
      hRespIetaAll_->Fill(correction, (*ctit)->id().ieta());
    }
    hRespIetaHighest_->Fill(correction, (*highestctit)->id().ieta());

    // Fill corrected cluster plots after selection
    math::PtEtaPhiMLorentzVector correctedP4;
    for(edm::RefVector<CaloTowerCollection>::const_iterator ctit=caloTowerRef.begin();
	ctit!=caloTowerRef.end(); ++ctit) {
      int ieta = (*ctit)->id().ieta();
      double K = respCorr_[ieta+41];
      double had = (*ctit)->hadEnergy();
      double em = (*ctit)->emEnergy();
      double correction=1;
      if(ieta>29 || ieta<-29) { // in the HF
	correction = K;
      } else {
	correction = (K*had+em)/(had+em);
      }
      correctedP4 += (*ctit)->p4() * correction;
    }
    double corre = correctedP4.e();
    double correoverp = corre/genpRef->p();
    hCorrClstrE_->Fill(corre);
    hCorrClstrEoverP_->Fill(correoverp);
    hCorrClstrEEta_->Fill(corre, correctedP4.eta());
    hCorrClstrEPhi_->Fill(corre, correctedP4.phi());
    hCorrClstrEoverPEta_->Fill(correoverp, correctedP4.eta());
    hCorrClstrEoverPPhi_->Fill(correoverp, correctedP4.phi());

  }   // done looping over clusters
  return;
}


// ------------ method called once each job just before starting event loop  ------------
void 
CalcRespCorr::beginJob()
{
  // book histograms
  rootfile_ = new TFile(rootHistFilename_.c_str(), "RECREATE");

  hGenpE_      = new TH1D("hGenpE","Genp Energy",100,0,100);
  hGenpEta_    = new TH1D("hGenpEta","Genp Eta",100,-5,5);
  hGenpPhi_    = new TH1D("hGenpPhi","Genp Phi",100,-TMath::Pi(),TMath::Pi());
  hGenpEtaPhi_ = new TH2D("hGenpEtaPhi","Genp Eta v. Phi",100,-5,5,100,-TMath::Pi(),TMath::Pi());

  hClstrE_           = new TH1D("hClstrE","Cluster Energy",100,0,100);
  hClstrEta_         = new TH1D("hClstrEta","Cluster Eta",100,-5,5);
  hClstrPhi_         = new TH1D("hClstrPhi","Cluster Phi",100,-TMath::Pi(),TMath::Pi());
  hClstrEtaPhi_      = new TH2D("hClstrEtaPhi","Cluster Eta v. Phi",100,-5,5,100,-TMath::Pi(),TMath::Pi());
  hClstrEMF_         = new TH1D("hClstrEMF","Cluster EMF",100,-2,2);
  hClstrModifiedEMF_ = new TH1D("hClstrModifiedEMF","Cluster Modified EMF",100,0,1);
  hClstrDeltaR_      = new TH1D("hClstrDeltaR","Delta-R between cluster and genp",100,0,5);

  hAfterClstrE_         = new TH1D("hAfterClstrE","Cluster Energy After Selection",100,0,100);
  hAfterClstrEoverP_    = new TH1D("hAfterClstrEoverP","Cluster E/P After Selection",100,0,5);
  hAfterClstrEta_       = new TH1D("hAfterClstrEta","Cluster Eta After Selection",100,-5,5);
  hAfterClstrPhi_       = new TH1D("hhAfterClstrPhi","Cluster Phi After Selection",100,-TMath::Pi(),TMath::Pi());
  hAfterClstrEEta_      = new TH2D("hAfterClstrEEta","Cluster Energy v. Eta After Selection",100,0,100,24,-5,5);
  hAfterClstrEPhi_      = new TH2D("hAfterClstrEPhi","Cluster Energy v. Phi After Selection",100,0,100,24,-TMath::Pi(),TMath::Pi());
  hAfterClstrEoverPEta_ = new TH2D("hAfterClstrEoverPEta","Cluster E/P v. Eta After Selection",100,0,5,24,-5,5);
  hAfterClstrEoverPPhi_ = new TH2D("hAfterClstrEoverPPhi","Cluster E/p v. Phi After Selection",100,0,5,24,-TMath::Pi(),TMath::Pi());

  hRespIetaHighest_ = new TH2D("hRespIetaHighest","Highest Energy Tower Response v. ieta",100,0,5,83,-41.5,41.5);
  hRespIetaCentral_ = new TH2D("hRespIetaCentral","Central Tower Response v. ieta",100,0,5,83,-41.5,41.5);
  hRespIetaAll_     = new TH2D("hRespIetaAll","All Towers Response v. ieta",100,0,5,83,-41.5,41.5);
  
  hCorrClstrE_         = new TH1D("hCorrClstrE","Response Corrected Cluster Energy",100,0,100);
  hCorrClstrEoverP_    = new TH1D("hCorrClstrEoverP","Response Corrected Cluster E/P",100,0,5);
  hCorrClstrEEta_      = new TH2D("hCorrClstrEEta","Response Corrected Cluster Energy v. Eta",100,0,100,24,-5,5);
  hCorrClstrEPhi_      = new TH2D("hCorrClsterEPhi","Response Corrected Cluster Energy v. Phi",100,0,100,24,-TMath::Pi(),TMath::Pi());
  hCorrClstrEoverPEta_ = new TH2D("hCorrClstrEoverPEta","Response Corrected Cluster E/P v. Eta",100,0,5,24,-5,5);
  hCorrClstrEoverPPhi_ = new TH2D("hCorrClsterEoverPPhi","Response Corrected Cluster E/P v. Phi",100,0,5,24,-TMath::Pi(),TMath::Pi());
}

// ------------ method called once each job just after ending the event loop  ------------
void 
CalcRespCorr::endJob() {

  // write histograms
  rootfile_->cd();

  hGenpE_->Write();
  hGenpPhi_->Write();
  hGenpEta_->Write();
  hGenpEtaPhi_->Write();

  hClstrE_->Write();
  hClstrEta_->Write();
  hClstrPhi_->Write();
  hClstrEtaPhi_->Write();
  hClstrEMF_->Write();
  hClstrModifiedEMF_->Write();
  hClstrDeltaR_->Write();

  hAfterClstrE_->Write();
  hAfterClstrEoverP_->Write();
  hAfterClstrEta_->Write();
  hAfterClstrPhi_->Write();
  hAfterClstrEEta_->Write();
  hAfterClstrEPhi_->Write();
  hAfterClstrEoverPEta_->Write();
  hAfterClstrEoverPPhi_->Write();

  hRespIetaHighest_->Write();
  hRespIetaCentral_->Write();
  hRespIetaAll_->Write();
  
  hCorrClstrE_->Write();
  hCorrClstrEoverP_->Write();
  hCorrClstrEEta_->Write();
  hCorrClstrEPhi_->Write();
  hCorrClstrEoverPEta_->Write();
  hCorrClstrEoverPPhi_->Write();

  rootfile_->Close();
}


//define this as a plug-in
DEFINE_FWK_MODULE(CalcRespCorr);
