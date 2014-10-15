//
#include "HcalClosureTest/Analyzers/interface/CalcRespCorrPhotonPlusJet.h"
#include "DataFormats/JetReco/interface/CaloJetCollection.h"
#include "DataFormats/JetReco/interface/PFJetCollection.h"
#include "DataFormats/JetReco/interface/GenJetCollection.h"
#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/TrackReco/interface/TrackExtra.h"
#include "JetMETCorrections/Objects/interface/JetCorrector.h"
#include "FWCore/Utilities/interface/EDMException.h"
#include "DataFormats/ParticleFlowCandidate/interface/PFCandidate.h"
#include "RecoEgamma/EgammaElectronAlgos/interface/ElectronHcalHelper.h"
#include "RecoEgamma/EgammaTools/interface/ConversionTools.h"
#include "RecoEgamma/EgammaTools/interface/ConversionFinder.h"
#include "DataFormats/EgammaCandidates/interface/GsfElectron.h"


#include "TTree.h"
#include "TFile.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TClonesArray.h"

#include <iostream>
#include <vector>
#include <set>
#include <map>
using namespace std;

CalcRespCorrPhotonPlusJet::CalcRespCorrPhotonPlusJet(const edm::ParameterSet& iConfig)
{
  // set parameters
  caloJetCollName_     = iConfig.getParameter<std::string>("caloJetCollName");
  photonCollName_      = iConfig.getParameter<std::string>("photonCollName");
  caloJetCorrName_     = iConfig.getParameter<std::string>("caloJetCorrName");
  pfJetCollName_       = iConfig.getParameter<std::string>("pfJetCollName");
  pfJetCorrName_       = iConfig.getParameter<std::string>("pfJetCorrName");
  genJetCollName_      = iConfig.getParameter<std::string>("genJetCollName");
  genParticleCollName_ = iConfig.getParameter<std::string>("genParticleCollName");
  genEventInfoName_    = iConfig.getParameter<std::string>("genEventInfoName");
  hbheRecHitName_      = iConfig.getParameter<std::string>("hbheRecHitName");
  hfRecHitName_        = iConfig.getParameter<std::string>("hfRecHitName");
  hoRecHitName_        = iConfig.getParameter<std::string>("hoRecHitName");
  rootHistFilename_    = iConfig.getParameter<std::string>("rootHistFilename");
  doPFJets_            = iConfig.getParameter<bool>("doPFJets");
  doGenJets_           = iConfig.getParameter<bool>("doGenJets");
  debug_               = iConfig.getUntrackedParameter<bool>("debug", false);
}

CalcRespCorrPhotonPlusJet::~CalcRespCorrPhotonPlusJet()
{
}
  
//
// member functions
//
  
