#include "HcalClosureTest/DataFormat/interface/runPFJetCorr.h"

int main()
{
  TChain* tree = new TChain("pf_dijettree");
  TString input = "/eos/uscms/store/user/dgsheffi/QCD_Pt-15to3000_TuneD6T_Flat_8TeV_pythia6/DijetCalibration_dEta-1p5_Et-10_3rdEt-50/e02441adc4b1f61e7a01cc47fa7cba8d/tree_*.root";
  std::cout << "Opening file: " << input << std::endl;
  tree->Add(input);
  std::cout << "File opened." << std::endl;

  TString output = "/uscms_data/d3/dgsheffi/HCal/corrections/test.root";

  DijetRespCorrData data;

  float tjet_pt_, tjet_p_, tjet_E_, tjet_eta_, tjet_phi_, tjet_scale_;
  float tjet_gendr_, tjet_genpt_, tjet_genp_, tjet_genE_;
  //float tjet_EBE_, tjet_EEE_, tjet_HBE_, tjet_HEE_, tjet_HFE_;
  float tjet_unkown_E_, tjet_unkown_px_, tjet_unkown_py_, tjet_unkown_pz_, tjet_unkown_EcalE_;
  float tjet_electron_E_, tjet_electron_px_, tjet_electron_py_, tjet_electron_pz_, tjet_electron_EcalE_;
  float tjet_muon_E_, tjet_muon_px_, tjet_muon_py_, tjet_muon_pz_, tjet_muon_EcalE_;
  float tjet_photon_E_, tjet_photon_px_, tjet_photon_py_, tjet_photon_pz_, tjet_photon_EcalE_;
  int tjet_unkown_n_, tjet_electron_n_, tjet_muon_n_, tjet_photon_n_;
  int tjet_had_n_, tjet_cluster_n_;
  std::vector<float>* tjet_had_E_ = 0;
  std::vector<float>* tjet_had_px_ = 0;
  std::vector<float>* tjet_had_py_ = 0;
  std::vector<float>* tjet_had_pz_ = 0;
  std::vector<float>* tjet_had_EcalE_ = 0;
  std::vector<float>* tjet_had_rawHcalE_ = 0;
  std::vector<float>* tjet_had_emf_ = 0;
  std::vector<float>* tjet_had_E_mctruth_ = 0;
  std::vector<int>* tjet_had_id_ = 0;
  std::vector<int>* tjet_had_candtrackind_ = 0;
  std::vector<int>* tjet_had_mcpdgId_ = 0;
  std::vector<int>* tjet_had_ntwrs_ = 0;
  int tjet_ntwrs_;
  std::vector<int>* tjet_twr_ieta_ = 0;
  std::vector<int>* tjet_twr_iphi_ = 0;
  std::vector<int>* tjet_twr_candtrackind_ = 0;
  std::vector<int>* tjet_twr_hadind_ = 0;
  std::vector<int>* tjet_twr_elmttype_ = 0;
  std::vector<int>* tjet_twr_subdet_ = 0;
  std::vector<float>* tjet_twr_hade_ = 0;
  std::vector<float>* tjet_twr_frac_ = 0;
  std::vector<float>* tjet_twr_dR_ = 0;
  std::vector<int>* tjet_twr_clusterind_ = 0;
  std::vector<float>* tjet_cluster_eta_ = 0;
  std::vector<float>* tjet_cluster_phi_ = 0;
  std::vector<float>* tjet_cluster_dR_ = 0;
  int tjet_ncandtracks_;
  std::vector<float>* tjet_candtrack_px_ = 0;
  std::vector<float>* tjet_candtrack_py_ = 0;
  std::vector<float>* tjet_candtrack_pz_ = 0;
  std::vector<float>* tjet_candtrack_EcalE_ = 0;
  float pjet_pt_, pjet_p_, pjet_E_, pjet_eta_, pjet_phi_, pjet_scale_;
  float pjet_gendr_, pjet_genpt_, pjet_genp_, pjet_genE_;
  //float pjet_EBE_, pjet_EEE_, pjet_HBE_, pjet_HEE_, pjet_HFE_;
  float pjet_unkown_E_, pjet_unkown_px_, pjet_unkown_py_, pjet_unkown_pz_, pjet_unkown_EcalE_;
  float pjet_electron_E_, pjet_electron_px_, pjet_electron_py_, pjet_electron_pz_, pjet_electron_EcalE_;
  float pjet_muon_E_, pjet_muon_px_, pjet_muon_py_, pjet_muon_pz_, pjet_muon_EcalE_;
  float pjet_photon_E_, pjet_photon_px_, pjet_photon_py_, pjet_photon_pz_, pjet_photon_EcalE_;
  int pjet_unkown_n_, pjet_electron_n_, pjet_muon_n_, pjet_photon_n_;
  int pjet_had_n_, pjet_cluster_n_;
  std::vector<float>* pjet_had_E_ = 0;
  std::vector<float>* pjet_had_px_ = 0;
  std::vector<float>* pjet_had_py_ = 0;
  std::vector<float>* pjet_had_pz_ = 0;
  std::vector<float>* pjet_had_EcalE_ = 0;
  std::vector<float>* pjet_had_rawHcalE_ = 0;
  std::vector<float>* pjet_had_emf_ = 0;
  std::vector<float>* pjet_had_E_mctruth_ = 0;
  std::vector<int>* pjet_had_id_ = 0;
  std::vector<int>* pjet_had_candtrackind_ = 0;
  std::vector<int>* pjet_had_mcpdgId_ = 0;
  std::vector<int>* pjet_had_ntwrs_ = 0;
  int pjet_ntwrs_;
  std::vector<int>* pjet_twr_ieta_ = 0;
  std::vector<int>* pjet_twr_iphi_ = 0;
  std::vector<int>* pjet_twr_subdet_ = 0;
  std::vector<float>* pjet_twr_candtrackind_ = 0;
  std::vector<float>* pjet_twr_hadind_ = 0;
  std::vector<float>* pjet_twr_elmttype_ = 0;
  std::vector<float>* pjet_twr_hade_ = 0;
  std::vector<float>* pjet_twr_frac_ = 0;
  std::vector<float>* pjet_twr_dR_ = 0;
  std::vector<int>* pjet_twr_clusterind_ = 0;
  std::vector<float>* pjet_cluster_eta_ = 0;
  std::vector<float>* pjet_cluster_phi_ = 0;
  std::vector<float>* pjet_cluster_dR_ = 0;
  int pjet_ncandtracks_;
  std::vector<float>* pjet_candtrack_px_ = 0;
  std::vector<float>* pjet_candtrack_py_ = 0;
  std::vector<float>* pjet_candtrack_pz_ = 0;
  std::vector<float>* pjet_candtrack_EcalE_ = 0;
  float dijet_deta_, dijet_dphi_, dijet_balance_;
  float thirdjet_px_, thirdjet_py_;
  int pf_Run_, pf_Lumi_, pf_Event_;
  float weight_;

  tree->SetBranchAddress("tpfjet_E",&tjet_E_);
  tree->SetBranchAddress("tpfjet_pt",&tjet_pt_);
  tree->SetBranchAddress("tpfjet_p",&tjet_p_);
  tree->SetBranchAddress("tpfjet_E",&tjet_E_);
  tree->SetBranchAddress("tpfjet_eta",&tjet_eta_);
  tree->SetBranchAddress("tpfjet_phi",&tjet_phi_);
  tree->SetBranchAddress("tpfjet_scale",&tjet_scale_);
  tree->SetBranchAddress("tpfjet_genpt",&tjet_genpt_);
  tree->SetBranchAddress("tpfjet_genp",&tjet_genp_);
  tree->SetBranchAddress("tpfjet_genE",&tjet_genE_);
  tree->SetBranchAddress("tpfjet_gendr",&tjet_gendr_);
  tree->SetBranchAddress("tpfjet_unkown_E",&tjet_unkown_E_);
  tree->SetBranchAddress("tpfjet_electron_E",&tjet_electron_E_);
  tree->SetBranchAddress("tpfjet_muon_E",&tjet_muon_E_);
  tree->SetBranchAddress("tpfjet_photon_E",&tjet_photon_E_);
  tree->SetBranchAddress("tpfjet_unkown_px",&tjet_unkown_px_);
  tree->SetBranchAddress("tpfjet_electron_px",&tjet_electron_px_);
  tree->SetBranchAddress("tpfjet_muon_px",&tjet_muon_px_);
  tree->SetBranchAddress("tpfjet_photon_px",&tjet_photon_px_);
  tree->SetBranchAddress("tpfjet_unkown_py",&tjet_unkown_py_);
  tree->SetBranchAddress("tpfjet_electron_py",&tjet_electron_py_);
  tree->SetBranchAddress("tpfjet_muon_py",&tjet_muon_py_);
  tree->SetBranchAddress("tpfjet_photon_py",&tjet_photon_py_);
  tree->SetBranchAddress("tpfjet_unkown_pz",&tjet_unkown_pz_);
  tree->SetBranchAddress("tpfjet_electron_pz",&tjet_electron_pz_);
  tree->SetBranchAddress("tpfjet_muon_pz",&tjet_muon_pz_);
  tree->SetBranchAddress("tpfjet_photon_pz",&tjet_photon_pz_);
  tree->SetBranchAddress("tpfjet_unkown_EcalE",&tjet_unkown_EcalE_);
  tree->SetBranchAddress("tpfjet_electron_EcalE",&tjet_electron_EcalE_);
  tree->SetBranchAddress("tpfjet_muon_EcalE",&tjet_muon_EcalE_);
  tree->SetBranchAddress("tpfjet_photon_EcalE",&tjet_photon_EcalE_);
  tree->SetBranchAddress("tpfjet_unkown_n",&tjet_unkown_n_);
  tree->SetBranchAddress("tpfjet_electron_n",&tjet_electron_n_);
  tree->SetBranchAddress("tpfjet_muon_n",&tjet_muon_n_);
  tree->SetBranchAddress("tpfjet_photon_n",&tjet_photon_n_);
  tree->SetBranchAddress("tpfjet_had_n",&tjet_had_n_);
  tree->SetBranchAddress("tpfjet_had_E",&tjet_had_E_);
  tree->SetBranchAddress("tpfjet_had_px",&tjet_had_px_);
  tree->SetBranchAddress("tpfjet_had_py",&tjet_had_py_);
  tree->SetBranchAddress("tpfjet_had_pz",&tjet_had_pz_);
  tree->SetBranchAddress("tpfjet_had_EcalE",&tjet_had_EcalE_);
  tree->SetBranchAddress("tpfjet_had_rawHcalE",&tjet_had_rawHcalE_);
  tree->SetBranchAddress("tpfjet_had_emf",&tjet_had_emf_);
  tree->SetBranchAddress("tpfjet_had_id",&tjet_had_id_);
  tree->SetBranchAddress("tpfjet_had_candtrackind",&tjet_had_candtrackind_);
  tree->SetBranchAddress("tpfjet_had_E_mctruth",&tjet_had_E_mctruth_);
  tree->SetBranchAddress("tpfjet_had_mcpdgId",&tjet_had_mcpdgId_);
  tree->SetBranchAddress("tpfjet_had_ntwrs",&tjet_had_ntwrs_);
  tree->SetBranchAddress("tpfjet_ntwrs",&tjet_ntwrs_);
  tree->SetBranchAddress("tpfjet_twr_ieta",&tjet_twr_ieta_);
  tree->SetBranchAddress("tpfjet_twr_iphi",&tjet_twr_iphi_);
  tree->SetBranchAddress("tpfjet_twr_hade",&tjet_twr_hade_);
  tree->SetBranchAddress("tpfjet_twr_frac",&tjet_twr_frac_);
  tree->SetBranchAddress("tpfjet_twr_candtrackind",&tjet_twr_candtrackind_);
  tree->SetBranchAddress("tpfjet_twr_hadind",&tjet_twr_hadind_);
  tree->SetBranchAddress("tpfjet_twr_elmttype",&tjet_twr_elmttype_);
  tree->SetBranchAddress("tpfjet_twr_dR",&tjet_twr_dR_);
  tree->SetBranchAddress("tpfjet_twr_clusterind",&tjet_twr_clusterind_);
  tree->SetBranchAddress("tpfjet_cluster_n",&tjet_cluster_n_);
  tree->SetBranchAddress("tpfjet_cluster_eta",&tjet_cluster_eta_);
  tree->SetBranchAddress("tpfjet_cluster_phi",&tjet_cluster_phi_);
  tree->SetBranchAddress("tpfjet_cluster_dR",&tjet_cluster_dR_);
  tree->SetBranchAddress("tpfjet_twr_subdet",&tjet_twr_subdet_);
  tree->SetBranchAddress("tpfjet_ncandtracks",&tjet_ncandtracks_);
  tree->SetBranchAddress("tpfjet_candtrack_px",&tjet_candtrack_px_);
  tree->SetBranchAddress("tpfjet_candtrack_py",&tjet_candtrack_py_);
  tree->SetBranchAddress("tpfjet_candtrack_pz",&tjet_candtrack_pz_);
  tree->SetBranchAddress("tpfjet_candtrack_EcalE",&tjet_candtrack_EcalE_);
  tree->SetBranchAddress("ppfjet_pt",&pjet_pt_);
  tree->SetBranchAddress("ppfjet_p",&pjet_p_);
  tree->SetBranchAddress("ppfjet_E",&pjet_E_);
  tree->SetBranchAddress("ppfjet_eta",&pjet_eta_);
  tree->SetBranchAddress("ppfjet_phi",&pjet_phi_);
  tree->SetBranchAddress("ppfjet_scale",&pjet_scale_);
  tree->SetBranchAddress("ppfjet_genpt",&pjet_genpt_);
  tree->SetBranchAddress("ppfjet_genp",&pjet_genp_);
  tree->SetBranchAddress("ppfjet_genE",&pjet_genE_);
  tree->SetBranchAddress("ppfjet_gendr",&pjet_gendr_);
  tree->SetBranchAddress("ppfjet_unkown_E",&pjet_unkown_E_);
  tree->SetBranchAddress("ppfjet_electron_E",&pjet_electron_E_);
  tree->SetBranchAddress("ppfjet_muon_E",&pjet_muon_E_);
  tree->SetBranchAddress("ppfjet_photon_E",&pjet_photon_E_);
  tree->SetBranchAddress("ppfjet_unkown_px",&pjet_unkown_px_);
  tree->SetBranchAddress("ppfjet_electron_px",&pjet_electron_px_);
  tree->SetBranchAddress("ppfjet_muon_px",&pjet_muon_px_);
  tree->SetBranchAddress("ppfjet_photon_px",&pjet_photon_px_);
  tree->SetBranchAddress("ppfjet_unkown_py",&pjet_unkown_py_);
  tree->SetBranchAddress("ppfjet_electron_py",&pjet_electron_py_);
  tree->SetBranchAddress("ppfjet_muon_py",&pjet_muon_py_);
  tree->SetBranchAddress("ppfjet_photon_py",&pjet_photon_py_);
  tree->SetBranchAddress("ppfjet_unkown_pz",&pjet_unkown_pz_);
  tree->SetBranchAddress("ppfjet_electron_pz",&pjet_electron_pz_);
  tree->SetBranchAddress("ppfjet_muon_pz",&pjet_muon_pz_);
  tree->SetBranchAddress("ppfjet_photon_pz",&pjet_photon_pz_);
  tree->SetBranchAddress("ppfjet_unkown_EcalE",&pjet_unkown_EcalE_);
  tree->SetBranchAddress("ppfjet_electron_EcalE",&pjet_electron_EcalE_);
  tree->SetBranchAddress("ppfjet_muon_EcalE",&pjet_muon_EcalE_);
  tree->SetBranchAddress("ppfjet_photon_EcalE",&pjet_photon_EcalE_);
  tree->SetBranchAddress("ppfjet_unkown_n",&pjet_unkown_n_);
  tree->SetBranchAddress("ppfjet_electron_n",&pjet_electron_n_);
  tree->SetBranchAddress("ppfjet_muon_n",&pjet_muon_n_);
  tree->SetBranchAddress("ppfjet_photon_n",&pjet_photon_n_);
  tree->SetBranchAddress("ppfjet_had_n",&pjet_had_n_);
  tree->SetBranchAddress("ppfjet_had_E",&pjet_had_E_);
  tree->SetBranchAddress("ppfjet_had_px",&pjet_had_px_);
  tree->SetBranchAddress("ppfjet_had_py",&pjet_had_py_);
  tree->SetBranchAddress("ppfjet_had_pz",&pjet_had_pz_);
  tree->SetBranchAddress("ppfjet_had_EcalE",&pjet_had_EcalE_);
  tree->SetBranchAddress("ppfjet_had_rawHcalE",&pjet_had_rawHcalE_);
  tree->SetBranchAddress("ppfjet_had_emf",&pjet_had_emf_);
  tree->SetBranchAddress("ppfjet_had_id",&pjet_had_id_);
  tree->SetBranchAddress("ppfjet_had_candtrackind",&pjet_had_candtrackind_);
  tree->SetBranchAddress("ppfjet_had_E_mctruth",&pjet_had_E_mctruth_);
  tree->SetBranchAddress("ppfjet_had_mcpdgId",&pjet_had_mcpdgId_);
  tree->SetBranchAddress("ppfjet_had_ntwrs",&pjet_had_ntwrs_);
  tree->SetBranchAddress("ppfjet_ntwrs",&pjet_ntwrs_);
  tree->SetBranchAddress("ppfjet_twr_ieta",&pjet_twr_ieta_);
  tree->SetBranchAddress("ppfjet_twr_iphi",&pjet_twr_iphi_);
  tree->SetBranchAddress("ppfjet_twr_hade",&pjet_twr_hade_);
  tree->SetBranchAddress("ppfjet_twr_frac",&pjet_twr_frac_);
  tree->SetBranchAddress("ppfjet_twr_candtrackind",&pjet_twr_candtrackind_);
  tree->SetBranchAddress("ppfjet_twr_hadind",&pjet_twr_hadind_);
  tree->SetBranchAddress("ppfjet_twr_elmttype",&pjet_twr_elmttype_);
  tree->SetBranchAddress("ppfjet_twr_dR",&pjet_twr_dR_);
  tree->SetBranchAddress("ppfjet_twr_clusterind",&pjet_twr_clusterind_);
  tree->SetBranchAddress("ppfjet_cluster_n",&pjet_cluster_n_);
  tree->SetBranchAddress("ppfjet_cluster_eta",&pjet_cluster_eta_);
  tree->SetBranchAddress("ppfjet_cluster_phi",&pjet_cluster_phi_);
  tree->SetBranchAddress("ppfjet_cluster_dR",&pjet_cluster_dR_);
  tree->SetBranchAddress("ppfjet_twr_subdet",&pjet_twr_subdet_);
  tree->SetBranchAddress("ppfjet_ncandtracks",&pjet_ncandtracks_);
  tree->SetBranchAddress("ppfjet_candtrack_px",&pjet_candtrack_px_);
  tree->SetBranchAddress("ppfjet_candtrack_py",&pjet_candtrack_py_);
  tree->SetBranchAddress("ppfjet_candtrack_pz",&pjet_candtrack_pz_);
  tree->SetBranchAddress("ppfjet_candtrack_EcalE",&pjet_candtrack_EcalE_);
  tree->SetBranchAddress("pf_dijet_deta",&dijet_deta_);
  tree->SetBranchAddress("pf_dijet_dphi",&dijet_dphi_);
  tree->SetBranchAddress("pf_dijet_balance",&dijet_balance_);
  tree->SetBranchAddress("pf_thirdjet_px",&thirdjet_px_);
  tree->SetBranchAddress("pf_thirdjet_py",&thirdjet_py_);
  tree->SetBranchAddress("pf_Run",&pf_Run_);
  tree->SetBranchAddress("pf_Lumi",&pf_Lumi_);
  tree->SetBranchAddress("pf_Event",&pf_Event_);
  tree->SetBranchAddress("pf_weight",&weight_);

  TH1D* h_PassSel_ = new TH1D("h_PassSelection", "Selection Pass Failures",256,-0.5,255.5);
  int fails = 0;

  int nEvents = tree->GetEntries();
  std::cout << "Running over " << nEvents << " events" << std::endl;
  //nEvents = 5;
  for(int iEvent=0; iEvent<nEvents; iEvent++){
    if(iEvent % 10000 == 0){
      std::cout << "Processing event " << iEvent << std::endl;
    }
    tree->GetEntry(iEvent);
    
    int passSel = 0;

    if(tjet_ntwrs_ == 0 || pjet_ntwrs_ == 0){
      fails++;
      passSel |= 0x80;
      //std::cout << "Fails: " << iEvent << " " << tjet_ntwrs_ << " " << pjet_ntwrs_ << std::endl;
      //continue;
    }
    float tjet_Et = tjet_E_/cosh(tjet_eta_);
    float pjet_Et = pjet_E_/cosh(pjet_eta_);
    float minSumJetEt_ = 40.0;//40.0;
    float minJetEt_ = 20.0;//20.0;
    float maxThirdJetEt_ = 15.0;//15.0;
    float maxDeltaEta_ = 0.5;//0.5;
    if(tjet_Et + pjet_Et < minSumJetEt_) passSel |= 0x1;
    if(tjet_Et < minJetEt_ || pjet_Et < minJetEt_) passSel |= 0x2;
    if(sqrt(thirdjet_px_*thirdjet_px_ + thirdjet_py_*thirdjet_py_) > maxThirdJetEt_) passSel |= 0x4;
    if(dijet_deta_ > maxDeltaEta_) passSel |= 0x8;
    
    h_PassSel_->Fill(passSel);
    if(passSel) continue;

    DijetRespCorrDatum datum;
    
    // Fill datum
    datum.SetWeight(weight_);

    float sumt = 0;
    datum.SetTagEta(tjet_eta_);
    datum.SetTagPhi(tjet_phi_);
    for(int i=0; i<tjet_ntwrs_; i++){
      if(tjet_twr_hade_->at(i) > 0.0 && (tjet_twr_clusterind_->at(i) < 0 || tjet_cluster_dR_->at(tjet_twr_clusterind_->at(i)) < 0.5)){
	datum.AddTagHcalE(tjet_twr_hade_->at(i)*tjet_twr_frac_->at(i),tjet_twr_ieta_->at(i));
	sumt += tjet_twr_hade_->at(i)*tjet_twr_frac_->at(i);
      }
    }
    /*datum.SetCandTrackN(tjet_ncandtracks_);
    for(int i=0; i<tjet_ncandtracks_; i++){
      datum.AddCandTrackP(sqrt(tjet_candtrack_px_->at(i)*tjet_candtrack_px_->at(i) + tjet_candtrack_py_->at(i)*tjet_candtrack_py_->at(i) + tjet_candtrack_pz_->at(i)*tjet_candtrack_pz_->at(i)));
      datum.AddCandTrackEcalE(tjet_candtrack_EcalE_->at(i));
      map<Int_t, Double_t> clusterEnergies;
      for(int j=0; j<tjet_ntwrs_; j++){
	if(tjet_twr_candtrackind_->at(j) == i){
	  if(tjet_twr_hade_->at(j) > 0.0){
	    assert(tjet_twr_ieta_->at(j)<=41 && tjet_twr_ieta_->at(j)>=-41 && tjet_twr_ieta_->at(j)!=0);
	    //clusterEnergies[tjet_twr_ieta_[j]] = tjet_twr_hade_[j];
	    clusterEnergies[tjet_twr_ieta_->at(j)] = tjet_twr_hade_->at(j)*tjet_twr_frac_->at(j);
	  }
	}
      }
      datum.AddCandTrackHcalE(clusterEnergies);
      }*/
    float tjet_had_EcalE_total = 0;
    float tjet_had_candNoRecHits_E = 0;
    for(int iHad=0; iHad<tjet_had_n_; iHad++){
      if(tjet_had_id_->at(iHad) < 2) tjet_had_EcalE_total += tjet_had_EcalE_->at(iHad);
      if(tjet_had_ntwrs_->at(iHad) == 0 && tjet_had_candtrackind_->at(iHad) > -1){
	tjet_had_candNoRecHits_E += sqrt(tjet_candtrack_px_->at(tjet_had_candtrackind_->at(iHad))*tjet_candtrack_px_->at(tjet_had_candtrackind_->at(iHad)) + tjet_candtrack_py_->at(tjet_had_candtrackind_->at(iHad))*tjet_candtrack_py_->at(tjet_had_candtrackind_->at(iHad)) + tjet_candtrack_pz_->at(tjet_had_candtrackind_->at(iHad))*tjet_candtrack_pz_->at(tjet_had_candtrackind_->at(iHad))) - tjet_had_EcalE_->at(iHad);
      }
    }
    datum.SetTagEcalE(tjet_unkown_E_ + tjet_electron_E_ + tjet_muon_E_ + tjet_photon_E_ + tjet_had_EcalE_total + tjet_had_candNoRecHits_E);

    float sump = 0;
    datum.SetProbeEta(pjet_eta_);
    datum.SetProbePhi(pjet_phi_);
    for(int i=0; i<pjet_ntwrs_; i++){
      //std::cout << pjet_twr_clusterind_->size() << " " << pjet_twr_clusterind_->at(i) << " " << pjet_cluster_n_ << std::endl;
      if(pjet_twr_hade_->at(i) > 0.0 && (pjet_twr_clusterind_->at(i) < 0 || pjet_cluster_dR_->at(pjet_twr_clusterind_->at(i)) < 0.5)){
	datum.AddProbeHcalE(pjet_twr_hade_->at(i)*pjet_twr_frac_->at(i),pjet_twr_ieta_->at(i));
	sump += pjet_twr_hade_->at(i)*pjet_twr_frac_->at(i);
      }
    }
    /*datum.SetCandTrackN(pjet_ncandtracks_);
    for(int i=0; i<pjet_ncandtracks_; i++){
      datum.AddCandTrackP(sqrt(pjet_candtrack_px_->at(i)*pjet_candtrack_px_->at(i) + pjet_candtrack_py_->at(i)*pjet_candtrack_py_->at(i) + pjet_candtrack_pz_->at(i)*pjet_candtrack_pz_->at(i)));
      datum.AddCandTrackEcalE(pjet_candtrack_EcalE_->at(i));
      map<Int_t, Double_t> clusterEnergies;
      for(int j=0; j<pjet_ntwrs_; j++){
	if(pjet_twr_candtrackind_->at(j) == i){
	  if(pjet_twr_hade_->at(j) > 0.0){
	    assert(pjet_twr_ieta_->at(j)<=41 && pjet_twr_ieta_->at(j)>=-41 && pjet_twr_ieta_->at(j)!=0);
	    //clusterEnergies[tjet_twr_ieta_->at(j)] = tjet_twr_hade_->at(j);
	    clusterEnergies[pjet_twr_ieta_->at(j)] = pjet_twr_hade_->at(j)*pjet_twr_frac_->at(j);
	  }
	}
      }
      datum.AddCandTrackHcalE(clusterEnergies);
      }*/
    float pjet_had_EcalE_total = 0;
    float pjet_had_candNoRecHits_E = 0;
    for(int iHad=0; iHad<pjet_had_n_; iHad++){
      if(pjet_had_id_->at(iHad) < 2) pjet_had_EcalE_total += pjet_had_EcalE_->at(iHad);
      if(pjet_had_ntwrs_->at(iHad) == 0 && pjet_had_candtrackind_->at(iHad) > -1){
	pjet_had_candNoRecHits_E += sqrt(pjet_candtrack_px_->at(pjet_had_candtrackind_->at(iHad))*pjet_candtrack_px_->at(pjet_had_candtrackind_->at(iHad)) + pjet_candtrack_py_->at(pjet_had_candtrackind_->at(iHad))*pjet_candtrack_py_->at(pjet_had_candtrackind_->at(iHad)) + pjet_candtrack_pz_->at(pjet_had_candtrackind_->at(iHad))*pjet_candtrack_pz_->at(pjet_had_candtrackind_->at(iHad))) - pjet_had_EcalE_->at(iHad);
      }
    }
    datum.SetProbeEcalE(pjet_unkown_E_ + pjet_electron_E_ + pjet_muon_E_ + pjet_photon_E_ + pjet_had_EcalE_total + pjet_had_candNoRecHits_E);

    datum.SetThirdJetPx(thirdjet_px_);
    datum.SetThirdJetPy(thirdjet_py_);

    if(sumt == 0 || sump == 0){
      fails++;
      continue;
    }
    

    data.push_back(datum);
  }

  std::cout << data.GetSize() << " data" << std::endl;
  
  std::cout << "Passes: " << nEvents - fails << " Fails: " << fails << std::endl;
  std::cout << "Do CandTrack? " << data.GetDoCandTrackEnergyDiff() << std::endl;
  data.SetDoCandTrackEnergyDiff(false);
  std::cout << "Do CandTrack? " << data.GetDoCandTrackEnergyDiff() << std::endl;

  //return 0;
  
  TH1D* hist = data.doFit("h_corr","Response Corrections");
  hist->GetXaxis()->SetTitle("ieta");
  hist->GetYaxis()->SetTitle("response corrections");

  TFile* fout = new TFile(output,"RECREATE");
  fout->cd();
  hist->Write();
  h_PassSel_->Write();
  fout->Close();

  std::cout << "Passes: " << nEvents - fails << " Fails: " << fails << std::endl;
  std::cout << "Events that passed cuts: " << h_PassSel_->GetBinContent(1) << std::endl;
  
  return 0;
}
