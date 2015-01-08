#include "HcalClosureTest/DataFormat/interface/testResponseCorr.h"

int main()
{
  TChain* tree = new TChain("pf_dijettree");
  TString input = "/eos/uscms/store/user/dgsheffi/QCD_Pt-15to3000_TuneD6T_Flat_8TeV_pythia6/DijetCalibration_dEta-1p5_Et-10_3rdEt-50/e02441adc4b1f61e7a01cc47fa7cba8d/tree_*.root";
  //TString input = "/uscms_data/d3/jhakala/cmssw/CMSSW_5_3_20/src/HcalClosureTest/tree_hbhe.root";
  std::cout << "Opening file:" << input << std::endl;
  tree->Add(input);

  TString output = "/uscms_data/d1/dgsheffi/HCal/corrections/validation/QCD_Pt-15to3000_TuneD6T_Flat_8TeV_pythia6_dEta-0p5_Et-20_3rdEt-15.root";

  TString corrname = "/uscms_data/d1/dgsheffi/HCal/corrections/QCD_Pt-15to3000_TuneD6R_Flat_8TeV_pythia6_dEta-0p5_Et-20_3rdEt-15.root";
  TFile* corrfile = new TFile(corrname);
  TH1D* h_corr_ = (TH1D*)corrfile->Get("h_corr");
  for(int i=1; i<84; i++){
    if(h_corr_->GetBinContent(i) < 0.5 || h_corr_->GetBinContent(i) > 2.0) h_corr_->SetBinContent(i,1.0);
  }
  
  float tpfjet_pt_, tpfjet_p_, tpfjet_E_, tpfjet_eta_, tpfjet_phi_, tpfjet_scale_;
  float tpfjet_gendr_, tpfjet_genpt_, tpfjet_genp_, tpfjet_genE_;
  //float tpfjet_EBE_, tpfjet_EEE_, tpfjet_HBE_, tpfjet_HEE_, tpfjet_HFE_;
  float tpfjet_unkown_E_, tpfjet_unkown_px_, tpfjet_unkown_py_, tpfjet_unkown_pz_, tpfjet_unkown_EcalE_;
  float tpfjet_electron_E_, tpfjet_electron_px_, tpfjet_electron_py_, tpfjet_electron_pz_, tpfjet_electron_EcalE_;
  float tpfjet_muon_E_, tpfjet_muon_px_, tpfjet_muon_py_, tpfjet_muon_pz_, tpfjet_muon_EcalE_;
  float tpfjet_photon_E_, tpfjet_photon_px_, tpfjet_photon_py_, tpfjet_photon_pz_, tpfjet_photon_EcalE_;
  int tpfjet_unkown_n_, tpfjet_electron_n_, tpfjet_muon_n_, tpfjet_photon_n_;
  int tpfjet_had_n_, tpfjet_cluster_n_;
  std::vector<float>* tpfjet_had_E_ = 0;
  std::vector<float>* tpfjet_had_px_ = 0;
  std::vector<float>* tpfjet_had_py_ = 0;
  std::vector<float>* tpfjet_had_pz_ = 0;
  std::vector<float>* tpfjet_had_EcalE_ = 0;
  std::vector<float>* tpfjet_had_rawHcalE_ = 0;
  std::vector<float>* tpfjet_had_emf_ = 0;
  std::vector<float>* tpfjet_had_E_mctruth_ = 0;
  std::vector<int>* tpfjet_had_id_ = 0;
  std::vector<int>* tpfjet_had_candtrackind_ = 0;
  std::vector<int>* tpfjet_had_mcpdgId_ = 0;
  std::vector<int>* tpfjet_had_ntwrs_ = 0;
  int tpfjet_ntwrs_;
  std::vector<int>* tpfjet_twr_ieta_ = 0;
  std::vector<int>* tpfjet_twr_iphi_ = 0;
  std::vector<int>* tpfjet_twr_candtrackind_ = 0;
  std::vector<int>* tpfjet_twr_hadind_ = 0;
  std::vector<int>* tpfjet_twr_elmttype_ = 0;
  std::vector<int>* tpfjet_twr_subdet_ = 0;
  std::vector<float>* tpfjet_twr_hade_ = 0;
  std::vector<float>* tpfjet_twr_frac_ = 0;
  std::vector<float>* tpfjet_twr_dR_ = 0;
  std::vector<int>* tpfjet_twr_clusterind_ = 0;
  std::vector<float>* tpfjet_cluster_eta_ = 0;
  std::vector<float>* tpfjet_cluster_phi_ = 0;
  std::vector<float>* tpfjet_cluster_dR_ = 0;
  int tpfjet_ncandtracks_;
  std::vector<float>* tpfjet_candtrack_px_ = 0;
  std::vector<float>* tpfjet_candtrack_py_ = 0;
  std::vector<float>* tpfjet_candtrack_pz_ = 0;
  std::vector<float>* tpfjet_candtrack_EcalE_ = 0;
  float ppfjet_pt_, ppfjet_p_, ppfjet_E_, ppfjet_eta_, ppfjet_phi_, ppfjet_scale_;
  float ppfjet_gendr_, ppfjet_genpt_, ppfjet_genp_, ppfjet_genE_;
  //float ppfjet_EBE_, ppfjet_EEE_, ppfjet_HBE_, ppfjet_HEE_, ppfjet_HFE_;
  float ppfjet_unkown_E_, ppfjet_unkown_px_, ppfjet_unkown_py_, ppfjet_unkown_pz_, ppfjet_unkown_EcalE_;
  float ppfjet_electron_E_, ppfjet_electron_px_, ppfjet_electron_py_, ppfjet_electron_pz_, ppfjet_electron_EcalE_;
  float ppfjet_muon_E_, ppfjet_muon_px_, ppfjet_muon_py_, ppfjet_muon_pz_, ppfjet_muon_EcalE_;
  float ppfjet_photon_E_, ppfjet_photon_px_, ppfjet_photon_py_, ppfjet_photon_pz_, ppfjet_photon_EcalE_;
  int ppfjet_unkown_n_, ppfjet_electron_n_, ppfjet_muon_n_, ppfjet_photon_n_;
  int ppfjet_had_n_, ppfjet_cluster_n_;
  std::vector<float>* ppfjet_had_E_ = 0;
  std::vector<float>* ppfjet_had_px_ = 0;
  std::vector<float>* ppfjet_had_py_ = 0;
  std::vector<float>* ppfjet_had_pz_ = 0;
  std::vector<float>* ppfjet_had_EcalE_ = 0;
  std::vector<float>* ppfjet_had_rawHcalE_ = 0;
  std::vector<float>* ppfjet_had_emf_ = 0;
  std::vector<float>* ppfjet_had_E_mctruth_ = 0;
  std::vector<int>* ppfjet_had_id_ = 0;
  std::vector<int>* ppfjet_had_candtrackind_ = 0;
  std::vector<int>* ppfjet_had_mcpdgId_ = 0;
  std::vector<int>* ppfjet_had_ntwrs_ = 0;
  int ppfjet_ntwrs_;
  std::vector<int>* ppfjet_twr_ieta_ = 0;
  std::vector<int>* ppfjet_twr_iphi_ = 0;
  std::vector<int>* ppfjet_twr_subdet_ = 0;
  std::vector<float>* ppfjet_twr_candtrackind_ = 0;
  std::vector<float>* ppfjet_twr_hadind_ = 0;
  std::vector<float>* ppfjet_twr_elmttype_ = 0;
  std::vector<float>* ppfjet_twr_hade_ = 0;
  std::vector<float>* ppfjet_twr_frac_ = 0;
  std::vector<float>* ppfjet_twr_dR_ = 0;
  std::vector<int>* ppfjet_twr_clusterind_ = 0;
  std::vector<float>* ppfjet_cluster_eta_ = 0;
  std::vector<float>* ppfjet_cluster_phi_ = 0;
  std::vector<float>* ppfjet_cluster_dR_ = 0;
  int ppfjet_ncandtracks_;
  std::vector<float>* ppfjet_candtrack_px_ = 0;
  std::vector<float>* ppfjet_candtrack_py_ = 0;
  std::vector<float>* ppfjet_candtrack_pz_ = 0;
  std::vector<float>* ppfjet_candtrack_EcalE_ = 0;
  float pf_dijet_deta_, pf_dijet_dphi_, pf_dijet_balance_;
  float pf_thirdjet_px_, pf_thirdjet_py_;
  int pf_Run_, pf_Lumi_, pf_Event_;

  tree->SetBranchAddress("tpfjet_E",&tpfjet_E_);
  tree->SetBranchAddress("tpfjet_pt",&tpfjet_pt_);
  tree->SetBranchAddress("tpfjet_p",&tpfjet_p_);
  tree->SetBranchAddress("tpfjet_E",&tpfjet_E_);
  tree->SetBranchAddress("tpfjet_eta",&tpfjet_eta_);
  tree->SetBranchAddress("tpfjet_phi",&tpfjet_phi_);
  tree->SetBranchAddress("tpfjet_scale",&tpfjet_scale_);
  tree->SetBranchAddress("tpfjet_genpt",&tpfjet_genpt_);
  tree->SetBranchAddress("tpfjet_genp",&tpfjet_genp_);
  tree->SetBranchAddress("tpfjet_genE",&tpfjet_genE_);
  tree->SetBranchAddress("tpfjet_gendr",&tpfjet_gendr_);
  tree->SetBranchAddress("tpfjet_unkown_E",&tpfjet_unkown_E_);
  tree->SetBranchAddress("tpfjet_electron_E",&tpfjet_electron_E_);
  tree->SetBranchAddress("tpfjet_muon_E",&tpfjet_muon_E_);
  tree->SetBranchAddress("tpfjet_photon_E",&tpfjet_photon_E_);
  tree->SetBranchAddress("tpfjet_unkown_px",&tpfjet_unkown_px_);
  tree->SetBranchAddress("tpfjet_electron_px",&tpfjet_electron_px_);
  tree->SetBranchAddress("tpfjet_muon_px",&tpfjet_muon_px_);
  tree->SetBranchAddress("tpfjet_photon_px",&tpfjet_photon_px_);
  tree->SetBranchAddress("tpfjet_unkown_py",&tpfjet_unkown_py_);
  tree->SetBranchAddress("tpfjet_electron_py",&tpfjet_electron_py_);
  tree->SetBranchAddress("tpfjet_muon_py",&tpfjet_muon_py_);
  tree->SetBranchAddress("tpfjet_photon_py",&tpfjet_photon_py_);
  tree->SetBranchAddress("tpfjet_unkown_pz",&tpfjet_unkown_pz_);
  tree->SetBranchAddress("tpfjet_electron_pz",&tpfjet_electron_pz_);
  tree->SetBranchAddress("tpfjet_muon_pz",&tpfjet_muon_pz_);
  tree->SetBranchAddress("tpfjet_photon_pz",&tpfjet_photon_pz_);
  tree->SetBranchAddress("tpfjet_unkown_EcalE",&tpfjet_unkown_EcalE_);
  tree->SetBranchAddress("tpfjet_electron_EcalE",&tpfjet_electron_EcalE_);
  tree->SetBranchAddress("tpfjet_muon_EcalE",&tpfjet_muon_EcalE_);
  tree->SetBranchAddress("tpfjet_photon_EcalE",&tpfjet_photon_EcalE_);
  tree->SetBranchAddress("tpfjet_unkown_n",&tpfjet_unkown_n_);
  tree->SetBranchAddress("tpfjet_electron_n",&tpfjet_electron_n_);
  tree->SetBranchAddress("tpfjet_muon_n",&tpfjet_muon_n_);
  tree->SetBranchAddress("tpfjet_photon_n",&tpfjet_photon_n_);
  tree->SetBranchAddress("tpfjet_had_n",&tpfjet_had_n_);
  tree->SetBranchAddress("tpfjet_had_E",&tpfjet_had_E_);
  tree->SetBranchAddress("tpfjet_had_px",&tpfjet_had_px_);
  tree->SetBranchAddress("tpfjet_had_py",&tpfjet_had_py_);
  tree->SetBranchAddress("tpfjet_had_pz",&tpfjet_had_pz_);
  tree->SetBranchAddress("tpfjet_had_EcalE",&tpfjet_had_EcalE_);
  tree->SetBranchAddress("tpfjet_had_rawHcalE",&tpfjet_had_rawHcalE_);
  tree->SetBranchAddress("tpfjet_had_emf",&tpfjet_had_emf_);
  tree->SetBranchAddress("tpfjet_had_id",&tpfjet_had_id_);
  tree->SetBranchAddress("tpfjet_had_candtrackind",&tpfjet_had_candtrackind_);
  tree->SetBranchAddress("tpfjet_had_E_mctruth",&tpfjet_had_E_mctruth_);
  tree->SetBranchAddress("tpfjet_had_mcpdgId",&tpfjet_had_mcpdgId_);
  tree->SetBranchAddress("tpfjet_had_ntwrs",&tpfjet_had_ntwrs_);
  tree->SetBranchAddress("tpfjet_ntwrs",&tpfjet_ntwrs_);
  tree->SetBranchAddress("tpfjet_twr_ieta",&tpfjet_twr_ieta_);
  tree->SetBranchAddress("tpfjet_twr_iphi",&tpfjet_twr_iphi_);
  tree->SetBranchAddress("tpfjet_twr_hade",&tpfjet_twr_hade_);
  tree->SetBranchAddress("tpfjet_twr_frac",&tpfjet_twr_frac_);
  tree->SetBranchAddress("tpfjet_twr_candtrackind",&tpfjet_twr_candtrackind_);
  tree->SetBranchAddress("tpfjet_twr_hadind",&tpfjet_twr_hadind_);
  tree->SetBranchAddress("tpfjet_twr_elmttype",&tpfjet_twr_elmttype_);
  tree->SetBranchAddress("tpfjet_twr_dR",&tpfjet_twr_dR_);
  tree->SetBranchAddress("tpfjet_twr_clusterind",&tpfjet_twr_clusterind_);
  tree->SetBranchAddress("tpfjet_cluster_n",&tpfjet_cluster_n_);
  tree->SetBranchAddress("tpfjet_cluster_eta",&tpfjet_cluster_eta_);
  tree->SetBranchAddress("tpfjet_cluster_phi",&tpfjet_cluster_phi_);
  tree->SetBranchAddress("tpfjet_cluster_dR",&tpfjet_cluster_dR_);
  tree->SetBranchAddress("tpfjet_twr_subdet",&tpfjet_twr_subdet_);
  tree->SetBranchAddress("tpfjet_ncandtracks",&tpfjet_ncandtracks_);
  tree->SetBranchAddress("tpfjet_candtrack_px",&tpfjet_candtrack_px_);
  tree->SetBranchAddress("tpfjet_candtrack_py",&tpfjet_candtrack_py_);
  tree->SetBranchAddress("tpfjet_candtrack_pz",&tpfjet_candtrack_pz_);
  tree->SetBranchAddress("tpfjet_candtrack_EcalE",&tpfjet_candtrack_EcalE_);
  tree->SetBranchAddress("ppfjet_pt",&ppfjet_pt_);
  tree->SetBranchAddress("ppfjet_p",&ppfjet_p_);
  tree->SetBranchAddress("ppfjet_E",&ppfjet_E_);
  tree->SetBranchAddress("ppfjet_eta",&ppfjet_eta_);
  tree->SetBranchAddress("ppfjet_phi",&ppfjet_phi_);
  tree->SetBranchAddress("ppfjet_scale",&ppfjet_scale_);
  tree->SetBranchAddress("ppfjet_genpt",&ppfjet_genpt_);
  tree->SetBranchAddress("ppfjet_genp",&ppfjet_genp_);
  tree->SetBranchAddress("ppfjet_genE",&ppfjet_genE_);
  tree->SetBranchAddress("ppfjet_gendr",&ppfjet_gendr_);
  tree->SetBranchAddress("ppfjet_unkown_E",&ppfjet_unkown_E_);
  tree->SetBranchAddress("ppfjet_electron_E",&ppfjet_electron_E_);
  tree->SetBranchAddress("ppfjet_muon_E",&ppfjet_muon_E_);
  tree->SetBranchAddress("ppfjet_photon_E",&ppfjet_photon_E_);
  tree->SetBranchAddress("ppfjet_unkown_px",&ppfjet_unkown_px_);
  tree->SetBranchAddress("ppfjet_electron_px",&ppfjet_electron_px_);
  tree->SetBranchAddress("ppfjet_muon_px",&ppfjet_muon_px_);
  tree->SetBranchAddress("ppfjet_photon_px",&ppfjet_photon_px_);
  tree->SetBranchAddress("ppfjet_unkown_py",&ppfjet_unkown_py_);
  tree->SetBranchAddress("ppfjet_electron_py",&ppfjet_electron_py_);
  tree->SetBranchAddress("ppfjet_muon_py",&ppfjet_muon_py_);
  tree->SetBranchAddress("ppfjet_photon_py",&ppfjet_photon_py_);
  tree->SetBranchAddress("ppfjet_unkown_pz",&ppfjet_unkown_pz_);
  tree->SetBranchAddress("ppfjet_electron_pz",&ppfjet_electron_pz_);
  tree->SetBranchAddress("ppfjet_muon_pz",&ppfjet_muon_pz_);
  tree->SetBranchAddress("ppfjet_photon_pz",&ppfjet_photon_pz_);
  tree->SetBranchAddress("ppfjet_unkown_EcalE",&ppfjet_unkown_EcalE_);
  tree->SetBranchAddress("ppfjet_electron_EcalE",&ppfjet_electron_EcalE_);
  tree->SetBranchAddress("ppfjet_muon_EcalE",&ppfjet_muon_EcalE_);
  tree->SetBranchAddress("ppfjet_photon_EcalE",&ppfjet_photon_EcalE_);
  tree->SetBranchAddress("ppfjet_unkown_n",&ppfjet_unkown_n_);
  tree->SetBranchAddress("ppfjet_electron_n",&ppfjet_electron_n_);
  tree->SetBranchAddress("ppfjet_muon_n",&ppfjet_muon_n_);
  tree->SetBranchAddress("ppfjet_photon_n",&ppfjet_photon_n_);
  tree->SetBranchAddress("ppfjet_had_n",&ppfjet_had_n_);
  tree->SetBranchAddress("ppfjet_had_E",&ppfjet_had_E_);
  tree->SetBranchAddress("ppfjet_had_px",&ppfjet_had_px_);
  tree->SetBranchAddress("ppfjet_had_py",&ppfjet_had_py_);
  tree->SetBranchAddress("ppfjet_had_pz",&ppfjet_had_pz_);
  tree->SetBranchAddress("ppfjet_had_EcalE",&ppfjet_had_EcalE_);
  tree->SetBranchAddress("ppfjet_had_rawHcalE",&ppfjet_had_rawHcalE_);
  tree->SetBranchAddress("ppfjet_had_emf",&ppfjet_had_emf_);
  tree->SetBranchAddress("ppfjet_had_id",&ppfjet_had_id_);
  tree->SetBranchAddress("ppfjet_had_candtrackind",&ppfjet_had_candtrackind_);
  tree->SetBranchAddress("ppfjet_had_E_mctruth",&ppfjet_had_E_mctruth_);
  tree->SetBranchAddress("ppfjet_had_mcpdgId",&ppfjet_had_mcpdgId_);
  tree->SetBranchAddress("ppfjet_had_ntwrs",&ppfjet_had_ntwrs_);
  tree->SetBranchAddress("ppfjet_ntwrs",&ppfjet_ntwrs_);
  tree->SetBranchAddress("ppfjet_twr_ieta",&ppfjet_twr_ieta_);
  tree->SetBranchAddress("ppfjet_twr_iphi",&ppfjet_twr_iphi_);
  tree->SetBranchAddress("ppfjet_twr_hade",&ppfjet_twr_hade_);
  tree->SetBranchAddress("ppfjet_twr_frac",&ppfjet_twr_frac_);
  tree->SetBranchAddress("ppfjet_twr_candtrackind",&ppfjet_twr_candtrackind_);
  tree->SetBranchAddress("ppfjet_twr_hadind",&ppfjet_twr_hadind_);
  tree->SetBranchAddress("ppfjet_twr_elmttype",&ppfjet_twr_elmttype_);
  tree->SetBranchAddress("ppfjet_twr_dR",&ppfjet_twr_dR_);
  tree->SetBranchAddress("ppfjet_twr_clusterind",&ppfjet_twr_clusterind_);
  tree->SetBranchAddress("ppfjet_cluster_n",&ppfjet_cluster_n_);
  tree->SetBranchAddress("ppfjet_cluster_eta",&ppfjet_cluster_eta_);
  tree->SetBranchAddress("ppfjet_cluster_phi",&ppfjet_cluster_phi_);
  tree->SetBranchAddress("ppfjet_cluster_dR",&ppfjet_cluster_dR_);
  tree->SetBranchAddress("ppfjet_twr_subdet",&ppfjet_twr_subdet_);
  tree->SetBranchAddress("ppfjet_ncandtracks",&ppfjet_ncandtracks_);
  tree->SetBranchAddress("ppfjet_candtrack_px",&ppfjet_candtrack_px_);
  tree->SetBranchAddress("ppfjet_candtrack_py",&ppfjet_candtrack_py_);
  tree->SetBranchAddress("ppfjet_candtrack_pz",&ppfjet_candtrack_pz_);
  tree->SetBranchAddress("ppfjet_candtrack_EcalE",&ppfjet_candtrack_EcalE_);
  tree->SetBranchAddress("pf_dijet_deta",&pf_dijet_deta_);
  tree->SetBranchAddress("pf_dijet_dphi",&pf_dijet_dphi_);
  tree->SetBranchAddress("pf_dijet_balance",&pf_dijet_balance_);
  tree->SetBranchAddress("pf_thirdjet_px",&pf_thirdjet_px_);
  tree->SetBranchAddress("pf_thirdjet_py",&pf_thirdjet_py_);
  tree->SetBranchAddress("pf_Run",&pf_Run_);
  tree->SetBranchAddress("pf_Lumi",&pf_Lumi_);
  tree->SetBranchAddress("pf_Event",&pf_Event_);

  // Jet
  TH1D* h_tag_jet_Ediff_once_track_cluster_ = new TH1D("h_tag_jet_EoverE_nocorr","tag E_{reconstructed}/E_{gen} with c_{i} = 1",200,0,2);
  TH1D* h_tag_jet_Ediff_once_track_cluster_corr_ = new TH1D("h_tag_jet_EoverE_respcorr","tag E_{reconstructed}/E_{gen} with response corrections",200,0,2);
  TH2D* h_tag_jet_EoverE_2D_ = new TH2D("h_tag_jet_EoverE_2D_nocorr","tag E_{reconstructed}/E_{gen} vs. #eta with c_{i} = 1",100,-5,5,100,0,2);
  TH2D* h_tag_jet_EoverE_2D_corr_ = new TH2D("h_tag_jet_EoverE_2D_respcorr","tag E_{reconstructed}/E_{gen} vs. #eta with response corrections",100,-5,5,100,0,2);
  
  TH1D* h_probe_jet_Ediff_once_track_cluster_ = new TH1D("h_probe_jet_EoverE_nocorr","probe E_{reconstructed}/E_{gen} with c_{i} = 1",200,0,2);
  TH1D* h_probe_jet_Ediff_once_track_cluster_corr_ = new TH1D("h_probe_jet_EoverE_respcorr","probe E_{reconstructed}/E_{gen} with response corrections",200,0,2);
  TH2D* h_probe_jet_EoverE_2D_ = new TH2D("h_probe_jet_EoverE_2D_nocorr","probe E_{reconstructed}/E_{gen} vs. #eta with c_{i} = 1",100,-5,5,100,0,2);
  TH2D* h_probe_jet_EoverE_2D_corr_ = new TH2D("h_probe_jet_EoverE_2D_respcorr","probe E_{reconstructed}/E_{gen} vs. #eta with response corrections",100,-5,5,100,0,2);

  TH1D* h_dijet_balance_nocorr_ = new TH1D("h_dijet_balance_nocorr","Dijet balance with c_{i} = 1",200,-1,1);
  TH1D* h_dijet_balance_respcorr_ = new TH1D("h_dijet_balance_respcorr","Dijet balance with response corrections",200,-1,1);
  TH2D* h_dijet_balance_2D_nocorr_ = new TH2D("h_dijet_balance_2D_nocorr","Dijet balance vs. #eta with c_{i} = 1",100,-5,5,100,-1,1);
  TH2D* h_dijet_balance_2D_respcorr_ = new TH2D("h_dijet_balance_2D_respcorr","Dijet balance vs. #eta with response corrections",100,-5,5,100,-1,1);
  
  
  int nEvents = tree->GetEntries();
  std::cout << "Running over " << nEvents << " events" << std::endl;
  //nEvents = 1000;
  for(int iEvent=0; iEvent<nEvents; iEvent++){
    if(iEvent % 10000 == 0){
      std::cout << "Processing event " << iEvent << std::endl;
    }
    tree->GetEntry(iEvent);

    int passSel = 0;
    if(tpfjet_ntwrs_ == 0 || ppfjet_ntwrs_ == 0){
      passSel |= 0x80;
    }
    float tjet_Et = tpfjet_E_/cosh(tpfjet_eta_);
    float pjet_Et = ppfjet_E_/cosh(ppfjet_eta_);
    float minSumJetEt_ = 40.0;//40.0;
    float minJetEt_ = 20.0;//20.0;
    float maxThirdJetEt_ = 15.0;//15.0;
    float maxDeltaEta_ = 0.5;//0.5;
    if(tjet_Et + pjet_Et < minSumJetEt_) passSel |= 0x1;
    if(tjet_Et < minJetEt_ || pjet_Et < minJetEt_) passSel |= 0x2;
    if(sqrt(pf_thirdjet_px_*pf_thirdjet_px_ + pf_thirdjet_py_*pf_thirdjet_py_) > maxThirdJetEt_) passSel |= 0x4;
    if(pf_dijet_deta_ > maxDeltaEta_) passSel |= 0x8;

    if(passSel) continue;
    
    //////////////////////////
    // Fill tag histograms
    //////////////////////////

    float tag_jet_rechit_E_once_cluster = 0;
    float tag_jet_rechit_E_once_cluster_corr = 0;
    float tag_jet_hadEcalE = 0;
    float tag_jet_candNoRecHits_E = 0;
    for(int i=0; i<tpfjet_had_n_; i++){
      tag_jet_hadEcalE += tpfjet_had_EcalE_->at(i);
      if(tpfjet_had_ntwrs_->at(i) == 0 && tpfjet_had_candtrackind_->at(i) > -1){
	tag_jet_candNoRecHits_E += sqrt(tpfjet_candtrack_px_->at(tpfjet_had_candtrackind_->at(i))*tpfjet_candtrack_px_->at(tpfjet_had_candtrackind_->at(i)) + tpfjet_candtrack_py_->at(tpfjet_had_candtrackind_->at(i))*tpfjet_candtrack_py_->at(tpfjet_had_candtrackind_->at(i)) + tpfjet_candtrack_pz_->at(tpfjet_had_candtrackind_->at(i))*tpfjet_candtrack_pz_->at(tpfjet_had_candtrackind_->at(i))) - tpfjet_had_EcalE_->at(i);
      }
      for(int j=0; j<tpfjet_ntwrs_; j++){
	if(tpfjet_twr_hadind_->at(j) == i && tpfjet_twr_hade_->at(j) > 0.0){
	  if(tpfjet_twr_clusterind_->at(j) < 0 || tpfjet_cluster_dR_->at(tpfjet_twr_clusterind_->at(j)) < 0.5){
	    if(tpfjet_twr_frac_->at(j) < 1){
	      tag_jet_rechit_E_once_cluster += tpfjet_twr_hade_->at(j)*tpfjet_twr_frac_->at(j);
	      tag_jet_rechit_E_once_cluster_corr += tpfjet_twr_hade_->at(j)*tpfjet_twr_frac_->at(j)*h_corr_->GetBinContent(tpfjet_twr_ieta_->at(j) + 42);
	    }
	    else{
	      tag_jet_rechit_E_once_cluster += tpfjet_twr_hade_->at(j);
	      tag_jet_rechit_E_once_cluster_corr += tpfjet_twr_hade_->at(j)*h_corr_->GetBinContent(tpfjet_twr_ieta_->at(j) + 42);
	    }
	  }
	}
      }
    }
    
    float tag_jet_E_once_track_cluster = tag_jet_rechit_E_once_cluster + tag_jet_hadEcalE + tag_jet_candNoRecHits_E + tpfjet_unkown_E_ + tpfjet_electron_E_ + tpfjet_muon_E_ + tpfjet_photon_E_;
    h_tag_jet_Ediff_once_track_cluster_->Fill(tag_jet_E_once_track_cluster/tpfjet_genE_);
    h_tag_jet_EoverE_2D_->Fill(tpfjet_eta_,tag_jet_E_once_track_cluster/tpfjet_genE_);
    float tag_jet_E_once_track_cluster_corr = tag_jet_rechit_E_once_cluster_corr + tag_jet_hadEcalE + tag_jet_candNoRecHits_E + tpfjet_unkown_E_ + tpfjet_electron_E_ + tpfjet_muon_E_ + tpfjet_photon_E_;
    h_tag_jet_Ediff_once_track_cluster_corr_->Fill(tag_jet_E_once_track_cluster_corr/tpfjet_genE_);
    h_tag_jet_EoverE_2D_corr_->Fill(tpfjet_eta_,tag_jet_E_once_track_cluster_corr/tpfjet_genE_);
  
    float probe_jet_rechit_E_once_cluster = 0;
    float probe_jet_rechit_E_once_cluster_corr = 0;
    float probe_jet_hadEcalE = 0;
    float probe_jet_candNoRecHits_E = 0;
    for(int i=0; i<ppfjet_had_n_; i++){
      probe_jet_hadEcalE += ppfjet_had_EcalE_->at(i);
      if(ppfjet_had_ntwrs_->at(i) == 0 && ppfjet_had_candtrackind_->at(i) > -1){
	probe_jet_candNoRecHits_E += sqrt(ppfjet_candtrack_px_->at(ppfjet_had_candtrackind_->at(i))*ppfjet_candtrack_px_->at(ppfjet_had_candtrackind_->at(i)) + ppfjet_candtrack_py_->at(ppfjet_had_candtrackind_->at(i))*ppfjet_candtrack_py_->at(ppfjet_had_candtrackind_->at(i)) + ppfjet_candtrack_pz_->at(ppfjet_had_candtrackind_->at(i))*ppfjet_candtrack_pz_->at(ppfjet_had_candtrackind_->at(i))) - ppfjet_had_EcalE_->at(i);
      }
      for(int j=0; j<ppfjet_ntwrs_; j++){
	if(ppfjet_twr_hadind_->at(j) == i && ppfjet_twr_hade_->at(j) > 0.0){
	  if(ppfjet_twr_clusterind_->at(j) < 0 || ppfjet_cluster_dR_->at(ppfjet_twr_clusterind_->at(j)) < 0.5){
	    if(ppfjet_twr_frac_->at(j) < 1){
	      probe_jet_rechit_E_once_cluster += ppfjet_twr_hade_->at(j)*ppfjet_twr_frac_->at(j);
	      probe_jet_rechit_E_once_cluster_corr += ppfjet_twr_hade_->at(j)*ppfjet_twr_frac_->at(j)*h_corr_->GetBinContent(ppfjet_twr_ieta_->at(j) + 42);
	    }
	    else{
	      probe_jet_rechit_E_once_cluster += ppfjet_twr_hade_->at(j);
	      probe_jet_rechit_E_once_cluster_corr += ppfjet_twr_hade_->at(j)*h_corr_->GetBinContent(ppfjet_twr_ieta_->at(j) + 42);
	    }
	  }
	}
      }
    }

    float probe_jet_E_once_track_cluster = probe_jet_rechit_E_once_cluster + probe_jet_hadEcalE + probe_jet_candNoRecHits_E + ppfjet_unkown_E_ + ppfjet_electron_E_ + ppfjet_muon_E_ + ppfjet_photon_E_;
    h_probe_jet_Ediff_once_track_cluster_->Fill(probe_jet_E_once_track_cluster/ppfjet_genE_);
    h_probe_jet_EoverE_2D_->Fill(ppfjet_eta_,probe_jet_E_once_track_cluster/ppfjet_genE_);
    float probe_jet_E_once_track_cluster_corr = probe_jet_rechit_E_once_cluster_corr + probe_jet_hadEcalE + probe_jet_candNoRecHits_E + ppfjet_unkown_E_ + ppfjet_electron_E_ + ppfjet_muon_E_ + ppfjet_photon_E_;
    h_probe_jet_Ediff_once_track_cluster_corr_->Fill(probe_jet_E_once_track_cluster_corr/ppfjet_genE_);
    h_probe_jet_EoverE_2D_corr_->Fill(ppfjet_eta_,probe_jet_E_once_track_cluster_corr/tpfjet_genE_);

    // Dijet

    double tag_jet_Et_nocorr = tag_jet_E_once_track_cluster/cosh(tpfjet_eta_);
    double probe_jet_Et_nocorr = probe_jet_E_once_track_cluster/cosh(ppfjet_eta_);

    double tpx_nocorr = tag_jet_Et_nocorr*cos(tpfjet_phi_);
    double tpy_nocorr = tag_jet_Et_nocorr*sin(tpfjet_phi_);
    double ppx_nocorr = probe_jet_Et_nocorr*cos(ppfjet_phi_);
    double ppy_nocorr = probe_jet_Et_nocorr*sin(ppfjet_phi_);

    tpx_nocorr += 0.5*pf_thirdjet_px_;
    tpy_nocorr += 0.5*pf_thirdjet_py_;
    ppx_nocorr += 0.5*pf_thirdjet_px_;
    ppy_nocorr += 0.5*pf_thirdjet_py_;

    double tag_jet_Et_nocorr_prime = sqrt(tpx_nocorr*tpx_nocorr + tpy_nocorr*tpy_nocorr);
    double probe_jet_Et_nocorr_prime = sqrt(ppx_nocorr*ppx_nocorr + ppy_nocorr*ppy_nocorr);

    double balance_nocorr = 2.0*(tag_jet_Et_nocorr_prime - probe_jet_Et_nocorr_prime)/(tag_jet_Et_nocorr_prime + probe_jet_Et_nocorr_prime);
    h_dijet_balance_nocorr_->Fill(balance_nocorr);
    h_dijet_balance_2D_nocorr_->Fill(ppfjet_eta_,balance_nocorr);

    double tag_jet_Et_respcorr = tag_jet_E_once_track_cluster_corr/cosh(tpfjet_eta_);
    double probe_jet_Et_respcorr = probe_jet_E_once_track_cluster_corr/cosh(ppfjet_eta_);

    double tpx_respcorr = tag_jet_Et_respcorr*cos(tpfjet_phi_);
    double tpy_respcorr = tag_jet_Et_respcorr*sin(tpfjet_phi_);
    double ppx_respcorr = probe_jet_Et_respcorr*cos(ppfjet_phi_);
    double ppy_respcorr = probe_jet_Et_respcorr*sin(ppfjet_phi_);

    tpx_respcorr += 0.5*pf_thirdjet_px_;
    tpy_respcorr += 0.5*pf_thirdjet_py_;
    ppx_respcorr += 0.5*pf_thirdjet_px_;
    ppy_respcorr += 0.5*pf_thirdjet_py_;

    double tag_jet_Et_respcorr_prime = sqrt(tpx_respcorr*tpx_respcorr + tpy_respcorr*tpy_respcorr);
    double probe_jet_Et_respcorr_prime = sqrt(ppx_respcorr*ppx_respcorr + ppy_respcorr*ppy_respcorr);

    double balance_respcorr = 2.0*(tag_jet_Et_respcorr_prime - probe_jet_Et_respcorr_prime)/(tag_jet_Et_respcorr_prime + probe_jet_Et_respcorr_prime);
    h_dijet_balance_respcorr_->Fill(balance_respcorr);
    h_dijet_balance_2D_respcorr_->Fill(ppfjet_eta_,balance_respcorr);
  }

  //////////////////////////
  // Set Labels
  //////////////////////////

  // Tag
  
  h_tag_jet_Ediff_once_track_cluster_->SetLineColor(1);
  h_tag_jet_Ediff_once_track_cluster_->GetXaxis()->SetTitle("E_{reconstructed}/E_{gen}");
  h_tag_jet_Ediff_once_track_cluster_->GetYaxis()->SetTitle("events");

  h_tag_jet_Ediff_once_track_cluster_corr_->SetLineColor(2);
  h_tag_jet_Ediff_once_track_cluster_corr_->GetXaxis()->SetTitle("E_{reconstructed}/E_{gen}");
  h_tag_jet_Ediff_once_track_cluster_corr_->GetYaxis()->SetTitle("events");

  h_tag_jet_EoverE_2D_->GetXaxis()->SetTitle("#eta(probe)");
  h_tag_jet_EoverE_2D_->GetYaxis()->SetTitle("E_{reconstructed}/E_{gen}");

  h_tag_jet_EoverE_2D_corr_->GetXaxis()->SetTitle("#eta(probe)");
  h_tag_jet_EoverE_2D_corr_->GetYaxis()->SetTitle("E_{reconstructed}/E_{gen}");

  // Probe

  h_probe_jet_Ediff_once_track_cluster_->SetLineColor(1);
  h_probe_jet_Ediff_once_track_cluster_->GetXaxis()->SetTitle("E_{reconstructed}/E_{gen}");
  h_probe_jet_Ediff_once_track_cluster_->GetYaxis()->SetTitle("events");

  h_probe_jet_Ediff_once_track_cluster_corr_->SetLineColor(2);
  h_probe_jet_Ediff_once_track_cluster_corr_->GetXaxis()->SetTitle("E_{reconstructed}/E_{gen}");
  h_probe_jet_Ediff_once_track_cluster_corr_->GetYaxis()->SetTitle("events");

  h_probe_jet_EoverE_2D_->GetXaxis()->SetTitle("#eta(probe)");
  h_probe_jet_EoverE_2D_->GetYaxis()->SetTitle("E_{reconstructed}/E_{gen}");

  h_probe_jet_EoverE_2D_corr_->GetXaxis()->SetTitle("#eta(probe)");
  h_probe_jet_EoverE_2D_corr_->GetYaxis()->SetTitle("E_{reconstructed}/E_{gen}");

  // Dijet

  h_dijet_balance_nocorr_->SetLineColor(1);
  h_dijet_balance_nocorr_->GetXaxis()->SetTitle("dijet balance");
  h_dijet_balance_nocorr_->GetYaxis()->SetTitle("events");

  h_dijet_balance_respcorr_->SetLineColor(2);
  h_dijet_balance_respcorr_->GetXaxis()->SetTitle("dijet balance");
  h_dijet_balance_respcorr_->GetYaxis()->SetTitle("events");

  h_dijet_balance_2D_nocorr_->GetXaxis()->SetTitle("#eta(probe)");
  h_dijet_balance_2D_nocorr_->GetYaxis()->SetTitle("dijet balance");
  
  h_dijet_balance_2D_respcorr_->GetXaxis()->SetTitle("#eta(probe)");
  h_dijet_balance_2D_respcorr_->GetYaxis()->SetTitle("dijet balance");
 
  //////////////////////////
  // Save to file
  //////////////////////////

  TFile* fout = new TFile(output,"RECREATE");
  fout->cd();

  h_tag_jet_Ediff_once_track_cluster_->Write();
  h_tag_jet_Ediff_once_track_cluster_corr_->Write();
  h_tag_jet_EoverE_2D_->Write();
  h_tag_jet_EoverE_2D_corr_->Write();
  
  h_probe_jet_Ediff_once_track_cluster_->Write();
  h_probe_jet_Ediff_once_track_cluster_corr_->Write();
  h_probe_jet_EoverE_2D_->Write();
  h_probe_jet_EoverE_2D_corr_->Write();

  h_dijet_balance_nocorr_->Write();
  h_dijet_balance_respcorr_->Write();
  h_dijet_balance_2D_nocorr_->Write();
  h_dijet_balance_2D_respcorr_->Write();
  
  fout->Close();
  std::cout << "Created file:" << output << std::endl;

  return 0;
}