// ------------ method called to for each event  ------------
void CalcRespCorrPhotonPlusJet::analyze(const edm::Event& iEvent, const edm::EventSetup& evSetup)
{ 

  //  cout<<"in analyze method...."<<endl;
  tagPho_pfiso_mycharged03.clear();

  edm::Handle<std::vector<reco::GenJet>> genjets;
  edm::Handle<std::vector<reco::GenParticle> > genparticles;

  edm::Handle<reco::PFCandidateCollection> pfHandle;
  iEvent.getByLabel("particleFlow", pfHandle);

  edm::Handle<reco::VertexCollection> vtxHandle;
  iEvent.getByLabel("offlinePrimaryVertices", vtxHandle);

  edm::Handle<reco::GsfElectronCollection> gsfElectronHandle;
  iEvent.getByLabel("gsfElectrons", gsfElectronHandle);

  ///  std::cout << "getting convH" << std::endl;
  edm::Handle<reco::ConversionCollection> convH;
  iEvent.getByLabel("allConversions", convH);

  /////  std::cout << "getting beamSpotHandle" << std::endl;
  edm::Handle<reco::BeamSpot> beamSpotHandle;
  iEvent.getByLabel("offlineBeamSpot", beamSpotHandle);


  if(doGenJets_){
    // Get GenJets
    iEvent.getByLabel(genJetCollName_,genjets);
    if(!genjets.isValid()) {
      throw edm::Exception(edm::errors::ProductNotFound)
	<< " could not find GenJet vector named " << genJetCollName_ << ".\n";
      return;
    }

    // Get GenParticles
    iEvent.getByLabel(genParticleCollName_,genparticles);
    if(!genparticles.isValid()) {
      throw edm::Exception(edm::errors::ProductNotFound)
	<< " could not find GenParticle vector named " << genParticleCollName_ << ".\n";
      return;
    }

    // Get weights
    edm::Handle<GenEventInfoProduct> genEventInfoProduct;
    iEvent.getByLabel(genEventInfoName_, genEventInfoProduct);
    if(!genEventInfoProduct.isValid()){
      throw edm::Exception(edm::errors::ProductNotFound)
	<< " could not find GenEventInfoProduct named " << genEventInfoName_ << " \n";
      return;
    }
    pf_weight_ = genEventInfoProduct->weight();
  }


  // Run over PFJets //
  
  if(doPFJets_){
    unsigned int debugEvent = 0;
    
    pf_Run_ = iEvent.id().run();
    pf_Lumi_ = iEvent.id().luminosityBlock();
    pf_Event_ = iEvent.id().event();
  ///////// Run Over Photons /////////

    // Get Photons //
    edm::Handle<reco::PhotonCollection> photons;
    iEvent.getByLabel(photonCollName_, photons);

    if(!photons.isValid()) {
      throw edm::Exception(edm::errors::ProductNotFound)
        << " could not find PhotonCollection named " << photonCollName_ << ".\n";
      return;
    }

    //////////////////////////////
    // Event Selection
    //////////////////////////////

    // sort photons by Et //
    std::set<PhotonPair, PhotonPairComp> photonpairset;
    for(reco::PhotonCollection::const_iterator it=photons->begin(); it!=photons->end(); ++it) {
      const reco::Photon* photon=&(*it);
      photonpairset.insert( PhotonPair(photon, photon->pt()) );
    }

    ///////////////////////////////    
    // TAG = Highest Et photon
    ///////////////////////////////

    // find highest Et photon //
    PhotonPair  photon_tag;
    PhotonPair  photon_2nd;
    int counter=0;
    for(std::set<PhotonPair, PhotonPairComp>::const_iterator it=photonpairset.begin(); it!=photonpairset.end(); ++it) {
      PhotonPair photon=(*it);
      ++counter;
      if(counter==1) photon_tag = photon;
      else if (counter==2) photon_2nd = photon;
    }
    
if(!photon_tag.photon())return;

    // fill tag photon variables
    tagPho_et_    = photon_tag.photon()->pt();
    ////////pho_2nd_pt_   = photon_2nd.photon()->pt();
    tagPho_energy_     = photon_tag.photon()->energy();
    tagPho_eta_   = photon_tag.photon()->eta();
    tagPho_phi_   = photon_tag.photon()->phi();
    tagPho_sieie_ = photon_tag.photon()->sigmaIetaIeta();
    tagPho_HoE_   = photon_tag.photon()->hadTowOverEm();
    tagPho_r9_    = photon_tag.photon()->r9();
    tagPho_pixelSeed_ = photon_tag.photon()->hasPixelSeed();
    tagPho_TrkIsoHollowDR04_ =  photon_tag.photon()->trkSumPtHollowConeDR04();
    tagPho_EcalIsoDR04_ = photon_tag.photon()->ecalRecHitSumEtConeDR04();
    tagPho_HcalIsoDR04_ = photon_tag.photon()->hcalTowerSumEtConeDR04();
    tagPho_HcalIsoDR0412_ = photon_tag.photon()->hcalTowerSumEtConeDR04() + (photon_tag.photon()->hadronicOverEm() - photon_tag.photon()->hadTowOverEm())*(photon_tag.photon()->energy()/cosh((photon_tag.photon()->eta())));
    
    tagPho_pfiso_myphoton03_  = pfEcalIso(photon_tag.photon(), pfHandle, 0.3, 0.0, 0.070, 0.015, 0.0, 0.0, 0.0, reco::PFCandidate::gamma);
    tagPho_pfiso_myneutral03_ = pfHcalIso(photon_tag.photon(), pfHandle, 0.3, 0.0, reco::PFCandidate::h0);
    tagPho_pfiso_mycharged03.push_back(pfTkIsoWithVertex(photon_tag.photon(), pfHandle, vtxHandle, 0.3, 0.02, 0.02, 0.0, 0.2, 0.1, reco::PFCandidate::h));

    tagPho_ConvSafeEleVeto_ = ((int)ConversionTools::hasMatchedPromptElectron(photon_tag.photon()->superCluster(), gsfElectronHandle, convH, beamSpotHandle->position()));



    // Get PFJets
    edm::Handle<reco::PFJetCollection> pfjets;
    iEvent.getByLabel(pfJetCollName_,pfjets);
    if(!pfjets.isValid()) {
      throw edm::Exception(edm::errors::ProductNotFound)
	<< " could not find PFJetCollection named " << pfJetCollName_ << ".\n";
      return;
    }

    // Get RecHits in HB and HE
    edm::Handle<edm::SortedCollection<HBHERecHit,edm::StrictWeakOrdering<HBHERecHit>>> hbhereco;
    iEvent.getByLabel(hbheRecHitName_,hbhereco);
    if(!hbhereco.isValid()) {
      throw edm::Exception(edm::errors::ProductNotFound)
	<< " could not find HBHERecHit named " << hbheRecHitName_ << ".\n";
      return;
    }
    
    // Get RecHits in HF
    edm::Handle<edm::SortedCollection<HFRecHit,edm::StrictWeakOrdering<HFRecHit>>> hfreco;
    iEvent.getByLabel(hfRecHitName_,hfreco);
    if(!hfreco.isValid()) {
      throw edm::Exception(edm::errors::ProductNotFound)
	<< " could not find HFRecHit named " << hfRecHitName_ << ".\n";
      return;
    }

    // Get RecHits in HO
    edm::Handle<edm::SortedCollection<HORecHit,edm::StrictWeakOrdering<HORecHit>>> horeco;
    iEvent.getByLabel(hoRecHitName_,horeco);
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
    
    // sort jets by corrected et
    std::set<PFJetCorretPair, PFJetCorretPairComp> pfjetcorretpairset;
    for(reco::PFJetCollection::const_iterator it=pfjets->begin(); it!=pfjets->end(); ++it) {
      const reco::PFJet* jet=&(*it);
      double minDr=99999;
      if(deltaR(photon_tag.photon()->eta(),photon_tag.photon()->phi(),jet->eta(), jet->phi())<minDr)
	minDr=deltaR(photon_tag.photon()->eta(),photon_tag.photon()->phi(),jet->eta(), jet->phi());
      if(minDr<0.5)continue;
      pfjetcorretpairset.insert( PFJetCorretPair(jet, correctorPF->correction(jet->p4())) );
    }

    PFJetCorretPair pfjet_probe;
    PFJetCorretPair pf_2ndjet;
    int cntr=0;
    for(std::set<PFJetCorretPair, PFJetCorretPairComp>::const_iterator it=pfjetcorretpairset.begin(); it!=pfjetcorretpairset.end(); ++it) {
      PFJetCorretPair jet=(*it);
      ++cntr;
      if(cntr==1) pfjet_probe = jet;
      else if(cntr==2) pf_2ndjet = jet;
    }
    
    // Reset particle variables
    ppfjet_unkown_E_ = ppfjet_unkown_px_ = ppfjet_unkown_py_ = ppfjet_unkown_pz_ = ppfjet_unkown_EcalE_ = 0.0;
    ppfjet_electron_E_ = ppfjet_electron_px_ = ppfjet_electron_py_ = ppfjet_electron_pz_ = ppfjet_electron_EcalE_ = 0.0;
    ppfjet_muon_E_ = ppfjet_muon_px_ = ppfjet_muon_py_ = ppfjet_muon_pz_ = ppfjet_muon_EcalE_ = 0.0;
    ppfjet_photon_E_ = ppfjet_photon_px_ = ppfjet_photon_py_ = ppfjet_photon_pz_ = ppfjet_photon_EcalE_ = 0.0;
    ppfjet_unkown_n_ = ppfjet_electron_n_ = ppfjet_muon_n_ = ppfjet_photon_n_ = 0;
    ppfjet_had_n_ = 0;
    ppfjet_cluster_n_ = 0;
    
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
    
    std::map<int,std::pair<int,std::set<float>>> ppfjet_rechits;
    std::map<float,int> ppfjet_clusters;
    
    int types = 0;
    int ntypes = 0;
    
    /////////////////////////////////////////////
    // Get PF constituents and fill HCAL towers
    /////////////////////////////////////////////
    
    
    h_types_->Fill(types);
    h_ntypes_->Fill(ntypes);
    
    // fill probe jet variables
    if(pfjet_probe.jet()){
      ppfjet_pt_    = pfjet_probe.jet()->pt();
      if(pf_2ndjet.jet())pf_2ndjet_pt_ = pf_2ndjet.jet()->pt();
      else pf_2ndjet_pt_ = -1;
      ppfjet_p_     = pfjet_probe.jet()->p();
      ppfjet_E_     = pfjet_probe.jet()->energy();
      ppfjet_eta_   = pfjet_probe.jet()->eta();
      ppfjet_phi_   = pfjet_probe.jet()->phi();
      ppfjet_NeutralHadronFrac_  = pfjet_probe.jet()->neutralHadronEnergyFraction();
      ppfjet_NeutralEMFrac_      = pfjet_probe.jet()->neutralEmEnergyFraction();
      ppfjet_nConstituents_      = pfjet_probe.jet()->nConstituents();
      ppfjet_ChargedHadronFrac_  = pfjet_probe.jet()->chargedHadronEnergyFraction();
      ppfjet_ChargedMultiplicity_= pfjet_probe.jet()->chargedMultiplicity();
      ppfjet_ChargedEMFrac_      = pfjet_probe.jet()->chargedEmEnergyFraction();
      ppfjet_scale_ = pfjet_probe.scale();
      ppfjet_ntwrs_=0;
      ppfjet_ncandtracks_=0;
      
      if(iEvent.id().event() == debugEvent){
	std::cout << "Probe eta: " << ppfjet_eta_ << " phi: " << ppfjet_phi_ << std::endl;
      }
      //std::cout << "Probe eta: " << ppfjet_eta_ << " phi: " << ppfjet_phi_ << std::endl; //debug
      
      // Get PF constituents and fill HCAL towers
      std::vector<reco::PFCandidatePtr> probeconst=pfjet_probe.jet()->getPFConstituents();
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
	      //cout<<"38%"<<endl;
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
 //cout<<"44%"<<endl;
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
 //cout<<"45%"<<endl;
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
 //cout<<"50%"<<endl;
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
			    if(cv[0].phi() < cv[2].phi()) std::cout << "pHB" << cv[0].phi() << " " << cv[2].phi() << std::endl;
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
			    if(cv[0].phi() < cv[2].phi()) std::cout << "pHE" << cv[0].phi() << " " << cv[2].phi() << std::endl;
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
		      std::cout << "HFHAD probe" << std::endl;
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
		    if(cv[0].phi() < cv[2].phi()) std::cout << "pHFhad" << cv[0].phi() << " " << cv[2].phi() << std::endl;
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
		      std::cout << "HFEM probe" << std::endl;
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
		    if(cv[0].phi() < cv[2].phi()) std::cout << "pHFem" << cv[0].phi() << " " << cv[2].phi() << std::endl;
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
			if(cv[0].phi() < cv[2].phi()) std::cout << "pHO" << cv[0].phi() << " " << cv[2].phi() << std::endl;
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
      
 
    if(doGenJets_){
      //cout<<"78%"<<endl;
      // fill genjet variables
      ppfjet_gendr_ = 99999.;
      ppfjet_genpt_ = 0;
      ppfjet_genp_  = 0;
      for(std::vector<reco::GenJet>::const_iterator it=genjets->begin(); it!=genjets->end(); ++it){
	const reco::GenJet* jet=&(*it);
	double dr=deltaR(jet, pfjet_probe.jet());
	if(dr<ppfjet_gendr_) {
	  ppfjet_gendr_ = dr;
	  ppfjet_genpt_ = jet->pt();
	  ppfjet_genp_ = jet->p();
	  ppfjet_genE_ = jet->energy();
	}
      }
    }
 }     
// fill photon+jet variables
 
 
 pf_tree_->Fill();
  }
  return;
  
}

