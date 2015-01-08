//
// CalcRespCorrDiJets.cc
//
//   description: Calculation of dijet response corrections
//
//   author: J.P. Chou, Brown
//
//   updated: David G. Sheffield, Rutgers
//
//

#include "HcalClosureTest/Analyzers/interface/CalcRespCorrDiJets.h"
#include "DataFormats/JetReco/interface/CaloJetCollection.h"
#include "DataFormats/JetReco/interface/PFJetCollection.h"
#include "JetMETCorrections/Objects/interface/JetCorrector.h"
#include "FWCore/Utilities/interface/EDMException.h"
#include "DataFormats/ParticleFlowCandidate/interface/PFCandidate.h"

#include "TTree.h"
#include "TFile.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TClonesArray.h"

#include <vector>
#include <set>
#include <map>

CalcRespCorrDiJets::CalcRespCorrDiJets(const edm::ParameterSet& iConfig)
{
  // set parameters
  caloJetCollName_     = iConfig.getParameter<std::string>("caloJetCollName");
  pfJetCollName_       = iConfig.getParameter<std::string>("pfJetCollName");
  genJetCollName_      = iConfig.getParameter<std::string>("genJetCollName");
  genParticleCollName_ = iConfig.getParameter<std::string>("genParticleCollName");
  genEventInfoName_    = iConfig.getParameter<std::string>("genEventInfoName");
  hbheRecHitName_      = iConfig.getParameter<std::string>("hbheRecHitName");
  hfRecHitName_        = iConfig.getParameter<std::string>("hfRecHitName");
  hoRecHitName_        = iConfig.getParameter<std::string>("hoRecHitName");

  caloJetCorrName_     = iConfig.getParameter<std::string>("caloJetCorrName");
  pfJetCorrName_       = iConfig.getParameter<std::string>("pfJetCorrName");
  rootHistFilename_    = iConfig.getParameter<std::string>("rootHistFilename");
  maxDeltaEta_         = iConfig.getParameter<double>("maxDeltaEta");
  minTagJetEta_        = iConfig.getParameter<double>("minTagJetEta");
  maxTagJetEta_        = iConfig.getParameter<double>("maxTagJetEta");
  minSumJetEt_         = iConfig.getParameter<double>("minSumJetEt");
  minJetEt_            = iConfig.getParameter<double>("minJetEt");
  maxThirdJetEt_       = iConfig.getParameter<double>("maxThirdJetEt");
  maxJetEMF_           = iConfig.getParameter<double>("maxJetEMF");
  doCaloJets_          = iConfig.getParameter<bool>("doCaloJets");
  doPFJets_            = iConfig.getParameter<bool>("doPFJets");
  doGenJets_           = iConfig.getParameter<bool>("doGenJets");
  debug_               = iConfig.getUntrackedParameter<bool>("debug", false);

  tok_CaloJet_   = consumes<reco::CaloJetCollection>        (caloJetCollName_);
  tok_PFJet_     = consumes<reco::PFJetCollection>          (pfJetCollName_);
  tok_GenJet_    = consumes<std::vector<reco::GenJet> >     (genJetCollName_);
  tok_GenPart_   = consumes<std::vector<reco::GenParticle> >(genParticleCollName_); 
  tok_GenEvInfo_ = consumes<GenEventInfoProduct>            (genEventInfoName_);
  tok_HBHE_      = consumes<edm::SortedCollection<HBHERecHit,edm::StrictWeakOrdering<HBHERecHit> > > (hbheRecHitName_);
  tok_HF_        = consumes<edm::SortedCollection<HFRecHit,edm::StrictWeakOrdering<HFRecHit> > >   (hfRecHitName_);
  tok_HO_        = consumes<edm::SortedCollection<HORecHit,edm::StrictWeakOrdering<HORecHit> > >   (hoRecHitName_);
}

CalcRespCorrDiJets::~CalcRespCorrDiJets()
{
}
  
//
// member functions
//
  
