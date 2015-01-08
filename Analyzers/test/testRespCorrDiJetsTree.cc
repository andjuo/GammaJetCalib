#include "HcalClosureTest/Analyzers/interface/testRespCorrDiJetsTree.h"

int main()
{
  TChain* tree = new TChain("pf_dijettree");
  TString input = "/eos/uscms/store/user/dgsheffi/QCD_Pt-15to3000_TuneD6T_Flat_8TeV_pythia6/DijetCalibration_dEta-1p5_Et-10_3rdEt-50/e02441adc4b1f61e7a01cc47fa7cba8d/tree_*.root";
  //TString input = "/uscms_data/d3/dgsheffi/HCal/Trees/QCD_Pt-15to3000_0030487D5E5F_cluster.root";
  //TString input = "/uscms_data/d3/dgsheffi/HCal/Trees/Pion_Pt-50.root";
  std::cout << "Opening file:" << input << std::endl;
  tree->Add(input);

  TString output = "/uscms_data/d3/dgsheffi/HCal/Trees/validation/QCD_Pt-15to3000_TuneD6T_Flat_8TeV_pythia6.root";
  //TString output = "/uscms_data/d3/dgsheffi/HCal/Trees/validation/Pion_Pt-50.root";

  TString corrname = "/uscms_data/d3/dgsheffi/HCal/corrections/QCD_Pt-15to3000_TuneD6R_Flat_8TeV_pythia6_dEta-0p5_Et-20_3rdEt-15.root";
  TFile* corrfile = new TFile(corrname);
  TH1D* h_corr_ = (TH1D*)corrfile->Get("h_corr");
  /*for(int i=1; i<84; i++){
    if(h_corr_->GetBinContent(i) < 0.5 || h_corr_->GetBinContent(i) > 2.0) h_corr_->SetBinContent(i,1.0);
    }*/
  
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

  int debugEvent = 372; //372 1432

  // Jet
  TH1D* h_tag_jet_Ediff_ = new TH1D("h_tag_jet_Ediff","tag (rechits - pfjet)/pfjet",200,-1,8);
  TH1D* h_tag_jet_Ediff_once_ = new TH1D("h_tag_jet_Ediff_once","tag (rechits - pfjet)/pfjet use rechits once",200,-1,8);
  TH1D* h_tag_jet_Ediff_once_track_ = new TH1D("h_tag_jet_Ediff_once_track","tag (rechits - pfjet)/pfjet use rechits once tracks for candidates without rechits",200,0,2);
  TH1D* h_tag_jet_Ediff_once_track_HB_ = new TH1D("h_tag_jet_Ediff_once_track_HB","tag (rechits - pfjet)/pfjet use rechits once tracks for candidates without rechits in HB",200,0,2);
  TH1D* h_tag_jet_Ediff_once_track_HE_ = new TH1D("h_tag_jet_Ediff_once_track_HE","tag (rechits - pfjet)/pfjet use rechits once tracks for candidates without rechits in HE",200,0,2);
  TH1D* h_tag_jet_Ediff_once_track_HF_ = new TH1D("h_tag_jet_Ediff_once_track_HF","tag (rechits - pfjet)/pfjet use rechits once tracks for candidates without rechits in HF",200,0,2);
  TH1D* h_tag_jet_Ediff_once_track_cluster_ = new TH1D("h_tag_jet_Ediff_once_track_cluster","tag (rechits - pfjet)/pfjet use rechits once tracks for candidates without rechits",200,0,2);
  TH1D* h_tag_jet_Ediff_once_track_cluster_corr_ = new TH1D("h_tag_jet_Ediff_once_track_cluster_corr","tag (rechits - pfjet)/pfjet use rechits once tracks for candidates without rechits and corrections",200,0,2);
  TH2D* h_tag_jet_Ediff_once_track_cluster_2d_ = new TH2D("h_tag_jet_Ediff_once_track_cluster_2d","tag (rechits - pfjet)/pfjet use rechits once tracks for candidates without rechits",200,0,5,25,0.3,1.55);
  TH1D* h_tag_jet_Ediff_once_track_cluster_range_ = new TH1D("h_tag_jet_Ediff_once_track_cluster_range","Mean and RMS for cuts with tag jet",25,0.275,1.525);
  TH1D* h_tag_jet_Ediff_once_track_nofrac_ = new TH1D("h_tag_jet_Ediff_once_track_nofrac","tag (rechits - pfjet)/pfjet use rechits once tracks for candidates without rechits",200,0,2);
  TH1D* h_tag_jet_Ediff_once_track_nofrac_HB_ = new TH1D("h_tag_jet_Ediff_once_track_nofrac_HB","tag (rechits - pfjet)/pfjet use rechits once tracks for candidates without rechits in HB",200,0,2);
  TH1D* h_tag_jet_Ediff_once_track_nofrac_HE_ = new TH1D("h_tag_jet_Ediff_once_track_nofrac_HE","tag (rechits - pfjet)/pfjet use rechits once tracks for candidates without rechits in hE",200,0,2);
  TH1D* h_tag_jet_Ediff_once_track_nofrac_HF_ = new TH1D("h_tag_jet_Ediff_once_track_nofrac_HF","tag (rechits - pfjet)/pfjet use rechits once tracks for candidates without rechits in HF",200,0,2);
  TH1D* h_tag_jet_Ediff_once_track_nofrac_nocut_ = new TH1D("h_tag_jet_Ediff_once_track_nofrac_nocut","tag (rechits - pfjet)/pfjet use rechits once tracks for candidates without rechits",200,0,10);
  TH1D* h_tag_jet_additionalE_ = new TH1D("h_tag_jet_additionalE","additional E from multiple rechits",200,-50,150);
  
  TH1D* h_tag_jet_eta_rechits_ = new TH1D("h_tag_jet_eta_rechits","tag #eta with rechits",200,-5,5);
  TH1D* h_tag_jet_eta_norechits_ = new TH1D("h_tag_jet_eta_norechits","tag #eta without rechits",200,-5,5);

  TH2D* h_tag_jet_fracvsdR_HB_ = new TH2D("h_tag_jet_fracvsdR_HB","fraction vs. #DeltaR in HB",200,0,5,200,0,1.5);
  TH1D* h_tag_jet_dR_HB_ = new TH1D("h_tag_jet_dR_HB","#DeltaR in HB",200,0,5);
  TH1D* h_tag_jet_frac_HB_ = new TH1D("h_tag_jet_frac_HB","fraction in HB",200,0,1.1);
  TH2D* h_tag_jet_fracvsdR_HE_ = new TH2D("h_tag_jet_fracvsdR_HE","fraction vs. #DeltaR in HE",200,0,5,200,0,1.5);
  TH1D* h_tag_jet_dR_HE_ = new TH1D("h_tag_jet_dR_HE","#DeltaR in HE",200,0,5);
  TH1D* h_tag_jet_frac_HE_ = new TH1D("h_tag_jet_frac_HE","fraction in HE",200,0,1.1);
  TH2D* h_tag_jet_rechitpos_ = new TH2D("h_tag_jet_rechitpos","rechits in tag jet",83,-41.5,41.5,72,-0.5,71.5);
  TH2D* h_tag_jet_rechitpos_R5_ = new TH2D("h_tag_jet_rechitpos_R5","rechits in tag jet #DeltaR < 0.5",83,-41.5,41.5,72,-0.5,71.5);
  TH2D* h_tag_jet_rechitpos_clusterR5_ = new TH2D("h_tag_jet_rechitpos_clusterR5","clusters in tag jet #DeltaR < 0.5",83,-41.5,41.5,72,-0.5,71.5);
  TH2D* h_tag_jet_candpos_ = new TH2D("h_tag_jet_candpos","candidates in tag jet",167,-41.5,41.5,144,-0.5,71.5);

  TH1D* h_probe_jet_Ediff_once_track_ = new TH1D("h_probe_jet_Ediff_once_track","probe (rechits - pfjet)/pfjet use rechits once tracks for candidates without rechits",200,0,2);
  TH1D* h_probe_jet_Ediff_once_track_HB_ = new TH1D("h_probe_jet_Ediff_once_track_HB","probe (rechits - pfjet)/pfjet use rechits once tracks for candidates without rechits in HB",200,0,2);
  TH1D* h_probe_jet_Ediff_once_track_HE_ = new TH1D("h_probe_jet_Ediff_once_track_HE","probe (rechits - pfjet)/pfjet use rechits once tracks for candidates without rechits in HE",200,0,2);
  TH1D* h_probe_jet_Ediff_once_track_HF_ = new TH1D("h_probe_jet_Ediff_once_track_HF","probe (rechits - pfjet)/pfjet use rechits once tracks for candidates without rechits in HF",200,0,2);
  TH1D* h_probe_jet_Ediff_once_track_cluster_ = new TH1D("h_probe_jet_Ediff_once_track_cluster","probe (rechits - pfjet)/pfjet use rechits once tracks for candidates without rechits",200,0,2);
  TH1D* h_probe_jet_Ediff_once_track_cluster_corr_ = new TH1D("h_probe_jet_Ediff_once_track_cluster_corr","probe (rechits - pfjet)/pfjet use rechits once tracks for candidates without rechits and corrections",200,0,2);
  TH2D* h_probe_jet_Ediff_once_track_cluster_2d_ = new TH2D("h_probe_jet_Ediff_once_track_cluster_2d","probe (rechits - pfjet)/pfjet use rechits once tracks for candidates without rechits",200,0,5,25,0.3,1.55);
  TH1D* h_probe_jet_Ediff_once_track_cluster_range_ = new TH1D("h_probe_jet_Ediff_once_track_cluster_range","Mean and RMS for cuts with probe jet",25,0.275,1.525);
  TH1D* h_probe_jet_Ediff_once_track_nofrac_ = new TH1D("h_probe_jet_Ediff_once_track_nofrac","probe (rechits - pfjet)/pfjet use rechits once tracks for candidates without rechits",200,0,2);
  TH1D* h_probe_jet_Ediff_once_track_nofrac_HB_ = new TH1D("h_probe_jet_Ediff_once_track_nofrac_HB","probe (rechits - pfjet)/pfjet use rechits once tracks for candidates without rechits in HB",200,-1,2);
  TH1D* h_probe_jet_Ediff_once_track_nofrac_HE_ = new TH1D("h_probe_jet_Ediff_once_track_nofrac_HE","probe (rechits - pfjet)/pfjet use rechits once tracks for candidates without rechits in hE",200,-1,2);
  TH1D* h_probe_jet_Ediff_once_track_nofrac_HF_ = new TH1D("h_probe_jet_Ediff_once_track_nofrac_HF","probe (rechits - pfjet)/pfjet use rechits once tracks for candidates without rechits in HF",200,-1,2);
  TH1D* h_probe_jet_Ediff_once_track_nofrac_nocut_ = new TH1D("h_probe_jet_Ediff_once_track_nofrac_nocut","probe (rechits - pfjet)/pfjet use rechits once tracks for candidates without rechits",200,0,10);
  TH1D* h_probe_jet_dR_HB_ = new TH1D("h_probe_jet_dR_HB","#DeltaR in HB",200,0,5);
  TH1D* h_probe_jet_frac_HB_ = new TH1D("h_probe_jet_frac_HB","fraction in HB",200,0,1.1);
  TH1D* h_probe_jet_dR_HE_ = new TH1D("h_probe_jet_dR_HE","#DeltaR in HE",200,0,5);
  TH1D* h_probe_jet_frac_HE_ = new TH1D("h_probe_jet_frac_HE","fraction in HE",200,0,1.1);
  TH2D* h_probe_jet_rechitpos_ = new TH2D("h_probe_jet_rechitpos","rechits in probe jet",83,-41.5,41.5,72,-0.5,71.5);
  TH2D* h_probe_jet_rechitpos_R5_ = new TH2D("h_probe_jet_rechitpos_R5","rechits in probe jet #DeltaR < 0.5",83,-41.5,41.5,72,-0.5,71.5);
  TH2D* h_probe_jet_rechitpos_clusterR5_ = new TH2D("h_probe_jet_rechitpos_clusterR5","clusters in probe jet #DeltaR < 0.5",83,-41.5,41.5,72,-0.5,71.5);
  TH2D* h_probe_jet_candpos_ = new TH2D("h_probe_jet_candpos","candidates in probe jet",167,-41.5,41.5,144,-0.5,71.5);

  TH1D* h_probe_jet_eta_rechits_ = new TH1D("h_probe_jet_eta_rechits","probe #eta with rechits",200,-5,5);
  TH1D* h_probe_jet_eta_norechits_ = new TH1D("h_probe_jet_eta_norechits","probe #eta without rechits",200,-5,5);

  TH1D* h_dijetbalance_PFJet_ = new TH1D("h_dijetbalance_PFJet","Dijet balance for PFJets",200,-2,2);
  TH1D* h_dijetbalance_reconstructed_ = new TH1D("h_dijetbalance_reconstructed","Dijet balance for reconstructed jets",200,-2,2);

  int nEventsNoRecHits = 0;
  int nEventsNoTagRecHits = 0;
  int nEventsNoProbeRecHits = 0;

  int nEvents = tree->GetEntries();
  std::cout << "Running over " << nEvents << " events" << std::endl;
  //nEvents = 5;
  for(int iEvent=0; iEvent<nEvents; iEvent++){
    if(iEvent % 10000 == 0){
      std::cout << "Processing event " << iEvent << std::endl;
    }
    tree->GetEntry(iEvent);
    
    //////////////////////////
    // Fill tag histograms
    //////////////////////////

    if(tpfjet_ntwrs_ == 0){
      nEventsNoTagRecHits++;
      h_tag_jet_eta_norechits_->Fill(tpfjet_eta_);
    }
    else{
      h_tag_jet_eta_rechits_->Fill(tpfjet_eta_);
    }
    if(ppfjet_ntwrs_ == 0){
      nEventsNoProbeRecHits++;
      h_probe_jet_eta_norechits_->Fill(ppfjet_eta_);
    }
    else{
      h_probe_jet_eta_rechits_->Fill(ppfjet_eta_);
    }
    if(tpfjet_ntwrs_ == 0 && ppfjet_ntwrs_ == 0) nEventsNoRecHits++;

    if(iEvent == debugEvent){
      std::cout << "tag " << tpfjet_eta_ << " " << tpfjet_phi_ << std::endl;
      std::cout << "probe " << ppfjet_eta_ << " " << ppfjet_phi_ << std::endl;
      for(int j=0; j<tpfjet_ntwrs_; j++){
	//if(tpfjet_twr_frac_->at(j) < 0.95) continue;
	h_tag_jet_rechitpos_->Fill(tpfjet_twr_ieta_->at(j),tpfjet_twr_iphi_->at(j),tpfjet_twr_hade_->at(j));
	if(tpfjet_twr_dR_->at(j) < 0.5) h_tag_jet_rechitpos_R5_->Fill(tpfjet_twr_ieta_->at(j),tpfjet_twr_iphi_->at(j),tpfjet_twr_hade_->at(j));
	if(tpfjet_twr_clusterind_->at(j) < 0 || tpfjet_cluster_dR_->at(tpfjet_twr_clusterind_->at(j)) < 0.5) h_tag_jet_rechitpos_clusterR5_->Fill(tpfjet_twr_ieta_->at(j),tpfjet_twr_iphi_->at(j),tpfjet_twr_hade_->at(j));
      }
      for(int j=0; j<ppfjet_ntwrs_; j++){
	//if(ppfjet_twr_frac_->at(j) < 0.95) continue;
	h_probe_jet_rechitpos_->Fill(ppfjet_twr_ieta_->at(j),ppfjet_twr_iphi_->at(j),ppfjet_twr_hade_->at(j));
	if(ppfjet_twr_dR_->at(j) < 0.5) h_probe_jet_rechitpos_R5_->Fill(ppfjet_twr_ieta_->at(j),ppfjet_twr_iphi_->at(j),ppfjet_twr_hade_->at(j));
	if(ppfjet_twr_clusterind_->at(j) < 0 || ppfjet_cluster_dR_->at(ppfjet_twr_clusterind_->at(j)) < 0.5) h_probe_jet_rechitpos_clusterR5_->Fill(ppfjet_twr_ieta_->at(j),ppfjet_twr_iphi_->at(j),ppfjet_twr_hade_->at(j));
      }
      for(int i=0; i<tpfjet_had_n_; i++){
	TLorentzVector tv(tpfjet_had_px_->at(i),tpfjet_had_py_->at(i),tpfjet_had_pz_->at(i),tpfjet_had_E_->at(i));
	float tmpphi = tv.Phi();
	if(tmpphi < 0) tmpphi += 2*3.141592653;
	h_tag_jet_candpos_->Fill(tv.Eta()/0.087,tmpphi/0.087,tpfjet_had_E_->at(i));
      }
      for(int i=0; i<ppfjet_had_n_; i++){
	TLorentzVector pv(ppfjet_had_px_->at(i),ppfjet_had_py_->at(i),ppfjet_had_pz_->at(i),ppfjet_had_E_->at(i));
	//std::cout << pv.Eta() << std::endl;
	float tmpphi = pv.Phi();
	if(tmpphi < 0) tmpphi += 2*3.141592653;
	h_probe_jet_candpos_->Fill(pv.Eta()/0.087,tmpphi/0.087,ppfjet_had_E_->at(i));
      }
    }

    float tag_jet_rechit_E = 0;
    float tag_jet_rechit_E_once = 0;
    float tag_jet_rechit_E_once_cluster = 0;
    float tag_jet_rechit_E_once_cluster_corr = 0;
    float tag_jet_rechit_E_once_cluster_arr[25] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
    float tag_jet_rechit_E_once_nofrac = 0;
    float tag_jet_rechit_E_once_nofrac_nocut = 0;
    float tag_jet_hadEcalE = 0;
    float tag_jet_candNoRecHits_E = 0;
    for(int i=0; i<tpfjet_had_n_; i++){
      tag_jet_hadEcalE += tpfjet_had_EcalE_->at(i);
      if(tpfjet_had_ntwrs_->at(i) == 0 && tpfjet_had_candtrackind_->at(i) > -1){
	tag_jet_candNoRecHits_E += sqrt(tpfjet_candtrack_px_->at(tpfjet_had_candtrackind_->at(i))*tpfjet_candtrack_px_->at(tpfjet_had_candtrackind_->at(i)) + tpfjet_candtrack_py_->at(tpfjet_had_candtrackind_->at(i))*tpfjet_candtrack_py_->at(tpfjet_had_candtrackind_->at(i)) + tpfjet_candtrack_pz_->at(tpfjet_had_candtrackind_->at(i))*tpfjet_candtrack_pz_->at(tpfjet_had_candtrackind_->at(i))) - tpfjet_had_EcalE_->at(i);
      }
      for(int j=0; j<tpfjet_ntwrs_; j++){
	if(tpfjet_twr_hadind_->at(j) == i && tpfjet_twr_hade_->at(j) > 0.0){
	  switch(tpfjet_twr_subdet_->at(j)){
	  case 1:
	    h_tag_jet_fracvsdR_HB_->Fill(tpfjet_twr_dR_->at(j),tpfjet_twr_frac_->at(j));
	    h_tag_jet_dR_HB_->Fill(tpfjet_twr_dR_->at(j));
	    h_tag_jet_frac_HB_->Fill(tpfjet_twr_frac_->at(j));
	    break;
	  case 2:
	    h_tag_jet_fracvsdR_HE_->Fill(tpfjet_twr_dR_->at(j),tpfjet_twr_frac_->at(j));
	    h_tag_jet_dR_HE_->Fill(tpfjet_twr_dR_->at(j));
	    h_tag_jet_frac_HE_->Fill(tpfjet_twr_frac_->at(j));
	    break;
	  default:
	    break;
	  }
	  tag_jet_rechit_E_once_nofrac_nocut += tpfjet_twr_hade_->at(j);
	  if(tpfjet_twr_dR_->at(j) < 0.5){
	    tag_jet_rechit_E += tpfjet_twr_hade_->at(j)*tpfjet_twr_frac_->at(j);
	    if(tpfjet_twr_frac_->at(j) < 1){
	      tag_jet_rechit_E_once += tpfjet_twr_hade_->at(j)*tpfjet_twr_frac_->at(j);
	    }
	    else{
	      tag_jet_rechit_E_once += tpfjet_twr_hade_->at(j);
	    }
	    tag_jet_rechit_E_once_nofrac += tpfjet_twr_hade_->at(j);
	  }
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
	  for(int k=0; k<25; k++){
	    if(tpfjet_twr_clusterind_->at(j) < 0 || tpfjet_cluster_dR_->at(tpfjet_twr_clusterind_->at(j)) < static_cast<float>(k)/20.0 + 0.3){
	      if(tpfjet_twr_frac_->at(j) < 1){
		tag_jet_rechit_E_once_cluster_arr[k] += tpfjet_twr_hade_->at(j)*tpfjet_twr_frac_->at(j);
	      }
	      else{
		tag_jet_rechit_E_once_cluster_arr[k] += tpfjet_twr_hade_->at(j);
	      }
	    }
	  }
	}
      }
    }
    
    float tag_jet_E = tag_jet_rechit_E + tag_jet_hadEcalE + tpfjet_unkown_E_ + tpfjet_electron_E_ + tpfjet_muon_E_ + tpfjet_photon_E_;
    h_tag_jet_Ediff_->Fill((tag_jet_E - tpfjet_E_)/tpfjet_E_);
    float tag_jet_E_once = tag_jet_rechit_E_once + tag_jet_hadEcalE + tpfjet_unkown_E_ + tpfjet_electron_E_ + tpfjet_muon_E_ + tpfjet_photon_E_;
    h_tag_jet_Ediff_once_->Fill((tag_jet_E_once - tpfjet_E_)/tpfjet_E_);
    float tag_jet_E_once_track = tag_jet_rechit_E_once + tag_jet_hadEcalE + tag_jet_candNoRecHits_E + tpfjet_unkown_E_ + tpfjet_electron_E_ + tpfjet_muon_E_ + tpfjet_photon_E_;
    h_tag_jet_Ediff_once_track_->Fill(tag_jet_E_once_track/tpfjet_E_);
    if(fabs(tpfjet_eta_) < 1.305){
      h_tag_jet_Ediff_once_track_HB_->Fill(tag_jet_E_once_track/tpfjet_E_);
    }
    else if(fabs(tpfjet_eta_) < 2.853){
      h_tag_jet_Ediff_once_track_HE_->Fill(tag_jet_E_once_track/tpfjet_E_);
    }
    else{
      h_tag_jet_Ediff_once_track_HF_->Fill(tag_jet_E_once_track/tpfjet_E_);
    }
    float tag_jet_E_once_track_cluster = tag_jet_rechit_E_once_cluster + tag_jet_hadEcalE + tag_jet_candNoRecHits_E + tpfjet_unkown_E_ + tpfjet_electron_E_ + tpfjet_muon_E_ + tpfjet_photon_E_;
    h_tag_jet_Ediff_once_track_cluster_->Fill(tag_jet_E_once_track_cluster/tpfjet_E_);
    float tag_jet_E_once_track_cluster_corr = tag_jet_rechit_E_once_cluster_corr + tag_jet_hadEcalE + tag_jet_candNoRecHits_E + tpfjet_unkown_E_ + tpfjet_electron_E_ + tpfjet_muon_E_ + tpfjet_photon_E_;
    h_tag_jet_Ediff_once_track_cluster_corr_->Fill(tag_jet_E_once_track_cluster_corr/tpfjet_E_);
    for(int k=0; k<25; k++){
      float tag_jet_E_once_track_cluster_arr = tag_jet_rechit_E_once_cluster_arr[k] + tag_jet_hadEcalE + tag_jet_candNoRecHits_E + tpfjet_unkown_E_ + tpfjet_electron_E_ + tpfjet_muon_E_ + tpfjet_photon_E_;
      h_tag_jet_Ediff_once_track_cluster_2d_->Fill(tag_jet_E_once_track_cluster_arr/tpfjet_E_,static_cast<float>(k)/20.0 + 0.31);
    }
    float tag_jet_E_once_track_nofrac = tag_jet_rechit_E_once_nofrac + tag_jet_hadEcalE + tag_jet_candNoRecHits_E + tpfjet_unkown_E_ + tpfjet_electron_E_ + tpfjet_muon_E_ + tpfjet_photon_E_;
    h_tag_jet_Ediff_once_track_nofrac_->Fill(tag_jet_E_once_track_nofrac/tpfjet_E_);
    if(fabs(tpfjet_eta_) < 1.305){
      h_tag_jet_Ediff_once_track_nofrac_HB_->Fill((tag_jet_E_once_track_nofrac - tpfjet_E_)/tpfjet_E_);
    }
    else if(fabs(tpfjet_eta_) < 2.853){
      h_tag_jet_Ediff_once_track_nofrac_HE_->Fill((tag_jet_E_once_track_nofrac - tpfjet_E_)/tpfjet_E_);
    }
    else{
      h_tag_jet_Ediff_once_track_nofrac_HF_->Fill((tag_jet_E_once_track_nofrac - tpfjet_E_)/tpfjet_E_);
    }
    float tag_jet_E_once_track_nofrac_nocut = tag_jet_rechit_E_once_nofrac_nocut + tag_jet_hadEcalE + tag_jet_candNoRecHits_E + tpfjet_unkown_E_ + tpfjet_electron_E_ + tpfjet_muon_E_ + tpfjet_photon_E_;
    h_tag_jet_Ediff_once_track_nofrac_nocut_->Fill(tag_jet_E_once_track_nofrac_nocut/tpfjet_E_);

    h_tag_jet_additionalE_->Fill(tag_jet_rechit_E - tag_jet_rechit_E_once);
  
    float probe_jet_rechit_E_once = 0;
    float probe_jet_rechit_E_once_cluster = 0;
    float probe_jet_rechit_E_once_cluster_corr = 0;
    float probe_jet_rechit_E_once_cluster_arr[25] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
    float probe_jet_rechit_E_once_nofrac = 0;
    float probe_jet_rechit_E_once_nofrac_nocut = 0;
    float probe_jet_hadEcalE = 0;
    float probe_jet_candNoRecHits_E = 0;
    for(int i=0; i<ppfjet_had_n_; i++){
      probe_jet_hadEcalE += ppfjet_had_EcalE_->at(i);
      if(ppfjet_had_ntwrs_->at(i) == 0 && ppfjet_had_candtrackind_->at(i) > -1){
	probe_jet_candNoRecHits_E += sqrt(ppfjet_candtrack_px_->at(ppfjet_had_candtrackind_->at(i))*ppfjet_candtrack_px_->at(ppfjet_had_candtrackind_->at(i)) + ppfjet_candtrack_py_->at(ppfjet_had_candtrackind_->at(i))*ppfjet_candtrack_py_->at(ppfjet_had_candtrackind_->at(i)) + ppfjet_candtrack_pz_->at(ppfjet_had_candtrackind_->at(i))*ppfjet_candtrack_pz_->at(ppfjet_had_candtrackind_->at(i))) - ppfjet_had_EcalE_->at(i);
      }
      for(int j=0; j<ppfjet_ntwrs_; j++){
	if(ppfjet_twr_hadind_->at(j) == i && ppfjet_twr_hade_->at(j) > 0.0){
	  switch(ppfjet_twr_subdet_->at(j)){
	  case 1:
	    h_probe_jet_dR_HB_->Fill(ppfjet_twr_dR_->at(j));
	    h_probe_jet_frac_HB_->Fill(ppfjet_twr_frac_->at(j));
	    break;
	  case 2:
	    h_probe_jet_dR_HE_->Fill(ppfjet_twr_dR_->at(j));
	    h_probe_jet_frac_HE_->Fill(ppfjet_twr_frac_->at(j));
	    break;
	  default:
	    break;
	  }
	  probe_jet_rechit_E_once_nofrac_nocut += ppfjet_twr_hade_->at(j);
	  if(ppfjet_twr_dR_->at(j) < 0.5){
	    probe_jet_rechit_E_once_nofrac += ppfjet_twr_hade_->at(j);
	    if(ppfjet_twr_frac_->at(j) < 1){
	      probe_jet_rechit_E_once += ppfjet_twr_hade_->at(j)*ppfjet_twr_frac_->at(j);
	    }
	    else{
	      probe_jet_rechit_E_once += ppfjet_twr_hade_->at(j);
	    }
	  }
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
	  for(int k=0; k<25; k++){
	    if(ppfjet_twr_clusterind_->at(j) < 0 || ppfjet_cluster_dR_->at(ppfjet_twr_clusterind_->at(j)) < static_cast<float>(k)/20.0 + 0.3){
	      if(ppfjet_twr_frac_->at(j) < 1){
		probe_jet_rechit_E_once_cluster_arr[k] += ppfjet_twr_hade_->at(j)*ppfjet_twr_frac_->at(j);
	      }
	      else{
		probe_jet_rechit_E_once_cluster_arr[k] += ppfjet_twr_hade_->at(j);
	      }
	    }
	  }
	}
      }
    }

    float probe_jet_E_once_track = probe_jet_rechit_E_once + probe_jet_hadEcalE + probe_jet_candNoRecHits_E + ppfjet_unkown_E_ + ppfjet_electron_E_ + ppfjet_muon_E_ + ppfjet_photon_E_;
    h_probe_jet_Ediff_once_track_->Fill(probe_jet_E_once_track/ppfjet_E_);
    if(fabs(ppfjet_eta_) < 1.305){
      h_probe_jet_Ediff_once_track_HB_->Fill(probe_jet_E_once_track/ppfjet_E_);
    }
    else if(fabs(ppfjet_eta_) < 2.853){
      h_probe_jet_Ediff_once_track_HE_->Fill(probe_jet_E_once_track/ppfjet_E_);
    }
    else{
      h_probe_jet_Ediff_once_track_HF_->Fill(probe_jet_E_once_track/ppfjet_E_);
    }
    float probe_jet_E_once_track_cluster = probe_jet_rechit_E_once_cluster + probe_jet_hadEcalE + probe_jet_candNoRecHits_E + ppfjet_unkown_E_ + ppfjet_electron_E_ + ppfjet_muon_E_ + ppfjet_photon_E_;
    h_probe_jet_Ediff_once_track_cluster_->Fill(probe_jet_E_once_track_cluster/ppfjet_E_);
    float probe_jet_E_once_track_cluster_corr = probe_jet_rechit_E_once_cluster_corr + probe_jet_hadEcalE + probe_jet_candNoRecHits_E + ppfjet_unkown_E_ + ppfjet_electron_E_ + ppfjet_muon_E_ + ppfjet_photon_E_;
    h_probe_jet_Ediff_once_track_cluster_corr_->Fill(probe_jet_E_once_track_cluster_corr/ppfjet_E_);
    for(int k=0; k<25; k++){
      float probe_jet_E_once_track_cluster_arr = probe_jet_rechit_E_once_cluster_arr[k] + probe_jet_hadEcalE + probe_jet_candNoRecHits_E + ppfjet_unkown_E_ + ppfjet_electron_E_ + ppfjet_muon_E_ + ppfjet_photon_E_;
      h_probe_jet_Ediff_once_track_cluster_2d_->Fill(probe_jet_E_once_track_cluster_arr/ppfjet_E_,static_cast<float>(k)/20.0 + 0.31);
    }
    float probe_jet_E_once_track_nofrac = probe_jet_rechit_E_once_nofrac + probe_jet_hadEcalE + probe_jet_candNoRecHits_E + ppfjet_unkown_E_ + ppfjet_electron_E_ + ppfjet_muon_E_ + ppfjet_photon_E_;
    h_probe_jet_Ediff_once_track_nofrac_->Fill(probe_jet_E_once_track_nofrac/ppfjet_E_);
    if(fabs(ppfjet_eta_) < 1.305){
      h_probe_jet_Ediff_once_track_nofrac_HB_->Fill((probe_jet_E_once_track_nofrac - ppfjet_E_)/ppfjet_E_);
    }
    else if(fabs(tpfjet_eta_) < 2.853){
      h_probe_jet_Ediff_once_track_nofrac_HE_->Fill((probe_jet_E_once_track_nofrac - ppfjet_E_)/ppfjet_E_);
    }
    else{
      h_probe_jet_Ediff_once_track_nofrac_HF_->Fill((probe_jet_E_once_track_nofrac - ppfjet_E_)/ppfjet_E_);
    }
    float probe_jet_E_once_track_nofrac_nocut = probe_jet_rechit_E_once_nofrac_nocut + probe_jet_hadEcalE + probe_jet_candNoRecHits_E + ppfjet_unkown_E_ + ppfjet_electron_E_ + ppfjet_muon_E_ + ppfjet_photon_E_;
    h_probe_jet_Ediff_once_track_nofrac_nocut_->Fill(probe_jet_E_once_track_nofrac_nocut/ppfjet_E_);

    double pf_t_Et = tpfjet_E_/cosh(tpfjet_eta_);
    double pf_p_Et = ppfjet_E_/cosh(ppfjet_eta_);
    double rec_t_Et = tag_jet_E_once_track_cluster/cosh(tpfjet_eta_);
    double rec_p_Et = probe_jet_E_once_track_cluster/cosh(ppfjet_eta_);

    double pf_t_px = pf_t_Et*cos(tpfjet_phi_);
    double pf_t_py = pf_t_Et*sin(tpfjet_phi_);
    double pf_p_px = pf_p_Et*cos(ppfjet_phi_);
    double pf_p_py = pf_p_Et*sin(ppfjet_phi_);
    double rec_t_px = rec_t_Et*cos(tpfjet_phi_);
    double rec_t_py = rec_t_Et*sin(tpfjet_phi_);
    double rec_p_px = rec_p_Et*cos(ppfjet_phi_);
    double rec_p_py = rec_p_Et*sin(ppfjet_phi_);

    pf_t_px += 0.5*pf_thirdjet_px_;
    pf_t_py += 0.5*pf_thirdjet_py_;
    pf_p_px += 0.5*pf_thirdjet_px_;
    pf_p_py += 0.5*pf_thirdjet_py_;
    rec_t_px += 0.5*pf_thirdjet_px_;
    rec_t_py += 0.5*pf_thirdjet_py_;
    rec_p_px += 0.5*pf_thirdjet_px_;
    rec_p_py += 0.5*pf_thirdjet_py_;

    double pf_t_Etcorr = sqrt(pf_t_px*pf_t_px + pf_t_py*pf_t_py);
    double pf_p_Etcorr = sqrt(pf_p_px*pf_p_px + pf_p_py*pf_p_py);
    double rec_t_Etcorr = sqrt(rec_t_px*rec_t_px + rec_t_py*rec_t_py);
    double rec_p_Etcorr = sqrt(rec_p_px*rec_p_px + rec_p_py*rec_p_py);

    double pf_balance = 2.0*(pf_t_Etcorr - pf_p_Etcorr)/(pf_t_Etcorr + pf_p_Etcorr);
    double rec_balance = 2.0*(rec_t_Etcorr - rec_p_Etcorr)/(rec_t_Etcorr + rec_p_Etcorr);

    h_dijetbalance_PFJet_->Fill(pf_balance);
    h_dijetbalance_reconstructed_->Fill(rec_balance);
  }

  for(int k=0; k<25; k++){
    TH1D* tmpthist = (TH1D*)h_tag_jet_Ediff_once_track_cluster_2d_->ProfileX(" ",k+1,k+1);
    //std::cout << k << ": " << tmpthist->GetMean() << " " << tmpthist->GetRMS() << std::endl;
    h_tag_jet_Ediff_once_track_cluster_range_->SetBinContent(k+1,tmpthist->GetMean());
    h_tag_jet_Ediff_once_track_cluster_range_->SetBinError(k+1,tmpthist->GetRMS());

    TH1D* tmpphist = (TH1D*)h_probe_jet_Ediff_once_track_cluster_2d_->ProfileX(" ",k+1,k+1);
    h_probe_jet_Ediff_once_track_cluster_range_->SetBinContent(k+1,tmpphist->GetMean());
    h_probe_jet_Ediff_once_track_cluster_range_->SetBinError(k+1,tmpphist->GetRMS());
  }

  
  //////////////////////////
  // Save to file
  //////////////////////////

  TFile* fout = new TFile(output,"RECREATE");
  fout->cd();

  h_tag_jet_Ediff_->Write();
  h_tag_jet_Ediff_once_->Write();
  h_tag_jet_Ediff_once_track_->Write();
  h_tag_jet_Ediff_once_track_HB_->Write();
  h_tag_jet_Ediff_once_track_HE_->Write();
  h_tag_jet_Ediff_once_track_HF_->Write();
  h_tag_jet_Ediff_once_track_cluster_->Write();
  h_tag_jet_Ediff_once_track_cluster_corr_->Write();
  h_tag_jet_Ediff_once_track_cluster_range_->Write();
  h_tag_jet_Ediff_once_track_nofrac_->Write();
  h_tag_jet_Ediff_once_track_nofrac_HB_->Write();
  h_tag_jet_Ediff_once_track_nofrac_HE_->Write();
  h_tag_jet_Ediff_once_track_nofrac_HF_->Write();
  h_tag_jet_Ediff_once_track_nofrac_nocut_->Write();

  h_tag_jet_additionalE_->Write();

  h_tag_jet_eta_rechits_->Write();
  h_tag_jet_eta_norechits_->Write();

  h_tag_jet_fracvsdR_HB_->Write();
  h_tag_jet_dR_HB_->Write();
  h_tag_jet_frac_HB_->Write();
  h_tag_jet_fracvsdR_HE_->Write();
  h_tag_jet_dR_HE_->Write();
  h_tag_jet_frac_HE_->Write();
  h_tag_jet_rechitpos_->Write();
  h_tag_jet_rechitpos_R5_->Write();
  h_tag_jet_rechitpos_clusterR5_->Write();
  h_tag_jet_candpos_->Write();

  h_probe_jet_Ediff_once_track_->Write();
  h_probe_jet_Ediff_once_track_HB_->Write();
  h_probe_jet_Ediff_once_track_HE_->Write();
  h_probe_jet_Ediff_once_track_HF_->Write();
  h_probe_jet_Ediff_once_track_cluster_->Write();
  h_probe_jet_Ediff_once_track_cluster_corr_->Write();
  h_probe_jet_Ediff_once_track_cluster_range_->Write();
  h_probe_jet_Ediff_once_track_nofrac_->Write();
  h_probe_jet_Ediff_once_track_nofrac_HB_->Write();
  h_probe_jet_Ediff_once_track_nofrac_HE_->Write();
  h_probe_jet_Ediff_once_track_nofrac_HF_->Write();
  h_probe_jet_Ediff_once_track_nofrac_nocut_->Write();
  h_probe_jet_dR_HB_->Write();
  h_probe_jet_frac_HB_->Write();
  h_probe_jet_dR_HE_->Write();
  h_probe_jet_frac_HE_->Write();
  h_probe_jet_rechitpos_->Write();
  h_probe_jet_rechitpos_R5_->Write();
  h_probe_jet_rechitpos_clusterR5_->Write();
  h_probe_jet_candpos_->Write();
  h_probe_jet_eta_rechits_->Write();
  h_probe_jet_eta_norechits_->Write();

  h_dijetbalance_PFJet_->Write();
  h_dijetbalance_reconstructed_->Write();

  //h_tag_jet_Ediff_once_track_cluster_2d_->Write();
  //h_probe_jet_Ediff_once_track_cluster_2d_->Write();
  
  fout->Close();
  std::cout << "Created file:" << output << std::endl;

  std::cout << "Events without tag rechits: " << nEventsNoTagRecHits << "/" << nEvents << " = " << (double)nEventsNoTagRecHits/(double)nEvents << std::endl;
  std::cout << "Events without probe rechits: " << nEventsNoProbeRecHits << "/" << nEvents << " = " << (double)nEventsNoProbeRecHits/(double)nEvents << std::endl;
  std::cout << "Events without any rechits: " << nEventsNoRecHits << "/" << nEvents << " = " << (double)nEventsNoRecHits/(double)nEvents << std::endl;

  return 0;
}