// ------------ method called once each job just before starting event loop  ------------
void CalcRespCorrPhotonPlusJet::beginJob()
{

  ///  std::cout << "Start beginJob()" << std::endl;

  // book histograms
  rootfile_ = new TFile(rootHistFilename_.c_str(), "RECREATE");

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

    //////// Particle Flow ////////

    pf_tree_ = new TTree("pf_dijettree", "tree for dijet balancing using PFJets");

    pf_tree_->Branch("tagPho_et",    &tagPho_et_,    "tagPho_et/F");
    pf_tree_->Branch("tagPho_energy",     &tagPho_energy_,     "tagPho_energy/F");
    pf_tree_->Branch("pho_2nd_pt",   &pho_2nd_pt_,   "pho_2nd_pt/F");
    pf_tree_->Branch("tagPho_eta",   &tagPho_eta_,   "tagPho_eta/F");
    pf_tree_->Branch("tagPho_phi",   &tagPho_phi_,   "tagPho_phi/F");
    pf_tree_->Branch("tagPho_sieie", &tagPho_sieie_, "tagPho_sieie/F");
    pf_tree_->Branch("tagPho_HoE",   &tagPho_HoE_,   "tagPho_HoE/F");
    pf_tree_->Branch("tagPho_r9",    &tagPho_r9_,    "tagPho_r9/F");
    pf_tree_->Branch("tagPho_pixelSeed",    &tagPho_pixelSeed_,    "tagPho_pixelSeed/I");
    pf_tree_->Branch("tagPho_TrkIsoHollowDR04",&tagPho_TrkIsoHollowDR04_, "tagPho_TrkIsoHollowDR04/F");
    pf_tree_->Branch("tagPho_EcalIsoDR04",&tagPho_EcalIsoDR04_, "tagPho_EcalIsoDR04/F");
    pf_tree_->Branch("tagPho_HcalIsoDR04",&tagPho_HcalIsoDR04_, "tagPho_HcalIsoDR04/F");
    pf_tree_->Branch("tagPho_HcalIsoDR0412",&tagPho_HcalIsoDR0412_, "tagPho_HcalIsoDR0412/F");
    pf_tree_->Branch("tagPho_pfiso_myphoton03",&tagPho_pfiso_myphoton03_, "tagPho_pfiso_myphoton03/F");
    pf_tree_->Branch("tagPho_pfiso_myneutral03",&tagPho_pfiso_myneutral03_, "tagPho_pfiso_myneutral03/F");
    pf_tree_->Branch("tagPho_pfiso_mycharged03","std::vector<std::vector<float> >", &tagPho_pfiso_mycharged03);
    pf_tree_->Branch("tagPho_ConvSafeEleVeto", &tagPho_ConvSafeEleVeto_, "tagPho_ConvSafeEleVeto/I");
    pf_tree_->Branch("ppfjet_pt",&ppfjet_pt_, "ppfjet_pt/F");
    pf_tree_->Branch("ppfjet_p",&ppfjet_p_, "ppfjet_p/F");
    pf_tree_->Branch("ppfjet_E",&ppfjet_E_, "ppfjet_E/F");
    pf_tree_->Branch("ppfjet_eta",&ppfjet_eta_, "ppfjet_eta/F");
    pf_tree_->Branch("ppfjet_phi",&ppfjet_phi_, "ppfjet_phi/F");
    pf_tree_->Branch("ppfjet_scale",&ppfjet_scale_, "ppfjet_scale/F");
    pf_tree_->Branch("ppfjet_NeutralHadronFrac", &ppfjet_NeutralHadronFrac_, "ppfjet_NeutralHadronFrac/F");
    pf_tree_->Branch("ppfjet_NeutralEMFrac", &ppfjet_NeutralEMFrac_, "ppfjet_NeutralEMFrac/F");
    pf_tree_->Branch("ppfjet_nConstituents", &ppfjet_nConstituents_, "ppfjet_nConstituents/I");
    pf_tree_->Branch("ppfjet_ChargedHadronFrac", &ppfjet_ChargedHadronFrac_, "ppfjet_ChargedHadronFrac/F");
    pf_tree_->Branch("ppfjet_ChargedMultiplicity", &ppfjet_ChargedMultiplicity_, "ppfjet_ChargedMultiplicity/F");
    pf_tree_->Branch("ppfjet_ChargedEMFrac", &ppfjet_ChargedEMFrac_, "ppfjet_ChargedEMFrac/F");
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
    pf_tree_->Branch("pf_2ndjet_pt",&pf_2ndjet_pt_, "pf_2ndjet_pt/F");
    pf_tree_->Branch("pf_Run",&pf_Run_, "pf_Run/I");
    pf_tree_->Branch("pf_Lumi",&pf_Lumi_, "pf_Lumi/I");
    pf_tree_->Branch("pf_Event",&pf_Event_, "pf_Event/I");
    if(doGenJets_){    
      pf_tree_->Branch("pf_weight",&pf_weight_, "pf_weight/F");
    }
  }

  return;
  ////  std::cout << "End beginJob()" << std::endl;
}  