// ------------ method called to for each event  ------------
void
CalcRespCorrDiJets::analyze(const edm::Event& iEvent, const edm::EventSetup& evSetup)
{ 
  edm::Handle<std::vector<reco::GenJet>> genjets;
  edm::Handle<std::vector<reco::GenParticle> > genparticles;
  if(doGenJets_){
    // Get GenJets
    iEvent.getByToken(tok_GenJet_,genjets);
    if(!genjets.isValid()) {
      throw edm::Exception(edm::errors::ProductNotFound)
	<< " could not find GenJet vector named " << genJetCollName_ << ".\n";
      return;
    }

    // Get GenParticles
    iEvent.getByToken(tok_GenPart_,genparticles);
    if(!genparticles.isValid()) {
      throw edm::Exception(edm::errors::ProductNotFound)
	<< " could not find GenParticle vector named " << genParticleCollName_ << ".\n";
      return;
    }

    // Get weights
    edm::Handle<GenEventInfoProduct> genEventInfoProduct;
    iEvent.getByToken(tok_GenEvInfo_, genEventInfoProduct);
    if(!genEventInfoProduct.isValid()){
      throw edm::Exception(edm::errors::ProductNotFound)
	<< " could not find GenEventInfoProduct named " << genEventInfoName_ << " \n";
      return;
    }
    pf_weight_ = genEventInfoProduct->weight();
  }

  // Run over CaloJets
  if(doCaloJets_){
    calo_Event_ = iEvent.id().event();
    
    // Get CaloJets
    edm::Handle<reco::CaloJetCollection> calojets;
    iEvent.getByToken(tok_CaloJet_,calojets);
    if(!calojets.isValid()) {
      throw edm::Exception(edm::errors::ProductNotFound)
	<< " could not find CaloJetCollection named " << caloJetCollName_ << ".\n";
      return;
    }

    // Get jet corrections
    const JetCorrector* correctorCalo = JetCorrector::getJetCorrector(caloJetCorrName_,evSetup);

    //////////////////////////////
    // Event Selection
    //////////////////////////////
    
    // determine which cut results in failure
    int passSelCalo=0;
    
    // sort jets by corrected et
    std::set<CaloJetCorretPair, CaloJetCorretPairComp> calojetcorretpairset;
    for(reco::CaloJetCollection::const_iterator it=calojets->begin(); it!=calojets->end(); ++it) {
      const reco::CaloJet* jet=&(*it);
      calojetcorretpairset.insert( CaloJetCorretPair(jet, correctorCalo->correction(jet->p4())) );
    }
    
    // find highest two (corrected) et jets
    CaloJetCorretPair calo_tag, calo_probe;
    calo_thirdjet_px_=calo_thirdjet_py_=0.0;
    int cntr=0;
    for(std::set<CaloJetCorretPair, CaloJetCorretPairComp>::const_iterator it=calojetcorretpairset.begin(); it!=calojetcorretpairset.end(); ++it) {
      CaloJetCorretPair jet=(*it);
      ++cntr;
      if(cntr==1) calo_tag=jet;
      else if(cntr==2) calo_probe=jet;
      else {
	calo_thirdjet_px_ += jet.scale()*jet.jet()->px();
	calo_thirdjet_py_ += jet.scale()*jet.jet()->py();
      }
    }
    
    if(calo_tag.jet() && calo_probe.jet()){
      // require that the first two jets are above some minimum,
      // and the rest are below some maximum
      if((calo_tag.jet()->et()+calo_probe.jet()->et())<minSumJetEt_) passSelCalo |= 0x1;
      if(calo_tag.jet()->et()<minJetEt_ || calo_probe.jet()->et()<minJetEt_) passSelCalo |= 0x2;
      if(sqrt(calo_thirdjet_px_*calo_thirdjet_px_ + calo_thirdjet_py_*calo_thirdjet_py_)>maxThirdJetEt_) passSelCalo |= 0x4;
      
      // force the tag jet to have the smaller |eta|
      if(std::fabs(calo_tag.jet()->eta())>std::fabs(calo_probe.jet()->eta())) {
	CaloJetCorretPair temp=calo_tag;
	calo_tag=calo_probe;
	calo_probe=temp;
      }
      
      // eta cuts
      double dAbsEta=std::fabs(std::fabs(calo_tag.jet()->eta())-std::fabs(calo_probe.jet()->eta()));
      if(dAbsEta>maxDeltaEta_) passSelCalo |= 0x8;
      if(fabs(calo_tag.jet()->eta())<minTagJetEta_) passSelCalo |= 0x10;
      if(fabs(calo_tag.jet()->eta())>maxTagJetEta_) passSelCalo |= 0x10;
      
      // emf cuts
      if(calo_tag.jet()->emEnergyFraction()>maxJetEMF_) passSelCalo |= 0x20;
      if(calo_probe.jet()->emEnergyFraction()>maxJetEMF_) passSelCalo |= 0x20;
    }
    else{
      passSelCalo = 0x40;
    }

    // make the cuts
    hPassSelCalo_->Fill(passSelCalo);
    if(!passSelCalo){
      
      // dump
      if(debug_) {
	std::cout << "Run: " << iEvent.id().run() << "; Event: " << iEvent.id().event() << std::endl;
	for(reco::CaloJetCollection::const_iterator it=calojets->begin(); it!=calojets->end(); ++it) {
	  const reco::CaloJet *jet=&(*it);
	  std::cout << "istag=" << (jet==calo_tag.jet()) << "; isprobe=" << (jet==calo_probe.jet()) << "; et=" << jet->et() << "; eta=" << jet->eta() << std::endl;
	}
      }
      
      // fill tag jet variables
      tcalojet_pt_    = calo_tag.jet()->pt();
      tcalojet_p_     = calo_tag.jet()->p();
      tcalojet_eta_   = calo_tag.jet()->eta();
      tcalojet_phi_   = calo_tag.jet()->phi();
      tcalojet_emf_   = calo_tag.jet()->emEnergyFraction();
      tcalojet_scale_ = calo_tag.scale();
      tcalojet_EBE_   = calo_tag.jet()->emEnergyInEB();
      tcalojet_EEE_   = calo_tag.jet()->emEnergyInEE();
      tcalojet_HBE_   = calo_tag.jet()->hadEnergyInHB();
      tcalojet_HEE_   = calo_tag.jet()->hadEnergyInHE();
      tcalojet_HFE_   = calo_tag.jet()->emEnergyInHF() + calo_tag.jet()->hadEnergyInHF();
      tcalojet_ntwrs_=0;
      std::vector<CaloTowerPtr> tagconst=calo_tag.jet()->getCaloConstituents();
      for(std::vector<CaloTowerPtr>::const_iterator it=tagconst.begin(); it!=tagconst.end(); ++it) {
	int ieta=(*it)->id().ieta();
	int ietaAbs=(*it)->id().ietaAbs();
	tcalojet_twr_ieta_[tcalojet_ntwrs_]=ieta;
	if(ietaAbs<=29) {
	  tcalojet_twr_eme_[tcalojet_ntwrs_] = (*it)->emEnergy();
	  tcalojet_twr_hade_[tcalojet_ntwrs_] = (*it)->hadEnergy();
	} else {
	  tcalojet_twr_eme_[tcalojet_ntwrs_] = 0;
	  tcalojet_twr_hade_[tcalojet_ntwrs_] = (*it)->emEnergy()+(*it)->hadEnergy();
	}
	++tcalojet_ntwrs_;
      }
      
      // fill probe jet variables
      pcalojet_pt_    = calo_probe.jet()->pt();
      pcalojet_p_     = calo_probe.jet()->p();
      pcalojet_eta_   = calo_probe.jet()->eta();
      pcalojet_phi_   = calo_probe.jet()->phi();
      pcalojet_emf_   = calo_probe.jet()->emEnergyFraction();
      pcalojet_scale_ = calo_probe.scale();
      pcalojet_EBE_   = calo_probe.jet()->emEnergyInEB();
      pcalojet_EEE_   = calo_probe.jet()->emEnergyInEE();
      pcalojet_HBE_   = calo_probe.jet()->hadEnergyInHB();
      pcalojet_HEE_   = calo_probe.jet()->hadEnergyInHE();
      pcalojet_HFE_   = calo_probe.jet()->emEnergyInHF() + calo_probe.jet()->hadEnergyInHF();
      pcalojet_ntwrs_=0;
      std::vector<CaloTowerPtr> probeconst=calo_probe.jet()->getCaloConstituents();
      for(std::vector<CaloTowerPtr>::const_iterator it=probeconst.begin(); it!=probeconst.end(); ++it) {
	int ieta=(*it)->id().ieta();
	int ietaAbs=(*it)->id().ietaAbs();
	pcalojet_twr_ieta_[pcalojet_ntwrs_]=ieta;
	if(ietaAbs<=29) {
	  pcalojet_twr_eme_[pcalojet_ntwrs_] = (*it)->emEnergy();
	  pcalojet_twr_hade_[pcalojet_ntwrs_] = (*it)->hadEnergy();
	} else {
	  pcalojet_twr_eme_[pcalojet_ntwrs_] = 0;
	  pcalojet_twr_hade_[pcalojet_ntwrs_] = (*it)->emEnergy()+(*it)->hadEnergy();
	}
	++pcalojet_ntwrs_;
      }
      
      if(doGenJets_){
	// fill genjet tag/probe variables
	tcalojet_gendr_ = 99999.;
	tcalojet_genpt_ = 0;
	tcalojet_genp_  = 0;
	pcalojet_gendr_ = 99999.;
	pcalojet_genpt_ = 0;
	pcalojet_genp_  = 0;
	for(std::vector<reco::GenJet>::const_iterator it=genjets->begin(); it!=genjets->end(); ++it){
	  const reco::GenJet* jet=&(*it);
	  double dr=deltaR(jet, calo_probe.jet());
	  if(dr<pcalojet_gendr_) {
	    pcalojet_gendr_ = dr;
	    pcalojet_genpt_ = jet->pt();
	    pcalojet_genp_ = jet->p();
	  }
	  dr=deltaR(jet, calo_tag.jet());
	  if(dr<tcalojet_gendr_) {
	    tcalojet_gendr_ = dr;
	    tcalojet_genpt_ = jet->pt();
	    tcalojet_genp_ = jet->p();
	  }
	}
      }
      
      // fill dijet variables
      calo_dijet_deta_=std::fabs(std::fabs(calo_tag.jet()->eta())-std::fabs(calo_probe.jet()->eta()));
      calo_dijet_dphi_=calo_tag.jet()->phi()-calo_probe.jet()->phi();
      if(calo_dijet_dphi_>3.1415) calo_dijet_dphi_ = 6.2832-calo_dijet_dphi_;
      calo_dijet_balance_ = (tcalojet_pt_-pcalojet_pt_)/(tcalojet_pt_+pcalojet_pt_);
      
      calo_tree_->Fill();
    }
  }

  // Run over PFJets
  if(doPFJets_){
    unsigned int debugEvent = 0;
    
    pf_Run_ = iEvent.id().run();
    pf_Lumi_ = iEvent.id().luminosityBlock();
    pf_Event_ = iEvent.id().event();
    
    // Get PFJets
    edm::Handle<reco::PFJetCollection> pfjets;
    iEvent.getByToken(tok_PFJet_,pfjets);
    if(!pfjets.isValid()) {
      throw edm::Exception(edm::errors::ProductNotFound)
	<< " could not find PFJetCollection named " << pfJetCollName_ << ".\n";
      return;
    }

    // Get RecHits in HB and HE
    edm::Handle<edm::SortedCollection<HBHERecHit,edm::StrictWeakOrdering<HBHERecHit>>> hbhereco;
    iEvent.getByToken(tok_HBHE_,hbhereco);
    if(!hbhereco.isValid()) {
      throw edm::Exception(edm::errors::ProductNotFound)
	<< " could not find HBHERecHit named " << hbheRecHitName_ << ".\n";
      return;
    }
    
    // Get RecHits in HF
    edm::Handle<edm::SortedCollection<HFRecHit,edm::StrictWeakOrdering<HFRecHit>>> hfreco;
    iEvent.getByToken(tok_HF_,hfreco);
    if(!hfreco.isValid()) {
      throw edm::Exception(edm::errors::ProductNotFound)
	<< " could not find HFRecHit named " << hfRecHitName_ << ".\n";
      return;
    }

    // Get RecHits in HO
    edm::Handle<edm::SortedCollection<HORecHit,edm::StrictWeakOrdering<HORecHit>>> horeco;
    iEvent.getByToken(tok_HO_,horeco);
    if(!horeco.isValid()) {
      throw edm::Exception(edm::errors::ProductNotFound)
	<< " could not find HORecHit named " << hoRecHitName_ << ".\n";
      return;
    }

    // Get geometry
    edm::ESHandle<CaloGeometry> geoHandle;
    evSetup.get<CaloGeometryRecord>().get(geoHandle);
    const CaloSubdetectorGeometry *HBGeom = geoHandle->getSubdetectorGeometry(DetId::Hcal, 1);
    const CaloSubdetectorGeometry *HEGeom = geoHandle->getSubdetectorGeometry(DetId::Hcal, 2);
    const CaloSubdetectorGeometry *HOGeom = geoHandle->getSubdetectorGeometry(DetId::Hcal, 3);
    const CaloSubdetectorGeometry *HFGeom = geoHandle->getSubdetectorGeometry(DetId::Hcal, 4);
    
    int HBHE_n = 0;
    for(edm::SortedCollection<HBHERecHit,edm::StrictWeakOrdering<HBHERecHit>>::const_iterator ith=hbhereco->begin(); ith!=hbhereco->end(); ++ith){
      HBHE_n++;
      h_hbherecoieta_->Fill((*ith).id().ieta());
      if(iEvent.id().event() == debugEvent){
	std::cout << (*ith).id().ieta() << " " << (*ith).id().iphi() << std::endl;
	h_rechitspos_->Fill((*ith).id().ieta(), (*ith).id().iphi());
      }
    }
    int HF_n = 0;
    for(edm::SortedCollection<HFRecHit,edm::StrictWeakOrdering<HFRecHit>>::const_iterator ith=hfreco->begin(); ith!=hfreco->end(); ++ith){
      HF_n++;
      if(iEvent.id().event() == debugEvent){
	h_rechitspos_->Fill((*ith).id().ieta(), (*ith).id().iphi());
      }
    }
    int HO_n = 0;
    for(edm::SortedCollection<HORecHit,edm::StrictWeakOrdering<HORecHit>>::const_iterator ith=horeco->begin(); ith!=horeco->end(); ++ith){
      HO_n++;
      if(iEvent.id().event() == debugEvent){
	h_rechitspos_->Fill((*ith).id().ieta(), (*ith).id().iphi());
      }
    }
    h_HBHE_n_->Fill(HBHE_n);
    h_HF_n_->Fill(HF_n);
    h_HO_n_->Fill(HO_n);
    
    // Get jet corrections
    const JetCorrector* correctorPF = JetCorrector::getJetCorrector(pfJetCorrName_,evSetup);
    
    //////////////////////////////
    // Event Selection
    //////////////////////////////
    
    // determine which cut results in failure
    int passSelPF=0;

    // sort jets by corrected et
    std::set<PFJetCorretPair, PFJetCorretPairComp> pfjetcorretpairset;
    for(reco::PFJetCollection::const_iterator it=pfjets->begin(); it!=pfjets->end(); ++it) {
      const reco::PFJet* jet=&(*it);
      pfjetcorretpairset.insert( PFJetCorretPair(jet, correctorPF->correction(jet->p4())) );
    }

    PFJetCorretPair pf_tag, pf_probe;
    pf_thirdjet_px_=pf_thirdjet_py_=0.0;
    int cntr=0;
    for(std::set<PFJetCorretPair, PFJetCorretPairComp>::const_iterator it=pfjetcorretpairset.begin(); it!=pfjetcorretpairset.end(); ++it) {
      PFJetCorretPair jet=(*it);
      ++cntr;
      if(cntr==1) pf_tag=jet;
      else if(cntr==2) pf_probe=jet;
      else {
	pf_thirdjet_px_ += jet.scale()*jet.jet()->px();
	pf_thirdjet_py_ += jet.scale()*jet.jet()->py();
      }
    }
    
    if(pf_tag.jet() && pf_probe.jet()){
    // require that the first two jets are above some minimum,
    // and the rest are below some maximum
      if((pf_tag.jet()->et()+pf_probe.jet()->et())<minSumJetEt_) passSelPF |= 0x1;
      if(pf_tag.jet()->et()<minJetEt_ || pf_probe.jet()->et()<minJetEt_) passSelPF |= 0x2;
      if(sqrt(pf_thirdjet_px_*pf_thirdjet_px_ + pf_thirdjet_py_*pf_thirdjet_py_)>maxThirdJetEt_) passSelPF |= 0x4;
      
      // force the tag jet to have the smaller |eta|
      if(std::fabs(pf_tag.jet()->eta())>std::fabs(pf_probe.jet()->eta())) {
	PFJetCorretPair temp=pf_tag;
	pf_tag=pf_probe;
	pf_probe=temp;
      }
      
      // eta cuts
      double dAbsEta=std::fabs(std::fabs(pf_tag.jet()->eta())-std::fabs(pf_probe.jet()->eta()));
      if(dAbsEta>maxDeltaEta_) passSelPF |= 0x8;
      if(fabs(pf_tag.jet()->eta())<minTagJetEta_) passSelPF |= 0x10;
      if(fabs(pf_tag.jet()->eta())>maxTagJetEta_) passSelPF |= 0x10;
    }
    else{
      passSelPF = 0x40;
    }
    
    hPassSelPF_->Fill(passSelPF);
    if(!passSelPF){
      // dump
      if(debug_) {
	std::cout << "Run: " << iEvent.id().run() << "; Event: " << iEvent.id().event() << std::endl;
	for(reco::PFJetCollection::const_iterator it=pfjets->begin(); it!=pfjets->end(); ++it) {
	  const reco::PFJet *jet=&(*it);
	  std::cout << "istag=" << (jet==pf_tag.jet()) << "; isprobe=" << (jet==pf_probe.jet()) << "; et=" << jet->et() << "; eta=" << jet->eta() << std::endl;
	}
      }

      // Reset particle variables
      tpfjet_unkown_E_ = tpfjet_unkown_px_ = tpfjet_unkown_py_ = tpfjet_unkown_pz_ = tpfjet_unkown_EcalE_ = 0.0;
      tpfjet_electron_E_ = tpfjet_electron_px_ = tpfjet_electron_py_ = tpfjet_electron_pz_ = tpfjet_electron_EcalE_ = 0.0;
      tpfjet_muon_E_ = tpfjet_muon_px_ = tpfjet_muon_py_ = tpfjet_muon_pz_ = tpfjet_muon_EcalE_ = 0.0;
      tpfjet_photon_E_ = tpfjet_photon_px_ = tpfjet_photon_py_ = tpfjet_photon_pz_ = tpfjet_photon_EcalE_ = 0.0;
      tpfjet_unkown_n_ = tpfjet_electron_n_ = tpfjet_muon_n_ = tpfjet_photon_n_ = 0;
      tpfjet_had_n_ = 0;
      tpfjet_cluster_n_ = 0;
      ppfjet_unkown_E_ = ppfjet_unkown_px_ = ppfjet_unkown_py_ = ppfjet_unkown_pz_ = ppfjet_unkown_EcalE_ = 0.0;
      ppfjet_electron_E_ = ppfjet_electron_px_ = ppfjet_electron_py_ = ppfjet_electron_pz_ = ppfjet_electron_EcalE_ = 0.0;
      ppfjet_muon_E_ = ppfjet_muon_px_ = ppfjet_muon_py_ = ppfjet_muon_pz_ = ppfjet_muon_EcalE_ = 0.0;
      ppfjet_photon_E_ = ppfjet_photon_px_ = ppfjet_photon_py_ = ppfjet_photon_pz_ = ppfjet_photon_EcalE_ = 0.0;
      ppfjet_unkown_n_ = ppfjet_electron_n_ = ppfjet_muon_n_ = ppfjet_photon_n_ = 0;
      ppfjet_had_n_ = 0;
      ppfjet_cluster_n_ = 0;

      tpfjet_had_E_.clear();
      tpfjet_had_px_.clear();
      tpfjet_had_py_.clear();
      tpfjet_had_pz_.clear();
      tpfjet_had_EcalE_.clear();
      tpfjet_had_rawHcalE_.clear();
      tpfjet_had_emf_.clear();
      tpfjet_had_E_mctruth_.clear();
      tpfjet_had_id_.clear();
      tpfjet_had_candtrackind_.clear();
      tpfjet_had_mcpdgId_.clear();
      tpfjet_had_ntwrs_.clear();
      tpfjet_twr_ieta_.clear();
      tpfjet_twr_iphi_.clear();
      tpfjet_twr_depth_.clear();
      tpfjet_twr_subdet_.clear();
      tpfjet_twr_candtrackind_.clear();
      tpfjet_twr_hadind_.clear();
      tpfjet_twr_elmttype_.clear();
      tpfjet_twr_hade_.clear();
      tpfjet_twr_frac_.clear();
      tpfjet_twr_dR_.clear();
      tpfjet_twr_clusterind_.clear();
      tpfjet_cluster_eta_.clear();
      tpfjet_cluster_phi_.clear();
      tpfjet_cluster_dR_.clear();
      tpfjet_candtrack_px_.clear();
      tpfjet_candtrack_py_.clear();
      tpfjet_candtrack_pz_.clear();
      tpfjet_candtrack_EcalE_.clear();
      ppfjet_had_E_.clear();
      ppfjet_had_px_.clear();
      ppfjet_had_py_.clear();
      ppfjet_had_pz_.clear();
      ppfjet_had_EcalE_.clear();
      ppfjet_had_rawHcalE_.clear();
      ppfjet_had_emf_.clear();
      ppfjet_had_E_mctruth_.clear();
      ppfjet_had_id_.clear();
      ppfjet_had_candtrackind_.clear();
      ppfjet_had_mcpdgId_.clear();
      ppfjet_had_ntwrs_.clear();
      ppfjet_twr_ieta_.clear();
      ppfjet_twr_iphi_.clear();
      ppfjet_twr_depth_.clear();
      ppfjet_twr_subdet_.clear();
      ppfjet_twr_candtrackind_.clear();
      ppfjet_twr_hadind_.clear();
      ppfjet_twr_elmttype_.clear();
      ppfjet_twr_hade_.clear();
      ppfjet_twr_frac_.clear();
      ppfjet_twr_dR_.clear();
      ppfjet_twr_clusterind_.clear();
      ppfjet_cluster_eta_.clear();
      ppfjet_cluster_phi_.clear();
      ppfjet_cluster_dR_.clear();
      ppfjet_candtrack_px_.clear();
      ppfjet_candtrack_py_.clear();
      ppfjet_candtrack_pz_.clear();
      ppfjet_candtrack_EcalE_.clear();

      std::map<int,std::pair<int,std::set<float>>> tpfjet_rechits;
      std::map<int,std::pair<int,std::set<float>>> ppfjet_rechits;
      std::map<float,int> tpfjet_clusters;
      std::map<float,int> ppfjet_clusters;
      
      // fill tag jet variables
      tpfjet_pt_    = pf_tag.jet()->pt();
      tpfjet_p_     = pf_tag.jet()->p();
      tpfjet_E_     = pf_tag.jet()->energy();
      tpfjet_eta_   = pf_tag.jet()->eta();
      tpfjet_phi_   = pf_tag.jet()->phi();
      tpfjet_scale_ = pf_tag.scale();
      tpfjet_ntwrs_=0;
      tpfjet_ncandtracks_=0;

      if(iEvent.id().event() == debugEvent){
	std::cout << "Tag eta: " << tpfjet_eta_ << " phi: " << tpfjet_phi_ << std::endl;
      }
      
      //std::cout << pf_tag.jet()->print() << std::endl;
      int types = 0;
      int ntypes = 0;
      
      /////////////////////////////////////////////
      // Get PF constituents and fill HCAL towers
      /////////////////////////////////////////////
      
      // Get tag PFCandidates
      std::vector<reco::PFCandidatePtr> tagconst=pf_tag.jet()->getPFConstituents();
      for(std::vector<reco::PFCandidatePtr>::const_iterator it=tagconst.begin(); it!=tagconst.end(); ++it){
	bool hasTrack = false;
	// Test PFCandidate type
	reco::PFCandidate::ParticleType candidateType = (*it)->particleId();
	switch(candidateType){
	case reco::PFCandidate::X:
	  tpfjet_unkown_E_ += (*it)->energy();
	  tpfjet_unkown_px_ += (*it)->px();
	  tpfjet_unkown_py_ += (*it)->py();
	  tpfjet_unkown_pz_ += (*it)->pz();
	  tpfjet_unkown_EcalE_ += (*it)->ecalEnergy();
	  tpfjet_unkown_n_++;
	  continue;
	case reco::PFCandidate::h:
	  {
	    tpfjet_had_E_.push_back((*it)->energy());
	    tpfjet_had_px_.push_back((*it)->px());
	    tpfjet_had_py_.push_back((*it)->py());
	    tpfjet_had_pz_.push_back((*it)->pz());
	    tpfjet_had_EcalE_.push_back((*it)->ecalEnergy());
	    tpfjet_had_rawHcalE_.push_back((*it)->rawHcalEnergy());
	    tpfjet_had_id_.push_back(0);
	    tpfjet_had_ntwrs_.push_back(0);
	    tpfjet_had_n_++;

	    if(doGenJets_){
	      float gendr = 99999;
	      float genE = 0;
	      int genpdgId = 0;
	      for(std::vector<reco::GenParticle>::const_iterator itmc = genparticles->begin(); itmc != genparticles->end(); itmc++){
		if(itmc->status() == 1 && itmc->pdgId() > 100){
		  double dr = deltaR((*it)->eta(),(*it)->phi(),itmc->eta(),itmc->phi());
		  if(dr < gendr){
		    gendr = dr;
		    genE = itmc->energy();
		    genpdgId = itmc->pdgId();
		  }
		}
	      }
	      tpfjet_had_E_mctruth_.push_back(genE);
	      tpfjet_had_mcpdgId_.push_back(genpdgId);
	    }
	    
	    reco::TrackRef trackRef = (*it)->trackRef();
	    if(trackRef.isNonnull()){
	      reco::Track track = *trackRef;
	      tpfjet_candtrack_px_.push_back(track.px());
	      tpfjet_candtrack_py_.push_back(track.py());
	      tpfjet_candtrack_pz_.push_back(track.pz());
	      tpfjet_candtrack_EcalE_.push_back((*it)->ecalEnergy());
	      tpfjet_had_candtrackind_.push_back(tpfjet_ncandtracks_);
	      hasTrack = true;
	      tpfjet_ncandtracks_++;
	    }
	    else{
	      tpfjet_had_candtrackind_.push_back(-2);
	    }
	  }
	  break;
	case reco::PFCandidate::e:
	  tpfjet_electron_E_ += (*it)->energy();
	  tpfjet_electron_px_ += (*it)->px();
	  tpfjet_electron_py_ += (*it)->py();
	  tpfjet_electron_pz_ += (*it)->pz();
	  tpfjet_electron_EcalE_ += (*it)->ecalEnergy();
	  tpfjet_electron_n_++;
	  continue;
	case reco::PFCandidate::mu:
	  tpfjet_muon_E_ += (*it)->energy();
	  tpfjet_muon_px_ += (*it)->px();
	  tpfjet_muon_py_ += (*it)->py();
	  tpfjet_muon_pz_ += (*it)->pz();
	  tpfjet_muon_EcalE_ += (*it)->ecalEnergy();
	  tpfjet_muon_n_++;
	  continue;
	case reco::PFCandidate::gamma:
	  tpfjet_photon_E_ += (*it)->energy();
	  tpfjet_photon_px_ += (*it)->px();
	  tpfjet_photon_py_ += (*it)->py();
	  tpfjet_photon_pz_ += (*it)->pz();
	  tpfjet_photon_EcalE_ += (*it)->ecalEnergy();
	  tpfjet_photon_n_++;
	  continue;
	case reco::PFCandidate::h0:
	  {
	    tpfjet_had_E_.push_back((*it)->energy());
	    tpfjet_had_px_.push_back((*it)->px());
	    tpfjet_had_py_.push_back((*it)->py());
	    tpfjet_had_pz_.push_back((*it)->pz());
	    tpfjet_had_EcalE_.push_back((*it)->ecalEnergy());
	    tpfjet_had_rawHcalE_.push_back((*it)->rawHcalEnergy());
	    tpfjet_had_id_.push_back(1);
	    tpfjet_had_candtrackind_.push_back(-1);
	    tpfjet_had_ntwrs_.push_back(0);
	    tpfjet_had_n_++;
	    
	    if(doGenJets_){
	      float gendr = 99999;
	      float genE = 0;
	      int genpdgId = 0;
	      for(std::vector<reco::GenParticle>::const_iterator itmc = genparticles->begin(); itmc != genparticles->end(); itmc++){
		if(itmc->status() == 1 && itmc->pdgId() > 100){
		  double dr = deltaR((*it)->eta(),(*it)->phi(),itmc->eta(),itmc->phi());
		  if(dr < gendr){
		    gendr = dr;
		    genE = itmc->energy();
		    genpdgId = itmc->pdgId();
		  }
		}
	      }
	      tpfjet_had_E_mctruth_.push_back(genE);
	      tpfjet_had_mcpdgId_.push_back(genpdgId);
	    }
	    
	    break;
	  }
	case reco::PFCandidate::h_HF:
	  {
	    tpfjet_had_E_.push_back((*it)->energy());
	    tpfjet_had_px_.push_back((*it)->px());
	    tpfjet_had_py_.push_back((*it)->py());
	    tpfjet_had_pz_.push_back((*it)->pz());
	    tpfjet_had_EcalE_.push_back((*it)->ecalEnergy());
	    tpfjet_had_rawHcalE_.push_back((*it)->rawHcalEnergy());
	    tpfjet_had_id_.push_back(2);
	    tpfjet_had_candtrackind_.push_back(-1);
	    tpfjet_had_ntwrs_.push_back(0);
	    tpfjet_had_n_++;
	    
	    if(doGenJets_){
	      float gendr = 99999;
	      float genE = 0;
	      int genpdgId = 0;
	      for(std::vector<reco::GenParticle>::const_iterator itmc = genparticles->begin(); itmc != genparticles->end(); itmc++){
		if(itmc->status() == 1 && itmc->pdgId() > 100){
		  double dr = deltaR((*it)->eta(),(*it)->phi(),itmc->eta(),itmc->phi());
		  if(dr < gendr){
		    gendr = dr;
		    genE = itmc->energy();
		    genpdgId = itmc->pdgId();
		  }
		}
	      }
	      tpfjet_had_E_mctruth_.push_back(genE);
	      tpfjet_had_mcpdgId_.push_back(genpdgId);
	    }
	    
	    break;
	  }
	case reco::PFCandidate::egamma_HF:
	  {
	    tpfjet_had_E_.push_back((*it)->energy());
	    tpfjet_had_px_.push_back((*it)->px());
	    tpfjet_had_py_.push_back((*it)->py());
	    tpfjet_had_pz_.push_back((*it)->pz());
	    tpfjet_had_EcalE_.push_back((*it)->ecalEnergy());
	    tpfjet_had_rawHcalE_.push_back((*it)->rawHcalEnergy());
	    tpfjet_had_id_.push_back(3);
	    tpfjet_had_candtrackind_.push_back(-1);
	    tpfjet_had_ntwrs_.push_back(0);
	    tpfjet_had_n_++;

	    if(doGenJets_){
	      float gendr = 99999;
	      float genE = 0;
	      int genpdgId = 0;
	      for(std::vector<reco::GenParticle>::const_iterator itmc = genparticles->begin(); itmc != genparticles->end(); itmc++){
		if(itmc->status() == 1 && itmc->pdgId() > 100){
		  double dr = deltaR((*it)->eta(),(*it)->phi(),itmc->eta(),itmc->phi());
		  if(dr < gendr){
		    gendr = dr;
		    genE = itmc->energy();
		    genpdgId = itmc->pdgId();
		  }
		}
	      }
	      tpfjet_had_E_mctruth_.push_back(genE);
	      tpfjet_had_mcpdgId_.push_back(genpdgId);
	    }
	    
	    break;
	  }
	}
	
	std::map<int,int> twrietas;
	float HFHAD_E = 0;
	float HFEM_E = 0;
	int HFHAD_n_ = 0;
	int HFEM_n_ = 0;
	int HF_type_ = 0;
	int maxElement=(*it)->elementsInBlocks().size();
	for(int e=0; e<maxElement; ++e){
	  // Get elements from block
	  reco::PFBlockRef blockRef = (*it)->elementsInBlocks()[e].first;
	  const edm::OwnVector<reco::PFBlockElement>& elements = blockRef->elements();
	  for(unsigned iEle=0; iEle<elements.size(); iEle++) {
	    if(elements[iEle].index() == (*it)->elementsInBlocks()[e].second){
	      if(elements[iEle].type() == reco::PFBlockElement::HCAL){ // Element is HB or HE
		types |= 0x1;
		ntypes++;
		HF_type_ |= 0x1;
		// Get cluster and hits
		reco::PFClusterRef clusterref = elements[iEle].clusterRef();
		reco::PFCluster cluster = *clusterref;
		double cluster_dR = deltaR(tpfjet_eta_,tpfjet_phi_,cluster.eta(),cluster.phi());
		if(tpfjet_clusters.count(cluster_dR) == 0){
		  tpfjet_clusters[cluster_dR] = tpfjet_cluster_n_;
		  tpfjet_cluster_eta_.push_back(cluster.eta());
		  tpfjet_cluster_phi_.push_back(cluster.phi());
		  tpfjet_cluster_dR_.push_back(cluster_dR);
		  tpfjet_cluster_n_++;
		}
		int cluster_ind = tpfjet_clusters[cluster_dR];

		std::vector<std::pair<DetId,float>> hitsAndFracs = cluster.hitsAndFractions();
		
		// Run over hits and match
		int nHits = hitsAndFracs.size();
		for(int iHit=0; iHit<nHits; iHit++){
		  int etaPhiPF = getEtaPhi(hitsAndFracs[iHit].first);
		  
		  int tmpzside = ((hitsAndFracs[iHit].first.rawId() >> 13) & 0x1) ? 1 : -1;
		  int tmpieta = ((hitsAndFracs[iHit].first.rawId() >> 7) & 0x3F);
		  h_ietaHCAL_->Fill(tmpzside*tmpieta);

		  for(edm::SortedCollection<HBHERecHit,edm::StrictWeakOrdering<HBHERecHit>>::const_iterator ith=hbhereco->begin(); ith!=hbhereco->end(); ++ith){
		    int etaPhiRecHit = getEtaPhi((*ith).id());
		    if(etaPhiPF == etaPhiRecHit){
		      tpfjet_had_ntwrs_.at(tpfjet_had_n_ - 1)++;
		      if(tpfjet_rechits.count((*ith).id()) == 0){
			tpfjet_twr_ieta_.push_back((*ith).id().ieta());
			tpfjet_twr_iphi_.push_back((*ith).id().iphi());
			tpfjet_twr_depth_.push_back((*ith).id().depth());
			tpfjet_twr_subdet_.push_back((*ith).id().subdet());
			if(hitsAndFracs[iHit].second > 0.05 && (*ith).energy() > 0.0) twrietas[(*ith).id().ieta()]++;
			tpfjet_twr_hade_.push_back((*ith).energy());
			tpfjet_twr_frac_.push_back(hitsAndFracs[iHit].second);
			tpfjet_rechits[(*ith).id()].second.insert(hitsAndFracs[iHit].second);
			tpfjet_twr_hadind_.push_back(tpfjet_had_n_ - 1);
			tpfjet_twr_elmttype_.push_back(0);
			tpfjet_twr_clusterind_.push_back(cluster_ind);
			if(hasTrack){
			  tpfjet_twr_candtrackind_.push_back(tpfjet_ncandtracks_ - 1);
			}
			else{
			  tpfjet_twr_candtrackind_.push_back(-1);
			}
			switch((*ith).id().subdet()){
			case HcalSubdetector::HcalBarrel:
			  {
			    const CaloCellGeometry *thisCell = HBGeom->getGeometry((*ith).id().rawId());
			    const CaloCellGeometry::CornersVec& cv = thisCell->getCorners();
			    float avgeta = (cv[0].eta() + cv[2].eta())/2.0;
			    float avgphi = (static_cast<double>(cv[0].phi()) + static_cast<double>(cv[2].phi()))/2.0;
			    if(cv[0].phi() < cv[2].phi()) avgphi = (2.0*3.141592653 + static_cast<double>(cv[0].phi()) + static_cast<double>(cv[2].phi()))/2.0;
			    tpfjet_twr_dR_.push_back(deltaR(tpfjet_eta_,tpfjet_phi_,avgeta,avgphi));
			    break;
			  }
			case HcalSubdetector::HcalEndcap:
			  {
			    const CaloCellGeometry *thisCell = HEGeom->getGeometry((*ith).id().rawId());
			    const CaloCellGeometry::CornersVec& cv = thisCell->getCorners();
			    float avgeta = (cv[0].eta() + cv[2].eta())/2.0;
			    float avgphi = (static_cast<double>(cv[0].phi()) + static_cast<double>(cv[2].phi()))/2.0;
			    if(cv[0].phi() < cv[2].phi()) avgphi = (2.0*3.141592653 + static_cast<double>(cv[0].phi()) + static_cast<double>(cv[2].phi()))/2.0;
			    tpfjet_twr_dR_.push_back(deltaR(tpfjet_eta_,tpfjet_phi_,avgeta,avgphi));
			    break;
			  }
			default:
			  tpfjet_twr_dR_.push_back(-1);
			  break;
			}
			tpfjet_rechits[(*ith).id()].first = tpfjet_ntwrs_;
			++tpfjet_ntwrs_;
		      }
		      else if(tpfjet_rechits[(*ith).id()].second.count(hitsAndFracs[iHit].second) == 0){
			tpfjet_twr_frac_.at(tpfjet_rechits[(*ith).id()].first) += hitsAndFracs[iHit].second;
			if(cluster_dR < tpfjet_cluster_dR_.at(tpfjet_twr_clusterind_.at(tpfjet_rechits[(*ith).id()].first))){
			  tpfjet_twr_clusterind_.at(tpfjet_rechits[(*ith).id()].first) = cluster_ind;
			}
			tpfjet_rechits[(*ith).id()].second.insert(hitsAndFracs[iHit].second);
		      }
		    } // Test if ieta,iphi matches
		  } // Loop over rechits
		} // Loop over hits
	      } // Test if element is from HCAL
	      else if(elements[iEle].type() == reco::PFBlockElement::HFHAD){ // Element is HF
		types |= 0x2;
		ntypes++;
		HFHAD_n_++;
		HF_type_ |= 0x2;
		
		h_etaHFHAD_->Fill((*it)->eta());
		
		for(edm::SortedCollection<HFRecHit,edm::StrictWeakOrdering<HFRecHit>>::const_iterator ith=hfreco->begin(); ith!=hfreco->end(); ++ith){
		  if((*ith).id().depth() == 1) continue; // Remove long fibers
		  const CaloCellGeometry *thisCell = HFGeom->getGeometry((*ith).id().rawId());
		  const CaloCellGeometry::CornersVec& cv = thisCell->getCorners();

		  bool passMatch = false;
		  if((*it)->eta() < cv[0].eta() && (*it)->eta() > cv[2].eta()){
		    if((*it)->phi() < cv[0].phi() && (*it)->phi() > cv[2].phi()) passMatch = true;
		    else if(cv[0].phi() < cv[2].phi()){
		      //std::cout << "HFHAD tag" << std::endl;
		      if((*it)->phi() < cv[0].phi()) passMatch = true;
		      else if((*it)->phi() > cv[2].phi()) passMatch = true;
		    }
		  }

		  if(passMatch){
		    tpfjet_had_ntwrs_.at(tpfjet_had_n_ - 1)++;
		    tpfjet_twr_ieta_.push_back((*ith).id().ieta());
		    tpfjet_twr_iphi_.push_back((*ith).id().iphi());
		    tpfjet_twr_depth_.push_back((*ith).id().depth());
		    tpfjet_twr_subdet_.push_back((*ith).id().subdet());
		    tpfjet_twr_hade_.push_back((*ith).energy());
		    tpfjet_twr_frac_.push_back(1.0);
		    tpfjet_twr_hadind_.push_back(tpfjet_had_n_ - 1);
		    tpfjet_twr_elmttype_.push_back(1);
		    tpfjet_twr_clusterind_.push_back(-1);
		    tpfjet_twr_candtrackind_.push_back(-1);
		    float avgeta = (cv[0].eta() + cv[2].eta())/2.0;
		    float avgphi = (static_cast<double>(cv[0].phi()) + static_cast<double>(cv[2].phi()))/2.0;
		    if(cv[0].phi() < cv[2].phi()) avgphi = (2.0*3.141592653 + static_cast<double>(cv[0].phi()) + static_cast<double>(cv[2].phi()))/2.0;
		    tpfjet_twr_dR_.push_back(deltaR(tpfjet_eta_,tpfjet_phi_,avgeta,avgphi));
		    ++tpfjet_ntwrs_;
		    HFHAD_E += (*ith).energy();
		  }
		}
	      }
	      else if(elements[iEle].type() == reco::PFBlockElement::HFEM){ // Element is HF
		types |= 0x4;
		ntypes++;
		HFEM_n_++;
		HF_type_ |= 0x4;

		h_etaHFEM_->Fill((*it)->eta());

		for(edm::SortedCollection<HFRecHit,edm::StrictWeakOrdering<HFRecHit>>::const_iterator ith=hfreco->begin(); ith!=hfreco->end(); ++ith){
		  if((*ith).id().depth() == 2) continue; // Remove short fibers
		  const CaloCellGeometry *thisCell = HFGeom->getGeometry((*ith).id().rawId());
		  const CaloCellGeometry::CornersVec& cv = thisCell->getCorners();

		  bool passMatch = false;
		  if((*it)->eta() < cv[0].eta() && (*it)->eta() > cv[2].eta()){
		    if((*it)->phi() < cv[0].phi() && (*it)->phi() > cv[2].phi()) passMatch = true;
		    else if(cv[0].phi() < cv[2].phi()){
		      //std::cout << "HFEM tag" << std::endl;
		      if((*it)->phi() < cv[0].phi()) passMatch = true;
		      else if((*it)->phi() > cv[2].phi()) passMatch = true;
		    }
		  }

		  if(passMatch){
		    tpfjet_had_ntwrs_.at(tpfjet_had_n_ - 1)++;
		    tpfjet_twr_ieta_.push_back((*ith).id().ieta());
		    tpfjet_twr_iphi_.push_back((*ith).id().iphi());
		    tpfjet_twr_depth_.push_back((*ith).id().depth());
		    tpfjet_twr_subdet_.push_back((*ith).id().subdet());
		    tpfjet_twr_hade_.push_back((*ith).energy());
		    tpfjet_twr_frac_.push_back(1.0);
		    tpfjet_twr_hadind_.push_back(tpfjet_had_n_ - 1);
		    tpfjet_twr_elmttype_.push_back(2);
		    tpfjet_twr_clusterind_.push_back(-1);
		    tpfjet_twr_candtrackind_.push_back(-1);
		    float avgeta = (cv[0].eta() + cv[2].eta())/2.0;
		    float avgphi = (static_cast<double>(cv[0].phi()) + static_cast<double>(cv[2].phi()))/2.0;
		    if(cv[0].phi() < cv[2].phi()) avgphi = (2.0*3.141592653 + static_cast<double>(cv[0].phi()) + static_cast<double>(cv[2].phi()))/2.0;
		    tpfjet_twr_dR_.push_back(deltaR(tpfjet_eta_,tpfjet_phi_,avgeta,avgphi));
		    ++tpfjet_ntwrs_;
		    HFEM_E += (*ith).energy();
		  }
		}
	      }
	      else if(elements[iEle].type() == reco::PFBlockElement::HO){ // Element is HO
		types |= 0x8;
		ntypes++;
		HF_type_ |= 0x8;
		reco::PFClusterRef clusterref = elements[iEle].clusterRef();
		reco::PFCluster cluster = *clusterref;
		double cluster_dR = deltaR(tpfjet_eta_,tpfjet_phi_,cluster.eta(),cluster.phi());
		if(tpfjet_clusters.count(cluster_dR) == 0){
		  tpfjet_clusters[cluster_dR] = tpfjet_cluster_n_;
		  tpfjet_cluster_eta_.push_back(cluster.eta());
		  tpfjet_cluster_phi_.push_back(cluster.phi());
		  tpfjet_cluster_dR_.push_back(cluster_dR);
		  tpfjet_cluster_n_++;
		}
		int cluster_ind = tpfjet_clusters[cluster_dR];

		std::vector<std::pair<DetId,float>> hitsAndFracs = cluster.hitsAndFractions();
		int nHits = hitsAndFracs.size();
		for(int iHit=0; iHit<nHits; iHit++){
		  int etaPhiPF = getEtaPhi(hitsAndFracs[iHit].first);

		  int tmpzside = ((hitsAndFracs[iHit].first.rawId() >> 13) & 0x1) ? 1 : -1;
		  int tmpieta = ((hitsAndFracs[iHit].first.rawId() >> 7) & 0x3F);
		  h_ietaHO_->Fill(tmpzside*tmpieta);

		  for(edm::SortedCollection<HORecHit,edm::StrictWeakOrdering<HORecHit>>::const_iterator ith=horeco->begin(); ith!=horeco->end(); ++ith){
		    int etaPhiRecHit = getEtaPhi((*ith).id());
		    if(etaPhiPF == etaPhiRecHit){
		      tpfjet_had_ntwrs_.at(tpfjet_had_n_ - 1)++;
		      if(tpfjet_rechits.count((*ith).id()) == 0){
			tpfjet_twr_ieta_.push_back((*ith).id().ieta());
			tpfjet_twr_iphi_.push_back((*ith).id().iphi());
			tpfjet_twr_depth_.push_back((*ith).id().depth());
			tpfjet_twr_subdet_.push_back((*ith).id().subdet());
			if(hitsAndFracs[iHit].second > 0.05 && (*ith).energy() > 0.0) twrietas[(*ith).id().ieta()]++;
			tpfjet_twr_hade_.push_back((*ith).energy());
			tpfjet_twr_frac_.push_back(hitsAndFracs[iHit].second);
			tpfjet_rechits[(*ith).id()].second.insert(hitsAndFracs[iHit].second);
			tpfjet_twr_hadind_.push_back(tpfjet_had_n_ - 1);
			tpfjet_twr_elmttype_.push_back(3);
			tpfjet_twr_clusterind_.push_back(cluster_ind);
			if(hasTrack){
			  tpfjet_twr_candtrackind_.push_back(tpfjet_ncandtracks_ - 1);
			}
			else{
			  tpfjet_twr_candtrackind_.push_back(-1);
			}
			const CaloCellGeometry *thisCell = HOGeom->getGeometry((*ith).id().rawId());
			const CaloCellGeometry::CornersVec& cv = thisCell->getCorners();
			float avgeta = (cv[0].eta() + cv[2].eta())/2.0;
			float avgphi = (static_cast<double>(cv[0].phi()) + static_cast<double>(cv[2].phi()))/2.0;
			if(cv[0].phi() < cv[2].phi()) avgphi = (2.0*3.141592653 + static_cast<double>(cv[0].phi()) + static_cast<double>(cv[2].phi()))/2.0;
			tpfjet_twr_dR_.push_back(deltaR(tpfjet_eta_,tpfjet_phi_,avgeta,avgphi));
			tpfjet_rechits[(*ith).id()].first = tpfjet_ntwrs_;
			++tpfjet_ntwrs_;
		      }
		      else if(tpfjet_rechits[(*ith).id()].second.count(hitsAndFracs[iHit].second) == 0){
			tpfjet_twr_frac_.at(tpfjet_rechits[(*ith).id()].first) += hitsAndFracs[iHit].second;
			if(cluster_dR < tpfjet_cluster_dR_.at(tpfjet_twr_clusterind_.at(tpfjet_rechits[(*ith).id()].first))){
			  tpfjet_twr_clusterind_.at(tpfjet_rechits[(*ith).id()].first) = cluster_ind;
			}
			tpfjet_rechits[(*ith).id()].second.insert(hitsAndFracs[iHit].second);
		      }
		    } // Test if ieta,iphi match
		  } // Loop over rechits
		} // Loop over hits
	      } // Test if element is HO
	    } // Test for right element index
	  } // Loop over elements
	} // Loop over elements in blocks
	h_HFHAD_n_->Fill(HFHAD_n_);
	h_HFEM_n_->Fill(HFEM_n_);
	h_twrietas_->Fill(twrietas.size());

	switch(candidateType){
	case reco::PFCandidate::h_HF:
	  h_HFHAD_type_->Fill(HF_type_);
	  tpfjet_had_emf_.push_back(HFEM_E/(HFEM_E + HFHAD_E));
	  break;
	case reco::PFCandidate::egamma_HF:
	  h_HFEM_type_->Fill(HF_type_);
	  tpfjet_had_emf_.push_back(-1);
	  break;
	default:
	  tpfjet_had_emf_.push_back(-1);
	  break;
	}
      } // Loop over PF constitutents

      int tag_had_EcalE = 0;
      int tag_had_rawHcalE = 0;
      for(int i=0; i<tpfjet_had_n_; i++){
	tag_had_EcalE += tpfjet_had_EcalE_[i];
	tag_had_rawHcalE += tpfjet_had_rawHcalE_[i];
      }
      tpfjet_emf_ = 1.0 - tag_had_rawHcalE/(tag_had_rawHcalE + tag_had_EcalE + tpfjet_unkown_E_ + tpfjet_electron_E_ + tpfjet_muon_E_ + tpfjet_photon_E_);

      if(debug_ && tpfjet_ntwrs_ == 0) std::cout << "no rechits " << iEvent.id().event() << std::endl;

      h_types_->Fill(types);
      h_ntypes_->Fill(ntypes);

      // fill probe jet variables
      ppfjet_pt_    = pf_probe.jet()->pt();
      ppfjet_p_     = pf_probe.jet()->p();
      ppfjet_E_     = pf_probe.jet()->energy();
      ppfjet_eta_   = pf_probe.jet()->eta();
      ppfjet_phi_   = pf_probe.jet()->phi();
      ppfjet_scale_ = pf_probe.scale();
      ppfjet_ntwrs_=0;
      ppfjet_ncandtracks_=0;

      if(iEvent.id().event() == debugEvent){
	std::cout << "Probe eta: " << ppfjet_eta_ << " phi: " << ppfjet_phi_ << std::endl;
      }
      //std::cout << "Probe eta: " << ppfjet_eta_ << " phi: " << ppfjet_phi_ << std::endl; //debug

      // Get PF constituents and fill HCAL towers
      std::vector<reco::PFCandidatePtr> probeconst=pf_probe.jet()->getPFConstituents();
      for(std::vector<reco::PFCandidatePtr>::const_iterator it=probeconst.begin(); it!=probeconst.end(); ++it){
	bool hasTrack = false;
	reco::PFCandidate::ParticleType candidateType = (*it)->particleId();
	switch(candidateType){
	case reco::PFCandidate::X:
	  ppfjet_unkown_E_ += (*it)->energy();
	  ppfjet_unkown_px_ += (*it)->px();
	  ppfjet_unkown_py_ += (*it)->py();
	  ppfjet_unkown_pz_ += (*it)->pz();
	  ppfjet_unkown_EcalE_ += (*it)->ecalEnergy();
	  ppfjet_unkown_n_++;
	  continue;
	case reco::PFCandidate::h:
	  {
	    ppfjet_had_E_.push_back((*it)->energy());
	    ppfjet_had_px_.push_back((*it)->px());
	    ppfjet_had_py_.push_back((*it)->py());
	    ppfjet_had_pz_.push_back((*it)->pz());
	    ppfjet_had_EcalE_.push_back((*it)->ecalEnergy());
	    ppfjet_had_rawHcalE_.push_back((*it)->rawHcalEnergy());
	    ppfjet_had_id_.push_back(0);
	    ppfjet_had_ntwrs_.push_back(0);
	    ppfjet_had_n_++;

	    if(doGenJets_){
	      float gendr = 99999;
	      float genE = 0;
	      int genpdgId = 0;
	      for(std::vector<reco::GenParticle>::const_iterator itmc = genparticles->begin(); itmc != genparticles->end(); itmc++){
		if(itmc->status() == 1 && itmc->pdgId() > 100){
		  double dr = deltaR((*it)->eta(),(*it)->phi(),itmc->eta(),itmc->phi());
		  if(dr < gendr){
		    gendr = dr;
		    genE = itmc->energy();
		    genpdgId = itmc->pdgId();
		  }
		}
	      }
	      ppfjet_had_E_mctruth_.push_back(genE);
	      ppfjet_had_mcpdgId_.push_back(genpdgId);
	    }
	    
	    reco::TrackRef trackRef = (*it)->trackRef();
	    if(trackRef.isNonnull()){
	      reco::Track track = *trackRef;
	      ppfjet_candtrack_px_.push_back(track.px());
	      ppfjet_candtrack_py_.push_back(track.py());
	      ppfjet_candtrack_pz_.push_back(track.pz());
	      ppfjet_candtrack_EcalE_.push_back((*it)->ecalEnergy());
	      ppfjet_had_candtrackind_.push_back(ppfjet_ncandtracks_);
	      hasTrack = true;
	      ppfjet_ncandtracks_++;
	    }
	    else{
	      ppfjet_had_candtrackind_.push_back(-2);
	    }
	  }
	  break;
	case reco::PFCandidate::e:
	  ppfjet_electron_E_ += (*it)->energy();
	  ppfjet_electron_px_ += (*it)->px();
	  ppfjet_electron_py_ += (*it)->py();
	  ppfjet_electron_pz_ += (*it)->pz();
	  ppfjet_electron_EcalE_ += (*it)->ecalEnergy();
	  ppfjet_electron_n_++;
	  continue;
	case reco::PFCandidate::mu:
	  ppfjet_muon_E_ += (*it)->energy();
	  ppfjet_muon_px_ += (*it)->px();
	  ppfjet_muon_py_ += (*it)->py();
	  ppfjet_muon_pz_ += (*it)->pz();
	  ppfjet_muon_EcalE_ += (*it)->ecalEnergy();
	  ppfjet_muon_n_++;
	  continue;
	case reco::PFCandidate::gamma:
	  ppfjet_photon_E_ += (*it)->energy();
	  ppfjet_photon_px_ += (*it)->px();
	  ppfjet_photon_py_ += (*it)->py();
	  ppfjet_photon_pz_ += (*it)->pz();
	  ppfjet_photon_EcalE_ += (*it)->ecalEnergy();
	  ppfjet_photon_n_++;
	  continue;
	case reco::PFCandidate::h0:
	  {
	    ppfjet_had_E_.push_back((*it)->energy());
	    ppfjet_had_px_.push_back((*it)->px());
	    ppfjet_had_py_.push_back((*it)->py());
	    ppfjet_had_pz_.push_back((*it)->pz());
	    ppfjet_had_EcalE_.push_back((*it)->ecalEnergy());
	    ppfjet_had_rawHcalE_.push_back((*it)->rawHcalEnergy());
	    ppfjet_had_id_.push_back(1);
	    ppfjet_had_candtrackind_.push_back(-1);
	    ppfjet_had_ntwrs_.push_back(0);
	    ppfjet_had_n_++;

	    if(doGenJets_){
	      float gendr = 99999;
	      float genE = 0;
	      int genpdgId = 0;
	      for(std::vector<reco::GenParticle>::const_iterator itmc = genparticles->begin(); itmc != genparticles->end(); itmc++){
		if(itmc->status() == 1 && itmc->pdgId() > 100){
		  double dr = deltaR((*it)->eta(),(*it)->phi(),itmc->eta(),itmc->phi());
		  if(dr < gendr){
		    gendr = dr;
		    genE = itmc->energy();
		    genpdgId = itmc->pdgId();
		  }
		}
	      }
	      ppfjet_had_E_mctruth_.push_back(genE);
	      ppfjet_had_mcpdgId_.push_back(genpdgId);
	    }
	    
	    break;
	  }
	case reco::PFCandidate::h_HF:
	  {
	    ppfjet_had_E_.push_back((*it)->energy());
	    ppfjet_had_px_.push_back((*it)->px());
	    ppfjet_had_py_.push_back((*it)->py());
	    ppfjet_had_pz_.push_back((*it)->pz());
	    ppfjet_had_EcalE_.push_back((*it)->ecalEnergy());
	    ppfjet_had_rawHcalE_.push_back((*it)->rawHcalEnergy());
	    ppfjet_had_id_.push_back(2);
	    ppfjet_had_candtrackind_.push_back(-1);
	    ppfjet_had_ntwrs_.push_back(0);
	    ppfjet_had_n_++;
	    
	    if(doGenJets_){
	      float gendr = 99999;
	      float genE = 0;
	      int genpdgId = 0;
	      for(std::vector<reco::GenParticle>::const_iterator itmc = genparticles->begin(); itmc != genparticles->end(); itmc++){
		if(itmc->status() == 1 && itmc->pdgId() > 100){
		  double dr = deltaR((*it)->eta(),(*it)->phi(),itmc->eta(),itmc->phi());
		  if(dr < gendr){
		    gendr = dr;
		    genE = itmc->energy();
		    genpdgId = itmc->pdgId();
		  }
		}
	      }
	      ppfjet_had_E_mctruth_.push_back(genE);
	      ppfjet_had_mcpdgId_.push_back(genpdgId);
	    }
	    
	    break;
	  }
	case reco::PFCandidate::egamma_HF:
	  {
	    ppfjet_had_E_.push_back((*it)->energy());
	    ppfjet_had_px_.push_back((*it)->px());
	    ppfjet_had_py_.push_back((*it)->py());
	    ppfjet_had_pz_.push_back((*it)->pz());
	    ppfjet_had_EcalE_.push_back((*it)->ecalEnergy());
	    ppfjet_had_rawHcalE_.push_back((*it)->rawHcalEnergy());
	    ppfjet_had_id_.push_back(3);
	    ppfjet_had_candtrackind_.push_back(-1);
	    ppfjet_had_ntwrs_.push_back(0);
	    ppfjet_had_n_++;

	    if(doGenJets_){
	      float gendr = 99999;
	      float genE = 0;
	      int genpdgId = 0;
	      for(std::vector<reco::GenParticle>::const_iterator itmc = genparticles->begin(); itmc != genparticles->end(); itmc++){
		if(itmc->status() == 1 && itmc->pdgId() > 100){
		  double dr = deltaR((*it)->eta(),(*it)->phi(),itmc->eta(),itmc->phi());
		  if(dr < gendr){
		    gendr = dr;
		    genE = itmc->energy();
		    genpdgId = itmc->pdgId();
		  }
		}
	      }
	      ppfjet_had_E_mctruth_.push_back(genE);
	      ppfjet_had_mcpdgId_.push_back(genpdgId);
	    }
	    
	    break;
	  }
	}

	float HFHAD_E = 0;
	float HFEM_E = 0;
	int HFHAD_n_ = 0;
	int HFEM_n_ = 0;
	int HF_type_ = 0;
	int maxElement=(*it)->elementsInBlocks().size();
	for(int e=0; e<maxElement; ++e){
	  // Get elements from block
	  reco::PFBlockRef blockRef = (*it)->elementsInBlocks()[e].first;
	  const edm::OwnVector<reco::PFBlockElement>& elements = blockRef->elements();
	  for(unsigned iEle=0; iEle<elements.size(); iEle++) {
	    if(elements[iEle].index() == (*it)->elementsInBlocks()[e].second){
	      if(elements[iEle].type() == reco::PFBlockElement::HCAL){ // Element is HB or HE
		HF_type_ |= 0x1;
		// Get cluster and hits
		reco::PFClusterRef clusterref = elements[iEle].clusterRef();
		reco::PFCluster cluster = *clusterref;
		double cluster_dR = deltaR(ppfjet_eta_,ppfjet_phi_,cluster.eta(),cluster.phi());
		if(ppfjet_clusters.count(cluster_dR) == 0){
		  ppfjet_clusters[cluster_dR] = ppfjet_cluster_n_;
		  ppfjet_cluster_eta_.push_back(cluster.eta());
		  ppfjet_cluster_phi_.push_back(cluster.phi());
		  ppfjet_cluster_dR_.push_back(cluster_dR);
		  ppfjet_cluster_n_++;
		}
		int cluster_ind = ppfjet_clusters[cluster_dR];
		std::vector<std::pair<DetId,float>> hitsAndFracs = cluster.hitsAndFractions();

		// Run over hits and match
		int nHits = hitsAndFracs.size();
		for(int iHit=0; iHit<nHits; iHit++){
		  int etaPhiPF = getEtaPhi(hitsAndFracs[iHit].first);

		  for(edm::SortedCollection<HBHERecHit,edm::StrictWeakOrdering<HBHERecHit>>::const_iterator ith=hbhereco->begin(); ith!=hbhereco->end(); ++ith){
		    int etaPhiRecHit = getEtaPhi((*ith).id());
		    if(etaPhiPF == etaPhiRecHit){
		      ppfjet_had_ntwrs_.at(ppfjet_had_n_ - 1)++;
		      if(ppfjet_rechits.count((*ith).id()) == 0){
			ppfjet_twr_ieta_.push_back((*ith).id().ieta());
			ppfjet_twr_iphi_.push_back((*ith).id().iphi());
			ppfjet_twr_depth_.push_back((*ith).id().depth());
			ppfjet_twr_subdet_.push_back((*ith).id().subdet());
			ppfjet_twr_hade_.push_back((*ith).energy());
			ppfjet_twr_frac_.push_back(hitsAndFracs[iHit].second);
			ppfjet_rechits[(*ith).id()].second.insert(hitsAndFracs[iHit].second);
			ppfjet_twr_hadind_.push_back(ppfjet_had_n_ - 1);
			ppfjet_twr_elmttype_.push_back(0);
			ppfjet_twr_clusterind_.push_back(cluster_ind);
			if(hasTrack){
			  ppfjet_twr_candtrackind_.push_back(ppfjet_ncandtracks_ - 1);
			}
			else{
			  ppfjet_twr_candtrackind_.push_back(-1);
			}
			switch((*ith).id().subdet()){
			case HcalSubdetector::HcalBarrel:
			  {
			    const CaloCellGeometry *thisCell = HBGeom->getGeometry((*ith).id().rawId());
			    const CaloCellGeometry::CornersVec& cv = thisCell->getCorners();
			    float avgeta = (cv[0].eta() + cv[2].eta())/2.0;
			    float avgphi = (static_cast<double>(cv[0].phi()) + static_cast<double>(cv[2].phi()))/2.0;
			    if(cv[0].phi() < cv[2].phi()) avgphi = (2.0*3.141592653 + static_cast<double>(cv[0].phi()) + static_cast<double>(cv[2].phi()))/2.0;
			    //if(pf_Event_ == 9413996) //debug
			    //printf("pHB ieta: %3d iphi: %2d eta0: %6f phi0: %6f eta2: %6f phi2: %6f dR: %f\n",(*ith).id().ieta(),(*ith).id().iphi(),static_cast<double>(cv[0].eta()),static_cast<double>(cv[0].phi()),static_cast<double>(cv[2].eta()),static_cast<double>(cv[2].phi()),static_cast<double>(deltaR(ppfjet_eta_,ppfjet_phi_,avgeta,avgphi))); //debug
			    ppfjet_twr_dR_.push_back(deltaR(ppfjet_eta_,ppfjet_phi_,avgeta,avgphi));
			    break;
			  }
			case HcalSubdetector::HcalEndcap:
			  {
			    const CaloCellGeometry *thisCell = HEGeom->getGeometry((*ith).id().rawId());
			    const CaloCellGeometry::CornersVec& cv = thisCell->getCorners();
			    float avgeta = (cv[0].eta() + cv[2].eta())/2.0;
			    float avgphi = (static_cast<double>(cv[0].phi()) + static_cast<double>(cv[2].phi()))/2.0;
			    if(cv[0].phi() < cv[2].phi()) avgphi = (2.0*3.141592653 + static_cast<double>(cv[0].phi()) + static_cast<double>(cv[2].phi()))/2.0;
			    //printf("pHE ieta: %3d iphi: %2d eta0: %6f phi0: %6f eta2: %6f phi2: %6f dR: %f\n",(*ith).id().ieta(),(*ith).id().iphi(),static_cast<double>cv[0].eta(),static_cast<double>cv[0].phi(),static_cast<double>cv[2].eta(),static_cast<double>cv[2].phi(),static_cast<double>deltaR(ppfjet_eta_,ppfjet_phi_,avgeta,avgphi)); //debug
			    /*printf("  cv0: %f cv2: %f sum: %f avg: %f\n",
				   static_cast<double>cv[0].phi(),
				   static_cast<double>cv[2].phi(),
				   (static_cast<double>cv[0].phi() + static_cast<double>cv[2].phi()),
				   static_cast<double>((cv[0].phi() + cv[2].phi())/2.0));*/
			    ppfjet_twr_dR_.push_back(deltaR(ppfjet_eta_,ppfjet_phi_,avgeta,avgphi));
			    break;
			  }
			default:
			  ppfjet_twr_dR_.push_back(-1);
			  break;
			}
			ppfjet_rechits[(*ith).id()].first = ppfjet_ntwrs_;
			++ppfjet_ntwrs_;
		      }
		      else if(ppfjet_rechits[(*ith).id()].second.count(hitsAndFracs[iHit].second) == 0){
			ppfjet_twr_frac_.at(ppfjet_rechits[(*ith).id()].first) += hitsAndFracs[iHit].second;
			if(cluster_dR < ppfjet_cluster_dR_.at(ppfjet_twr_clusterind_.at(ppfjet_rechits[(*ith).id()].first))){
			  ppfjet_twr_clusterind_.at(ppfjet_rechits[(*ith).id()].first) = cluster_ind;
			}
			ppfjet_rechits[(*ith).id()].second.insert(hitsAndFracs[iHit].second);
		      }
		    } // Test if ieta,iphi matches
		  } // Loop over rechits
		} // Loop over hits
	      } // Test if element is from HCAL
	      else if(elements[iEle].type() == reco::PFBlockElement::HFHAD){ // Element is HF
		types |= 0x2;
		ntypes++;
		HFHAD_n_++;
		HF_type_ |= 0x2;
		
		h_etaHFHAD_->Fill((*it)->eta());

		for(edm::SortedCollection<HFRecHit,edm::StrictWeakOrdering<HFRecHit>>::const_iterator ith=hfreco->begin(); ith!=hfreco->end(); ++ith){
		  if((*ith).id().depth() == 1) continue; // Remove long fibers
		  const CaloCellGeometry *thisCell = HFGeom->getGeometry((*ith).id().rawId());
		  const CaloCellGeometry::CornersVec& cv = thisCell->getCorners();
		  
		  bool passMatch = false;
		  if((*it)->eta() < cv[0].eta() && (*it)->eta() > cv[2].eta()){
		    if((*it)->phi() < cv[0].phi() && (*it)->phi() > cv[2].phi()) passMatch = true;
		    else if(cv[0].phi() < cv[2].phi()){
		      //std::cout << "HFHAD probe" << std::endl;
		      if((*it)->phi() < cv[0].phi()) passMatch = true;
		      else if((*it)->phi() > cv[2].phi()) passMatch = true;
		    }
		  }
		  
		  if(passMatch){
		    ppfjet_had_ntwrs_.at(ppfjet_had_n_ - 1)++;
		    ppfjet_twr_ieta_.push_back((*ith).id().ieta());
		    ppfjet_twr_iphi_.push_back((*ith).id().iphi());
		    ppfjet_twr_depth_.push_back((*ith).id().depth());
		    ppfjet_twr_subdet_.push_back((*ith).id().subdet());
		    ppfjet_twr_hade_.push_back((*ith).energy());
		    ppfjet_twr_frac_.push_back(1.0);
		    ppfjet_twr_hadind_.push_back(ppfjet_had_n_ - 1);
		    ppfjet_twr_elmttype_.push_back(1);
		    ppfjet_twr_clusterind_.push_back(-1);
		    ppfjet_twr_candtrackind_.push_back(-1);
		    float avgeta = (cv[0].eta() + cv[2].eta())/2.0;
		    float avgphi = (static_cast<double>(cv[0].phi()) + static_cast<double>(cv[2].phi()))/2.0;
		    if(cv[0].phi() < cv[2].phi()) avgphi = (2.0*3.141592653 + static_cast<double>(cv[0].phi()) + static_cast<double>(cv[2].phi()))/2.0;
		    ppfjet_twr_dR_.push_back(deltaR(ppfjet_eta_,ppfjet_phi_,avgeta,avgphi));
		    ++ppfjet_ntwrs_;
		    HFHAD_E += (*ith).energy();
		  }
		}		
	      }
	      else if(elements[iEle].type() == reco::PFBlockElement::HFEM){ // Element is HF
		types |= 0x4;
		ntypes++;
		HFEM_n_++;
		HF_type_ |= 0x4;

		h_etaHFEM_->Fill((*it)->eta());
		
		for(edm::SortedCollection<HFRecHit,edm::StrictWeakOrdering<HFRecHit>>::const_iterator ith=hfreco->begin(); ith!=hfreco->end(); ++ith){
		  if((*ith).id().depth() == 2) continue; // Remove short fibers
		  const CaloCellGeometry *thisCell = HFGeom->getGeometry((*ith).id().rawId());
		  const CaloCellGeometry::CornersVec& cv = thisCell->getCorners();
		  
		  bool passMatch = false;
		  if((*it)->eta() < cv[0].eta() && (*it)->eta() > cv[2].eta()){
		    if((*it)->phi() < cv[0].phi() && (*it)->phi() > cv[2].phi()) passMatch = true;
		    else if(cv[0].phi() < cv[2].phi()){
		      //std::cout << "HFEM probe" << std::endl;
		      if((*it)->phi() < cv[0].phi()) passMatch = true;
		      else if((*it)->phi() > cv[2].phi()) passMatch = true;
		    }
		  }
		  
		  if(passMatch){
		    ppfjet_had_ntwrs_.at(ppfjet_had_n_ - 1)++;
		    ppfjet_twr_ieta_.push_back((*ith).id().ieta());
		    ppfjet_twr_iphi_.push_back((*ith).id().iphi());
		    ppfjet_twr_depth_.push_back((*ith).id().depth());
		    ppfjet_twr_subdet_.push_back((*ith).id().subdet());
		    ppfjet_twr_hade_.push_back((*ith).energy());
		    ppfjet_twr_frac_.push_back(1.0);
		    ppfjet_twr_hadind_.push_back(ppfjet_had_n_ - 1);
		    ppfjet_twr_elmttype_.push_back(2);
		    ppfjet_twr_clusterind_.push_back(-1);
		    ppfjet_twr_candtrackind_.push_back(-1);
		    float avgeta = (cv[0].eta() + cv[2].eta())/2.0;
		    float avgphi = (static_cast<double>(cv[0].phi()) + static_cast<double>(cv[2].phi()))/2.0;
		    if(cv[0].phi() < cv[2].phi()) avgphi = (2.0*3.141592653 + static_cast<double>(cv[0].phi()) + static_cast<double>(cv[2].phi()))/2.0;
		    ppfjet_twr_dR_.push_back(deltaR(ppfjet_eta_,ppfjet_phi_,avgeta,avgphi));
		    ++ppfjet_ntwrs_;
		    HFEM_E += (*ith).energy();
		  }
		}
	      }
	      else if(elements[iEle].type() == reco::PFBlockElement::HO){ // Element is HO
		types |= 0x8;
		ntypes++;
		HF_type_ |= 0x8;
		reco::PFClusterRef clusterref = elements[iEle].clusterRef();
		reco::PFCluster cluster = *clusterref;
		double cluster_dR = deltaR(ppfjet_eta_,ppfjet_phi_,cluster.eta(),cluster.phi());
		if(ppfjet_clusters.count(cluster_dR) == 0){
		  ppfjet_clusters[cluster_dR] = ppfjet_cluster_n_;
		  ppfjet_cluster_eta_.push_back(cluster.eta());
		  ppfjet_cluster_phi_.push_back(cluster.phi());
		  ppfjet_cluster_dR_.push_back(cluster_dR);
		  ppfjet_cluster_n_++;
		}
		int cluster_ind = ppfjet_clusters[cluster_dR];

		std::vector<std::pair<DetId,float>> hitsAndFracs = cluster.hitsAndFractions();
		int nHits = hitsAndFracs.size();
		for(int iHit=0; iHit<nHits; iHit++){
		  int etaPhiPF = getEtaPhi(hitsAndFracs[iHit].first);

		  int tmpzside = ((hitsAndFracs[iHit].first.rawId() >> 13) & 0x1) ? 1 : -1;
		  int tmpieta = ((hitsAndFracs[iHit].first.rawId() >> 7) & 0x3F);
		  h_ietaHO_->Fill(tmpzside*tmpieta);

		  for(edm::SortedCollection<HORecHit,edm::StrictWeakOrdering<HORecHit>>::const_iterator ith=horeco->begin(); ith!=horeco->end(); ++ith){
		    int etaPhiRecHit = getEtaPhi((*ith).id());
		    if(etaPhiPF == etaPhiRecHit){
		      ppfjet_had_ntwrs_.at(ppfjet_had_n_ - 1)++;
		      if(ppfjet_rechits.count((*ith).id()) == 0){
			ppfjet_twr_ieta_.push_back((*ith).id().ieta());
			ppfjet_twr_iphi_.push_back((*ith).id().iphi());
			ppfjet_twr_depth_.push_back((*ith).id().depth());
			ppfjet_twr_subdet_.push_back((*ith).id().subdet());
			ppfjet_twr_hade_.push_back((*ith).energy());
			ppfjet_twr_frac_.push_back(hitsAndFracs[iHit].second);
			ppfjet_rechits[(*ith).id()].second.insert(hitsAndFracs[iHit].second);
			ppfjet_twr_hadind_.push_back(ppfjet_had_n_ - 1);
			ppfjet_twr_elmttype_.push_back(3);
			ppfjet_twr_clusterind_.push_back(cluster_ind);
			if(hasTrack){
			  ppfjet_twr_candtrackind_.push_back(ppfjet_ncandtracks_ - 1);
			}
			else{
			  ppfjet_twr_candtrackind_.push_back(-1);
			}
			const CaloCellGeometry *thisCell = HOGeom->getGeometry((*ith).id().rawId());
			const CaloCellGeometry::CornersVec& cv = thisCell->getCorners();
			float avgeta = (cv[0].eta() + cv[2].eta())/2.0;
			float avgphi = (static_cast<double>(cv[0].phi()) + static_cast<double>(cv[2].phi()))/2.0;
			if(cv[0].phi() < cv[2].phi()) avgphi = (2.0*3.141592653 + static_cast<double>(cv[0].phi()) + static_cast<double>(cv[2].phi()))/2.0;
			ppfjet_twr_dR_.push_back(deltaR(ppfjet_eta_,ppfjet_phi_,avgeta,avgphi));
			ppfjet_rechits[(*ith).id()].first = ppfjet_ntwrs_;
			++ppfjet_ntwrs_;
		      }
		      else if(ppfjet_rechits[(*ith).id()].second.count(hitsAndFracs[iHit].second) == 0){
			ppfjet_twr_frac_.at(ppfjet_rechits[(*ith).id()].first) += hitsAndFracs[iHit].second;
			if(cluster_dR < ppfjet_cluster_dR_.at(ppfjet_twr_clusterind_.at(ppfjet_rechits[(*ith).id()].first))){
			  ppfjet_twr_clusterind_.at(ppfjet_rechits[(*ith).id()].first) = cluster_ind;
			}
			ppfjet_rechits[(*ith).id()].second.insert(hitsAndFracs[iHit].second);
		      }
		    } // Test if ieta,iphi match
		  } // Loop over rechits
		} // Loop over hits
	      } // Test if element is from HO
	    } // Test for right element index
	  } // Loop over elements
	} // Loop over elements in blocks
	h_HFHAD_n_->Fill(HFHAD_n_);
	h_HFEM_n_->Fill(HFEM_n_);
	switch(candidateType){
	case reco::PFCandidate::h_HF:
	  h_HFHAD_type_->Fill(HF_type_);
	  ppfjet_had_emf_.push_back(HFEM_E/(HFEM_E + HFHAD_E));
	  break;
	case reco::PFCandidate::egamma_HF:
	  h_HFEM_type_->Fill(HF_type_);
	  ppfjet_had_emf_.push_back(-1);
	  break;
	default:
	  ppfjet_had_emf_.push_back(-1);
	  break;
	}
      } // Loop over PF constitutents

      int probe_had_EcalE = 0;
      int probe_had_rawHcalE = 0;
      for(int i=0; i<ppfjet_had_n_; i++){
	probe_had_EcalE += ppfjet_had_EcalE_[i];
	probe_had_rawHcalE += ppfjet_had_rawHcalE_[i];
      }
      ppfjet_emf_ = 1.0 - probe_had_rawHcalE/(probe_had_rawHcalE + probe_had_EcalE + ppfjet_unkown_E_ + ppfjet_electron_E_ + ppfjet_muon_E_ + ppfjet_photon_E_);
      
      if(doGenJets_){
	// fill genjet tag/probe variables
	tpfjet_gendr_ = 99999.;
	tpfjet_genpt_ = 0;
	tpfjet_genp_  = 0;
	ppfjet_gendr_ = 99999.;
	ppfjet_genpt_ = 0;
	ppfjet_genp_  = 0;
	for(std::vector<reco::GenJet>::const_iterator it=genjets->begin(); it!=genjets->end(); ++it){
	  const reco::GenJet* jet=&(*it);
	  double dr=deltaR(jet, pf_probe.jet());
	  if(dr<ppfjet_gendr_) {
	    ppfjet_gendr_ = dr;
	    ppfjet_genpt_ = jet->pt();
	    ppfjet_genp_ = jet->p();
	    ppfjet_genE_ = jet->energy();
	  }
	  dr=deltaR(jet, pf_tag.jet());
	  if(dr<tpfjet_gendr_) {
	    tpfjet_gendr_ = dr;
	    tpfjet_genpt_ = jet->pt();
	    tpfjet_genp_ = jet->p();
	    tpfjet_genE_ = jet->energy();
	  }
	}
      }
      
      // fill dijet variables
      pf_dijet_deta_=std::fabs(std::fabs(pf_tag.jet()->eta())-std::fabs(pf_probe.jet()->eta()));
      pf_dijet_dphi_=pf_tag.jet()->phi()-pf_probe.jet()->phi();
      if(pf_dijet_dphi_>3.1415) pf_dijet_dphi_ = 6.2832-pf_dijet_dphi_;
      pf_dijet_balance_ = (tpfjet_pt_-ppfjet_pt_)/(tpfjet_pt_+ppfjet_pt_);
      
      pf_tree_->Fill();
    }
  }
  
  return;
}

