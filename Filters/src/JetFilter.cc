#include "HcalClosureTest/Filters/interface/JetFilter.h"

//
// constructors and destructor
//
JetFilter::JetFilter(const edm::ParameterSet& iConfig)
{
  //now do what ever initialization is needed
  minNumJets_ = iConfig.getUntrackedParameter<int>("minNumJets");
  maxNumJets_ = iConfig.getUntrackedParameter<int>("maxNumJets");

  minFirstJetEt_ = iConfig.getUntrackedParameter<double>("minFirstJetEt");
  maxFirstJetEt_ = iConfig.getUntrackedParameter<double>("maxFirstJetEt");
  minSecondJetEt_ = iConfig.getUntrackedParameter<double>("minSecondJetEt");
  maxSecondJetEt_ = iConfig.getUntrackedParameter<double>("maxSecondJetEt");
  minThirdJetEt_ = iConfig.getUntrackedParameter<double>("minThirdJetEt");
  maxThirdJetEt_ = iConfig.getUntrackedParameter<double>("maxThirdJetEt");
  minFourthJetEt_ = iConfig.getUntrackedParameter<double>("minFourthJetEt");
  maxFourthJetEt_ = iConfig.getUntrackedParameter<double>("maxFourthJetEt");
  minRestJetEt_ = iConfig.getUntrackedParameter<double>("minRestJetEt");
  maxRestJetEt_ = iConfig.getUntrackedParameter<double>("maxRestJetEt");

  caloJetCollName_ = iConfig.getUntrackedParameter<std::string>("caloJetCollName");
  tok_CaloJet_     = consumes<reco::CaloJetCollection>(caloJetCollName_);
}


JetFilter::~JetFilter()
{
 
  // do anything here that needs to be done at desctruction time
  // (e.g. close files, deallocate resources etc.)
  
}


//
// member functions
//

// ------------ method called on each new Event  ------------
bool
JetFilter::filter(edm::Event& iEvent, const edm::EventSetup& iSetup)
{
  // get the jet collection
  edm::Handle<reco::CaloJetCollection> handle;
  iEvent.getByToken(tok_CaloJet_, handle);
  if(!handle.isValid()) {
    throw edm::Exception(edm::errors::ProductNotFound)
      << " could not find jetCollection named " << caloJetCollName_ << "\n.";
    return false;
  }

  // loop over the jets, and reject events with bad jet energies
  int counter=0;
  for(reco::CaloJetCollection::const_iterator iJet= handle->begin(); iJet!=handle->end(); ++iJet) {
    ++counter;
    double et = iJet->et();

    if(counter==1      && (et<minFirstJetEt_  || et>maxFirstJetEt_))  return false;
    else if(counter==2 && (et<minSecondJetEt_ || et>maxSecondJetEt_)) return false;
    else if(counter==3 && (et<minThirdJetEt_  || et>maxThirdJetEt_))  return false;
    else if(counter==4 && (et<minFourthJetEt_ || et>maxFourthJetEt_)) return false;
    else if(counter>4  && (et<minRestJetEt_   || et>maxRestJetEt_))   return false;
  }
  
  if(counter<minNumJets_ || counter>maxNumJets_) return false;

  return true;
}

// ------------ method called once each job just before starting event loop  ------------
void 
JetFilter::beginJob(const edm::EventSetup&)
{
  return;
}

// ------------ method called once each job just after ending the event loop  ------------
void 
JetFilter::endJob() {
  return;
}

//define this as a plug-in
DEFINE_FWK_MODULE(JetFilter);