// ------------ method called once each job just after ending the event loop  ------------
void 
CalcRespCorrPhotonPlusJet::endJob() {
  ///  std::cout << "Start endJob()" << std::endl;
  // write histograms
  rootfile_->cd();

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
    pf_tree_->Write();
  }
  rootfile_->Close();
  ////  std::cout << "End endJob()" << std::endl;
}

// helper function

float CalcRespCorrPhotonPlusJet::pfEcalIso(const reco::Photon* localPho1, edm::Handle<reco::PFCandidateCollection> pfHandle, float dRmax, float dRVetoBarrel, float dRVetoEndcap, float etaStripBarrel, float etaStripEndcap, float energyBarrel, float energyEndcap, reco::PFCandidate::ParticleType pfToUse) {
  ////std::cout << "Inside pfEcalIso" << std::endl;
  reco::Photon* localPho = localPho1->clone();
  float dRVeto;
  float etaStrip;

  if (localPho->isEB()) {
    dRVeto = dRVetoBarrel;
    etaStrip = etaStripBarrel;
  } else {
    dRVeto = dRVetoEndcap;
    etaStrip = etaStripEndcap;
  }
  const reco::PFCandidateCollection* forIsolation = pfHandle.product();
  int nsize = forIsolation->size();
  float sum = 0;
  for (int i=0; i<nsize; i++) {
    const reco::PFCandidate& pfc = (*forIsolation)[i];
    if (pfc.particleId() ==  pfToUse) {
      // Do not include the PFCandidate associated by SC Ref to the reco::Photon                                                       
      if(pfc.superClusterRef().isNonnull() && localPho->superCluster().isNonnull()) {
        if (pfc.superClusterRef() == localPho->superCluster())
          continue;
      }

      if (localPho->isEB()) {
        if (fabs(pfc.pt()) < energyBarrel)
          continue;
      } else {
        if (fabs(pfc.energy()) < energyEndcap)
          continue;
      }
      // Shift the photon direction vector according to the PF vertex                                                                  
      math::XYZPoint pfvtx = pfc.vertex();
      math::XYZVector photon_directionWrtVtx(localPho->superCluster()->x() - pfvtx.x(),
                                             localPho->superCluster()->y() - pfvtx.y(),
                                             localPho->superCluster()->z() - pfvtx.z());

      float dEta = fabs(photon_directionWrtVtx.Eta() - pfc.momentum().Eta());
      float dR = deltaR(photon_directionWrtVtx.Eta(), photon_directionWrtVtx.Phi(), pfc.momentum().Eta(), pfc.momentum().Phi());

      if (dEta < etaStrip)
        continue;

      if(dR > dRmax || dR < dRVeto)
        continue;

      sum += pfc.pt();
    }
  }
  return sum;
}