// ------------ method called once each job just before starting event loop  ------------
void CalcRespCorrDiJets::beginJob()
{
  // book histograms
  rootfile_ = new TFile(rootHistFilename_.c_str(), "RECREATE");

  if(doCaloJets_){
    hPassSelCalo_ = new TH1D("hPassSelectionCalo", "Selection Pass Failures CaloJets",200,-0.5,199.5);
    
    calo_tree_ = new TTree("calo_dijettree", "tree for dijet balancing using CaloJets");
    
    calo_tree_->Branch("tcalojet_pt",&tcalojet_pt_, "tcalojet_pt/F");
    calo_tree_->Branch("tcalojet_p",&tcalojet_p_, "tcalojet_p/F");
    calo_tree_->Branch("tcalojet_eta",&tcalojet_eta_, "tcalojet_eta/F");
    calo_tree_->Branch("tcalojet_phi",&tcalojet_phi_, "tcalojet_phi/F");
    calo_tree_->Branch("tcalojet_emf",&tcalojet_emf_, "tcalojet_emf/F");
    calo_tree_->Branch("tcalojet_scale",&tcalojet_scale_, "tcalojet_scale/F");
    if(doGenJets_){
      calo_tree_->Branch("tcalojet_genpt",&tcalojet_genpt_, "tcalojet_genpt/F");
      calo_tree_->Branch("tcalojet_genp",&tcalojet_genp_, "tcalojet_genp/F");
      calo_tree_->Branch("tcalojet_gendr",&tcalojet_gendr_, "tcalojet_gendr/F");
    }
    calo_tree_->Branch("tcalojet_EBE",&tcalojet_EBE_, "tcalojet_EBE/F");
    calo_tree_->Branch("tcalojet_EEE",&tcalojet_EEE_, "tcalojet_EEE/F");
    calo_tree_->Branch("tcalojet_HBE",&tcalojet_HBE_, "tcalojet_HBE/F");
    calo_tree_->Branch("tcalojet_HEE",&tcalojet_HEE_, "tcalojet_HEE/F");
    calo_tree_->Branch("tcalojet_HFE",&tcalojet_HFE_, "tcalojet_HFE/F");
    calo_tree_->Branch("tcalojet_ntwrs",&tcalojet_ntwrs_, "tcalojet_ntwrs/I");
    calo_tree_->Branch("tcalojet_twr_ieta",tcalojet_twr_ieta_, "tcalojet_twr_ieta[tcalojet_ntwrs]/I");
    calo_tree_->Branch("tcalojet_twr_eme",tcalojet_twr_eme_, "tcalojet_twr_eme[tcalojet_ntwrs]/F");
    calo_tree_->Branch("tcalojet_twr_hade",tcalojet_twr_hade_, "tcalojet_twr_hade[tcalojet_ntwrs]/F");
    calo_tree_->Branch("pcalojet_pt",&pcalojet_pt_, "pcalojet_pt/F");
    calo_tree_->Branch("pcalojet_p",&pcalojet_p_, "pcalojet_p/F");
    calo_tree_->Branch("pcalojet_eta",&pcalojet_eta_, "pcalojet_eta/F");
    calo_tree_->Branch("pcalojet_phi",&pcalojet_phi_, "pcalojet_phi/F");
    calo_tree_->Branch("pcalojet_emf",&pcalojet_emf_, "pcalojet_emf/F");
    calo_tree_->Branch("pcalojet_scale",&pcalojet_scale_, "pcalojet_scale/F");
    if(doGenJets_){
      calo_tree_->Branch("pcalojet_genpt",&pcalojet_genpt_, "pcalojet_genpt/F");
      calo_tree_->Branch("pcalojet_genp",&pcalojet_genp_, "pcalojet_genp/F");
      calo_tree_->Branch("pcalojet_gendr",&pcalojet_gendr_, "pcalojet_gendr/F");
    }
    calo_tree_->Branch("pcalojet_EBE",&pcalojet_EBE_, "pcalojet_EBE/F");
    calo_tree_->Branch("pcalojet_EEE",&pcalojet_EEE_, "pcalojet_EEE/F");
    calo_tree_->Branch("pcalojet_HBE",&pcalojet_HBE_, "pcalojet_HBE/F");
    calo_tree_->Branch("pcalojet_HEE",&pcalojet_HEE_, "pcalojet_HEE/F");
    calo_tree_->Branch("pcalojet_HFE",&pcalojet_HFE_, "pcalojet_HFE/F");
    calo_tree_->Branch("pcalojet_ntwrs",&pcalojet_ntwrs_, "pcalojet_ntwrs/I");
    calo_tree_->Branch("pcalojet_twr_ieta",pcalojet_twr_ieta_, "pcalojet_twr_ieta[pcalojet_ntwrs]/I");
    calo_tree_->Branch("pcalojet_twr_eme",pcalojet_twr_eme_, "pcalojet_twr_eme[pcalojet_ntwrs]/F");
    calo_tree_->Branch("pcalojet_twr_hade",pcalojet_twr_hade_, "pcalojet_twr_hade[pcalojet_ntwrs]/F");
    calo_tree_->Branch("calo_dijet_deta",&calo_dijet_deta_, "calo_dijet_deta/F");
    calo_tree_->Branch("calo_dijet_dphi",&calo_dijet_dphi_, "calo_dijet_dphi/F");
    calo_tree_->Branch("calo_dijet_balance",&calo_dijet_balance_, "calo_dijet_balance/F");
    calo_tree_->Branch("calo_thirdjet_px",&calo_thirdjet_px_, "calo_thirdjet_px/F");
    calo_tree_->Branch("calo_thirdjet_py",&calo_thirdjet_py_, "calo_thirdjet_py/F");
    calo_tree_->Branch("calo_Event",&calo_Event_, "calo_Event/I");
  }

  if(doPFJets_){
    h_types_ = new TH1D("h_types","h_types",16,0,16);
    h_ntypes_ = new TH1D("h_ntypes","h_ntypes",50,0,50);
    h_ietaHCAL_ = new TH1D("h_ietaHCAL","h_ietaHCAL",83,-41.5,41.5);
    h_etaHFHAD_ = new TH1D("h_etaHFHAD","h_etaHFHAD",100,-5.5,5.5);
    h_etaHFEM_ = new TH1D("h_etaHFEM","h_etaHFEM",100,-5.5,5.5);
    h_ietaHO_ = new TH1D("h_ietaHO","h_ietaHO",83,-41.5,41.5);
    h_HFHAD_n_ = new TH1D("h_HFHAD_n","h_HFHAD_n",10,0,10);
    h_HFEM_n_ = new TH1D("h_HFEM_n","h_HFEM_n",10,0,10);
    h_HFHAD_type_ = new TH1D("h_HFHAD_type","h_HFHAD_type",16,0,16);
    h_HFEM_type_ = new TH1D("h_HFEM_type","h_HFEM_type",16,0,16);
    h_HBHE_n_ = new TH1D("h_HBHE_n","h_HBHE_n",200,0,200);
    h_HF_n_ = new TH1D("h_HF_n","h_HF_n",200,0,200);
    h_HO_n_ = new TH1D("h_HO_n","h_HO_n",200,0,200);
    h_twrietas_ = new TH1D("h_twrietas","h_twrietas",20,0,20);
    h_rechitspos_ = new TH2D("h_rechitspos","h_rechitspos",83,-41.5,41.5,72,-0.5,71.5);
    h_hbherecoieta_ = new TH1D("h_hbherecoieta","h_hbherecoieta",83,-41.5,41.5);
    hPassSelPF_ = new TH1D("hPassSelectionPF", "Selection Pass Failures PFJets",200,-0.5,199.5);

    pf_tree_ = new TTree("pf_dijettree", "tree for dijet balancing using PFJets");

    pf_tree_->Branch("tpfjet_pt",&tpfjet_pt_, "tpfjet_pt/F");
    pf_tree_->Branch("tpfjet_p",&tpfjet_p_, "tpfjet_p/F");
    pf_tree_->Branch("tpfjet_E",&tpfjet_E_, "tpfjet_E/F");
    pf_tree_->Branch("tpfjet_eta",&tpfjet_eta_, "tpfjet_eta/F");
    pf_tree_->Branch("tpfjet_phi",&tpfjet_phi_, "tpfjet_phi/F");
    pf_tree_->Branch("tpfjet_emf",&tpfjet_emf_, "tpfjet_emf/F");
    pf_tree_->Branch("tpfjet_scale",&tpfjet_scale_, "tpfjet_scale/F");
    if(doGenJets_){
      pf_tree_->Branch("tpfjet_genpt",&tpfjet_genpt_, "tpfjet_genpt/F");
      pf_tree_->Branch("tpfjet_genp",&tpfjet_genp_, "tpfjet_genp/F");
      pf_tree_->Branch("tpfjet_genE",&tpfjet_genE_, "tpfjet_genE/F");
      pf_tree_->Branch("tpfjet_gendr",&tpfjet_gendr_, "tpfjet_gendr/F");
    }
    pf_tree_->Branch("tpfjet_unkown_E",&tpfjet_unkown_E_, "tpfjet_unkown_E/F");
    pf_tree_->Branch("tpfjet_electron_E",&tpfjet_electron_E_, "tpfjet_electron_E/F");
    pf_tree_->Branch("tpfjet_muon_E",&tpfjet_muon_E_, "tpfjet_muon_E/F");
    pf_tree_->Branch("tpfjet_photon_E",&tpfjet_photon_E_, "tpfjet_photon_E/F");
    pf_tree_->Branch("tpfjet_unkown_px",&tpfjet_unkown_px_, "tpfjet_unkown_px/F");
    pf_tree_->Branch("tpfjet_electron_px",&tpfjet_electron_px_, "tpfjet_electron_px/F");
    pf_tree_->Branch("tpfjet_muon_px",&tpfjet_muon_px_, "tpfjet_muon_px/F");
    pf_tree_->Branch("tpfjet_photon_px",&tpfjet_photon_px_, "tpfjet_photon_px/F");
    pf_tree_->Branch("tpfjet_unkown_py",&tpfjet_unkown_py_, "tpfjet_unkown_py/F");
    pf_tree_->Branch("tpfjet_electron_py",&tpfjet_electron_py_, "tpfjet_electron_py/F");
    pf_tree_->Branch("tpfjet_muon_py",&tpfjet_muon_py_, "tpfjet_muon_py/F");
    pf_tree_->Branch("tpfjet_photon_py",&tpfjet_photon_py_, "tpfjet_photon_py/F");
    pf_tree_->Branch("tpfjet_unkown_pz",&tpfjet_unkown_pz_, "tpfjet_unkown_pz/F");
    pf_tree_->Branch("tpfjet_electron_pz",&tpfjet_electron_pz_, "tpfjet_electron_pz/F");
    pf_tree_->Branch("tpfjet_muon_pz",&tpfjet_muon_pz_, "tpfjet_muon_pz/F");
    pf_tree_->Branch("tpfjet_photon_pz",&tpfjet_photon_pz_, "tpfjet_photon_pz/F");
    pf_tree_->Branch("tpfjet_unkown_EcalE",&tpfjet_unkown_EcalE_, "tpfjet_unkown_EcalE/F");
    pf_tree_->Branch("tpfjet_electron_EcalE",&tpfjet_electron_EcalE_, "tpfjet_electron_EcalE/F");
    pf_tree_->Branch("tpfjet_muon_EcalE",&tpfjet_muon_EcalE_, "tpfjet_muon_EcalE/F");
    pf_tree_->Branch("tpfjet_photon_EcalE",&tpfjet_photon_EcalE_, "tpfjet_photon_EcalE/F");
    pf_tree_->Branch("tpfjet_unkown_n",&tpfjet_unkown_n_, "tpfjet_unkown_n/I");
    pf_tree_->Branch("tpfjet_electron_n",&tpfjet_electron_n_, "tpfjet_electron_n/I");
    pf_tree_->Branch("tpfjet_muon_n",&tpfjet_muon_n_, "tpfjet_muon_n/I");
    pf_tree_->Branch("tpfjet_photon_n",&tpfjet_photon_n_, "tpfjet_photon_n/I");
    pf_tree_->Branch("tpfjet_had_n",&tpfjet_had_n_, "tpfjet_had_n/I");
    pf_tree_->Branch("tpfjet_had_E",&tpfjet_had_E_);
    pf_tree_->Branch("tpfjet_had_px",&tpfjet_had_px_);
    pf_tree_->Branch("tpfjet_had_py",&tpfjet_had_py_);
    pf_tree_->Branch("tpfjet_had_pz",&tpfjet_had_pz_);
    pf_tree_->Branch("tpfjet_had_EcalE",&tpfjet_had_EcalE_);
    pf_tree_->Branch("tpfjet_had_rawHcalE",&tpfjet_had_rawHcalE_);
    pf_tree_->Branch("tpfjet_had_emf",&tpfjet_had_emf_);
    pf_tree_->Branch("tpfjet_had_id",&tpfjet_had_id_);
    pf_tree_->Branch("tpfjet_had_candtrackind",&tpfjet_had_candtrackind_);
    if(doGenJets_){
      pf_tree_->Branch("tpfjet_had_E_mctruth",&tpfjet_had_E_mctruth_);
      pf_tree_->Branch("tpfjet_had_mcpdgId",&tpfjet_had_mcpdgId_);
    }
    pf_tree_->Branch("tpfjet_had_ntwrs",&tpfjet_had_ntwrs_);
    pf_tree_->Branch("tpfjet_ntwrs",&tpfjet_ntwrs_, "tpfjet_ntwrs/I");
    pf_tree_->Branch("tpfjet_twr_ieta",&tpfjet_twr_ieta_);
    pf_tree_->Branch("tpfjet_twr_iphi",&tpfjet_twr_iphi_);
    pf_tree_->Branch("tpfjet_twr_depth",&tpfjet_twr_depth_);
    pf_tree_->Branch("tpfjet_twr_subdet",&tpfjet_twr_subdet_);
    pf_tree_->Branch("tpfjet_twr_hade",&tpfjet_twr_hade_);
    pf_tree_->Branch("tpfjet_twr_frac",&tpfjet_twr_frac_);
    pf_tree_->Branch("tpfjet_twr_candtrackind",&tpfjet_twr_candtrackind_);
    pf_tree_->Branch("tpfjet_twr_hadind",&tpfjet_twr_hadind_);
    pf_tree_->Branch("tpfjet_twr_elmttype",&tpfjet_twr_elmttype_);
    pf_tree_->Branch("tpfjet_twr_dR",&tpfjet_twr_dR_);
    pf_tree_->Branch("tpfjet_twr_clusterind",&tpfjet_twr_clusterind_);
    pf_tree_->Branch("tpfjet_cluster_n",&tpfjet_cluster_n_, "tpfjet_cluster_n/I");
    pf_tree_->Branch("tpfjet_cluster_eta",&tpfjet_cluster_eta_);
    pf_tree_->Branch("tpfjet_cluster_phi",&tpfjet_cluster_phi_);
    pf_tree_->Branch("tpfjet_cluster_dR",&tpfjet_cluster_dR_);
    pf_tree_->Branch("tpfjet_ncandtracks",&tpfjet_ncandtracks_, "tpfjet_ncandtracks/I");
    pf_tree_->Branch("tpfjet_candtrack_px",&tpfjet_candtrack_px_);
    pf_tree_->Branch("tpfjet_candtrack_py",&tpfjet_candtrack_py_);
    pf_tree_->Branch("tpfjet_candtrack_pz",&tpfjet_candtrack_pz_);
    pf_tree_->Branch("tpfjet_candtrack_EcalE",&tpfjet_candtrack_EcalE_);
    pf_tree_->Branch("ppfjet_pt",&ppfjet_pt_, "ppfjet_pt/F");
    pf_tree_->Branch("ppfjet_p",&ppfjet_p_, "ppfjet_p/F");
    pf_tree_->Branch("ppfjet_E",&ppfjet_E_, "ppfjet_E/F");
    pf_tree_->Branch("ppfjet_eta",&ppfjet_eta_, "ppfjet_eta/F");
    pf_tree_->Branch("ppfjet_phi",&ppfjet_phi_, "ppfjet_phi/F");
    pf_tree_->Branch("ppfjet_emf",&ppfjet_emf_, "ppfjet_emf/F");
    pf_tree_->Branch("ppfjet_scale",&ppfjet_scale_, "ppfjet_scale/F");
    if(doGenJets_){
      pf_tree_->Branch("ppfjet_genpt",&ppfjet_genpt_, "ppfjet_genpt/F");
      pf_tree_->Branch("ppfjet_genp",&ppfjet_genp_, "ppfjet_genp/F");
      pf_tree_->Branch("ppfjet_genE",&ppfjet_genE_, "ppfjet_genE/F");
      pf_tree_->Branch("ppfjet_gendr",&ppfjet_gendr_, "ppfjet_gendr/F");
    }
    pf_tree_->Branch("ppfjet_unkown_E",&ppfjet_unkown_E_, "ppfjet_unkown_E/F");
    pf_tree_->Branch("ppfjet_electron_E",&ppfjet_electron_E_, "ppfjet_electron_E/F");
    pf_tree_->Branch("ppfjet_muon_E",&ppfjet_muon_E_, "ppfjet_muon_E/F");
    pf_tree_->Branch("ppfjet_photon_E",&ppfjet_photon_E_, "ppfjet_photon_E/F");
    pf_tree_->Branch("ppfjet_unkown_px",&ppfjet_unkown_px_, "ppfjet_unkown_px/F");
    pf_tree_->Branch("ppfjet_electron_px",&ppfjet_electron_px_, "ppfjet_electron_px/F");
    pf_tree_->Branch("ppfjet_muon_px",&ppfjet_muon_px_, "ppfjet_muon_px/F");
    pf_tree_->Branch("ppfjet_photon_px",&ppfjet_photon_px_, "ppfjet_photon_px/F");
    pf_tree_->Branch("ppfjet_unkown_py",&ppfjet_unkown_py_, "ppfjet_unkown_py/F");
    pf_tree_->Branch("ppfjet_electron_py",&ppfjet_electron_py_, "ppfjet_electron_py/F");
    pf_tree_->Branch("ppfjet_muon_py",&ppfjet_muon_py_, "ppfjet_muon_py/F");
    pf_tree_->Branch("ppfjet_photon_py",&ppfjet_photon_py_, "ppfjet_photon_py/F");
    pf_tree_->Branch("ppfjet_unkown_pz",&ppfjet_unkown_pz_, "ppfjet_unkown_pz/F");
    pf_tree_->Branch("ppfjet_electron_pz",&ppfjet_electron_pz_, "ppfjet_electron_pz/F");
    pf_tree_->Branch("ppfjet_muon_pz",&ppfjet_muon_pz_, "ppfjet_muon_pz/F");
    pf_tree_->Branch("ppfjet_photon_pz",&ppfjet_photon_pz_, "ppfjet_photon_pz/F");
    pf_tree_->Branch("ppfjet_unkown_EcalE",&ppfjet_unkown_EcalE_, "ppfjet_unkown_EcalE/F");
    pf_tree_->Branch("ppfjet_electron_EcalE",&ppfjet_electron_EcalE_, "ppfjet_electron_EcalE/F");
    pf_tree_->Branch("ppfjet_muon_EcalE",&ppfjet_muon_EcalE_, "ppfjet_muon_EcalE/F");
    pf_tree_->Branch("ppfjet_photon_EcalE",&ppfjet_photon_EcalE_, "ppfjet_photon_EcalE/F");
    pf_tree_->Branch("ppfjet_unkown_n",&ppfjet_unkown_n_, "ppfjet_unkown_n/I");
    pf_tree_->Branch("ppfjet_electron_n",&ppfjet_electron_n_, "ppfjet_electron_n/I");
    pf_tree_->Branch("ppfjet_muon_n",&ppfjet_muon_n_, "ppfjet_muon_n/I");
    pf_tree_->Branch("ppfjet_photon_n",&ppfjet_photon_n_, "ppfjet_photon_n/I");
    pf_tree_->Branch("ppfjet_had_n",&ppfjet_had_n_, "ppfjet_had_n/I");
    pf_tree_->Branch("ppfjet_had_E",&ppfjet_had_E_);
    pf_tree_->Branch("ppfjet_had_px",&ppfjet_had_px_);
    pf_tree_->Branch("ppfjet_had_py",&ppfjet_had_py_);
    pf_tree_->Branch("ppfjet_had_pz",&ppfjet_had_pz_);
    pf_tree_->Branch("ppfjet_had_EcalE",&ppfjet_had_EcalE_);
    pf_tree_->Branch("ppfjet_had_rawHcalE",&ppfjet_had_rawHcalE_);
    pf_tree_->Branch("ppfjet_had_emf",&ppfjet_had_emf_);
    pf_tree_->Branch("ppfjet_had_id",&ppfjet_had_id_);
    pf_tree_->Branch("ppfjet_had_candtrackind",&ppfjet_had_candtrackind_);
    if(doGenJets_){
      pf_tree_->Branch("ppfjet_had_E_mctruth",&ppfjet_had_E_mctruth_);
      pf_tree_->Branch("ppfjet_had_mcpdgId",&ppfjet_had_mcpdgId_);
    }
    pf_tree_->Branch("ppfjet_had_ntwrs",&ppfjet_had_ntwrs_);
    pf_tree_->Branch("ppfjet_ntwrs",&ppfjet_ntwrs_, "ppfjet_ntwrs/I");
    pf_tree_->Branch("ppfjet_twr_ieta",&ppfjet_twr_ieta_);
    pf_tree_->Branch("ppfjet_twr_iphi",&ppfjet_twr_iphi_);
    pf_tree_->Branch("ppfjet_twr_depth",&ppfjet_twr_depth_);
    pf_tree_->Branch("ppfjet_twr_subdet",&ppfjet_twr_subdet_);
    pf_tree_->Branch("ppfjet_twr_hade",&ppfjet_twr_hade_);
    pf_tree_->Branch("ppfjet_twr_frac",&ppfjet_twr_frac_);
    pf_tree_->Branch("ppfjet_twr_candtrackind",&ppfjet_twr_candtrackind_);
    pf_tree_->Branch("ppfjet_twr_hadind",&ppfjet_twr_hadind_);
    pf_tree_->Branch("ppfjet_twr_elmttype",&ppfjet_twr_elmttype_);
    pf_tree_->Branch("ppfjet_twr_dR",&ppfjet_twr_dR_);
    pf_tree_->Branch("ppfjet_twr_clusterind",&ppfjet_twr_clusterind_);
    pf_tree_->Branch("ppfjet_cluster_n",&ppfjet_cluster_n_, "ppfjet_cluster_n/I");
    pf_tree_->Branch("ppfjet_cluster_eta",&ppfjet_cluster_eta_);
    pf_tree_->Branch("ppfjet_cluster_phi",&ppfjet_cluster_phi_);
    pf_tree_->Branch("ppfjet_cluster_dR",&ppfjet_cluster_dR_);
    pf_tree_->Branch("ppfjet_ncandtracks",&ppfjet_ncandtracks_, "ppfjet_ncandtracks/I");
    pf_tree_->Branch("ppfjet_candtrack_px",&ppfjet_candtrack_px_);
    pf_tree_->Branch("ppfjet_candtrack_py",&ppfjet_candtrack_py_);
    pf_tree_->Branch("ppfjet_candtrack_pz",&ppfjet_candtrack_pz_);
    pf_tree_->Branch("ppfjet_candtrack_EcalE",&ppfjet_candtrack_EcalE_);
    pf_tree_->Branch("pf_dijet_deta",&pf_dijet_deta_, "pf_dijet_deta/F");
    pf_tree_->Branch("pf_dijet_dphi",&pf_dijet_dphi_, "pf_dijet_dphi/F");
    pf_tree_->Branch("pf_dijet_balance",&pf_dijet_balance_, "pf_dijet_balance/F");
    pf_tree_->Branch("pf_thirdjet_px",&pf_thirdjet_px_, "pf_thirdjet_px/F");
    pf_tree_->Branch("pf_thirdjet_py",&pf_thirdjet_py_, "pf_thirdjet_py/F");
    pf_tree_->Branch("pf_Run",&pf_Run_, "pf_Run/I");
    pf_tree_->Branch("pf_Lumi",&pf_Lumi_, "pf_Lumi/I");
    pf_tree_->Branch("pf_Event",&pf_Event_, "pf_Event/I");
    if(doGenJets_){    
      pf_tree_->Branch("pf_weight",&pf_weight_, "pf_weight/F");
    }
  }

  return;
}  

// ------------ method called once each job just after ending the event loop  ------------
void 
CalcRespCorrDiJets::endJob() {
  // write histograms
  rootfile_->cd();
  if(doCaloJets_){
    hPassSelCalo_->Write();
    calo_tree_->Write();
  }
  if(doPFJets_){
    h_types_->Write();
    h_ntypes_->Write();
    h_ietaHCAL_->Write();
    h_etaHFHAD_->Write();
    h_etaHFEM_->Write();
    h_ietaHO_->Write();
    h_HFHAD_n_->Write();
    h_HFEM_n_->Write();
    h_HFHAD_type_->Write();
    h_HFEM_type_->Write();
    h_HBHE_n_->Write();
    h_HF_n_->Write();
    h_HO_n_->Write();
    h_twrietas_->Write();
    h_rechitspos_->Write();
    h_hbherecoieta_->Write();
    hPassSelPF_->Write();
    pf_tree_->Write();
  }
  rootfile_->Close();

}

// helper function

double CalcRespCorrDiJets::deltaR(const reco::Jet* j1, const reco::Jet* j2)
{
  double deta = j1->eta()-j2->eta();
  double dphi = std::fabs(j1->phi()-j2->phi());
  if(dphi>3.1415927) dphi = 2*3.1415927 - dphi;
  return std::sqrt(deta*deta + dphi*dphi);
}

double CalcRespCorrDiJets::deltaR(const double eta1, const double phi1, const double eta2, const double phi2)
{
  double deta = eta1 - eta2;
  double dphi = std::fabs(phi1 - phi2);
  if(dphi>3.1415927) dphi = 2*3.1415927 - dphi;
  return std::sqrt(deta*deta + dphi*dphi);
}

/*
// DetId rawId bits xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
//                  1111222      3333345555556666666
//   1 = detector
//   2 = subdetector
//   3 = depth
//   4 = zside: 0 = negative z, 1 = positive z \
//   5 = abs(ieta)                              | ieta,iphi
//   6 = abs(iphi)                             /
*/

int CalcRespCorrDiJets::getEtaPhi(const DetId id)
{
  return id.rawId() & 0x3FFF; // Get 14 least-significant digits
}

int CalcRespCorrDiJets::getEtaPhi(const HcalDetId id)
{
  return id.rawId() & 0x3FFF; // Get 14 least-significant digits
}

//define this as a plug-in
DEFINE_FWK_MODULE(CalcRespCorrDiJets);