float CalcRespCorrPhotonPlusJet::pfHcalIso(const reco::Photon* localPho,edm::Handle<reco::PFCandidateCollection> pfHandle,float dRmax, float dRveto,reco::PFCandidate::ParticleType pfToUse) {
  //// std::cout << "Inside pfHcalIso" << std::endl;
  return pfEcalIso(localPho, pfHandle, dRmax, dRveto, dRveto, 0.0, 0.0, 0.0, 0.0, pfToUse);

}

std::vector<float> CalcRespCorrPhotonPlusJet::pfTkIsoWithVertex(const reco::Photon* localPho1, edm::Handle<reco::PFCandidateCollection> pfHandle, edm::Handle<reco::VertexCollection> vtxHandle, float dRmax, float dRvetoBarrel, float dRvetoEndcap, float ptMin, float dzMax, float dxyMax, reco::PFCandidate::ParticleType pfToUse) {

  //  std::cout << "Inside pfTkIsoWithVertex()" << std::endl;
  reco::Photon* localPho = localPho1->clone();

  float dRveto;
  if (localPho->isEB())
    dRveto = dRvetoBarrel;
  else
    dRveto = dRvetoEndcap;

  std::vector<float> result;
  const reco::PFCandidateCollection* forIsolation = pfHandle.product();

  //Calculate isolation sum separately for each vertex
  //  std::cout << "vtxHandle->size() = " << vtxHandle->size() << std::endl;
  for(unsigned int ivtx=0; ivtx<(vtxHandle->size()); ++ivtx) {
    //std::cout << "Vtx " << ivtx << std::endl;
    // Shift the photon according to the vertex                                                                                        
    reco::VertexRef vtx(vtxHandle, ivtx);
    math::XYZVector photon_directionWrtVtx(localPho->superCluster()->x() - vtx->x(),
                                           localPho->superCluster()->y() - vtx->y(),
                                           localPho->superCluster()->z() - vtx->z());
    //std::cout << "pfTkIsoWithVertex :: Will Loop over the PFCandidates" << std::endl;
    float sum = 0;
    // Loop over the PFCandidates                                                                                                      
    for(unsigned i=0; i<forIsolation->size(); i++) {
      //    std::cout << "inside loop" << std::endl; 
      const reco::PFCandidate& pfc = (*forIsolation)[i];

      //require that PFCandidate is a charged hadron
      // std::cout << "pfToUse=" << pfToUse << std::endl;
      //  std::cout<< "pfc.particleId()=" << pfc.particleId() << std::endl;

      if (pfc.particleId() == pfToUse) {
	//std::cout << "\n ***** HERE pfc.particleId() == pfToUse " << std::endl;
	//std::cout << "pfc.pt()=" << pfc.pt() << std::endl;
        if (pfc.pt() < ptMin)
          continue;

        float dz = fabs(pfc.trackRef()->dz(vtx->position()));
        if (dz > dzMax) continue;

        float dxy = fabs(pfc.trackRef()->dxy(vtx->position()));
        if(fabs(dxy) > dxyMax) continue;
        float dR = deltaR(photon_directionWrtVtx.Eta(), photon_directionWrtVtx.Phi(), pfc.momentum().Eta(), pfc.momentum().Phi());
        if(dR > dRmax || dR < dRveto) continue;
        sum += pfc.pt();
	//	std::cout << "pt=" << pfc.pt() << std::endl;
      }
    }
    //    std::cout << "sum=" << sum << std::endl;
    sum = sum*1.0;
    result.push_back(sum);
  }
  //  std::cout << "Will return result" << std::endl;
  // std::cout << "result" << &result << std::endl;
  return result;
  //std::cout << "Result returned" << std::endl;
}


double CalcRespCorrPhotonPlusJet::deltaR(const reco::Jet* j1, const reco::Jet* j2)
{
  double deta = j1->eta()-j2->eta();
  double dphi = std::fabs(j1->phi()-j2->phi());
  if(dphi>3.1415927) dphi = 2*3.1415927 - dphi;
  return std::sqrt(deta*deta + dphi*dphi);
}

double CalcRespCorrPhotonPlusJet::deltaR(const double eta1, const double phi1, const double eta2, const double phi2)
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

int CalcRespCorrPhotonPlusJet::getEtaPhi(const DetId id)
{
  return id.rawId() & 0x3FFF; // Get 14 least-significant digits
}

int CalcRespCorrPhotonPlusJet::getEtaPhi(const HcalDetId id)
{
  return id.rawId() & 0x3FFF; // Get 14 least-significant digits
}

//define this as a plug-in
DEFINE_FWK_MODULE(CalcRespCorrPhotonPlusJet);
