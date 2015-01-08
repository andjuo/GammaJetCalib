// system include files
#include <memory>

// user include files

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "HcalClosureTest/DataFormat/interface/SingleParticleCluster.h"
#include "HcalClosureTest/Producers/interface/SingleParticleClusterProducer.h"

#include "TMath.h"

// hcal respcorr include files
//#include "CondFormats/DataRecord/interface/HcalRespCorrsRcd.h"
//#include "CalibCalorimetry/HcalAlgos/interface/HcalDbASCIIIO.h"
//#include "FWCore/Framework/interface/EventSetup.h"
//#include "FWCore/Framework/interface/ESHandle.h"

//
// constructors and destructor
//

SingleParticleClusterProducer::SingleParticleClusterProducer(const edm::ParameterSet& iConfig)
{
  tok_GenPart_   = consumes<std::vector<reco::GenParticle> >(edm::InputTag("genParticles"));
  tok_CaloTower_ = consumes<CaloTowerCollection>(edm::InputTag("towerMaker"));
  produces<SingleParticleClusterCollection>();
}


SingleParticleClusterProducer::~SingleParticleClusterProducer()
{
}


//
// member functions
//

// ------------ method called to for each event  ------------
void
SingleParticleClusterProducer::produce(edm::Event& iEvent, const edm::EventSetup& iSetup)
{
  // this is what we're writing to the edm
  std::auto_ptr<SingleParticleClusterCollection> result(new SingleParticleClusterCollection);

  // get the genparticles
  edm::Handle<reco::GenParticleCollection > genpcoll;
  iEvent.getByToken(tok_GenPart_, genpcoll);
  if(!(genpcoll.isValid())) {
    //    LogError("SingleParticleClusterProducer") << "genParticles not found";
    return;
  }

  // get the calotower collection
  edm::Handle<CaloTowerCollection> twrcoll;
  iEvent.getByToken(tok_CaloTower_, twrcoll);
  if(!(twrcoll.isValid())) { // use message logger
    //    LogError("CaloCalibAnalyzer") << "towerMaker not found";
    return;
  }

  // loop over gen particles
  for(reco::GenParticleCollection::const_iterator iGen=genpcoll->begin(); iGen!=genpcoll->end(); ++iGen) {
    const reco::GenParticle p = *iGen;

    // iterate until we have a stable 3x3 cluster
    // using the momentum to seed it
    int loops;
    math::XYZTLorentzVector seedp4=p.p4();
    std::vector<CaloTowerCollection::const_iterator> oldtwrs, newtwrs;
    for(loops=0; loops<20; ++loops) {
      get3x3TowersFromSeed(twrcoll, seedp4, newtwrs);

      // compare oldtwrs to newtwrs
      std::sort(oldtwrs.begin(), oldtwrs.end());
      std::sort(newtwrs.begin(), newtwrs.end());
      if(oldtwrs.size()==newtwrs.size() && std::equal(oldtwrs.begin(), oldtwrs.end(), newtwrs.begin())) break;

      // if they are different, reseed with the new set of towers
      seedp4 = getP4From3x3Towers(newtwrs);
      oldtwrs=newtwrs;
    }

    // create the SingleParticleCluster
    SingleParticleCluster cluster;
    
    // add a reference to the gen particle in the cluster
    edm::Ref<reco::GenParticleCollection> myRef(genpcoll, iGen-genpcoll->begin());
    cluster.genp_=myRef;

    // add references to the towers in the cluster
    for(std::vector<CaloTowerCollection::const_iterator>::const_iterator it=newtwrs.begin(); it!=newtwrs.end(); ++it) {
      edm::Ref<CaloTowerCollection>  myRef2(twrcoll, (*it)-twrcoll->begin());
      cluster.calotowers_.push_back(myRef2);
    }

    // add the cluster to the collection
    result->push_back(cluster);
  }

  // write the collection to the event
  iEvent.put(result);

  return;
}

void SingleParticleClusterProducer::get3x3TowersFromSeed(const edm::Handle<CaloTowerCollection>& twrcoll, const math::XYZTLorentzVector &p4, std::vector<CaloTowerCollection::const_iterator>& towervector) const
{
  // clear the vector
  towervector.clear();

  // match to tower
  CaloTowerCollection::const_iterator seedtwr=twrcoll->end();
  double bestdr=999;
  for(CaloTowerCollection::const_iterator iTwr = twrcoll->begin(); iTwr!=twrcoll->end(); ++iTwr) {
    const CaloTower &twr=*iTwr;
      
    // calculate dR between particle and tower
    // pick smallest
    double deta=p4.eta() - twr.eta();
    double dphi=fabs(p4.phi() - twr.phi());
    if(dphi > TMath::Pi()) dphi = 2.0*TMath::Pi() - dphi;
    double dr = TMath::Sqrt(deta*deta+dphi*dphi);
    if(dr<bestdr) {
      bestdr=dr;
      seedtwr=iTwr;
    }
  }

  int seedieta=seedtwr->ieta();
  int seedietaabs=TMath::Abs(seedtwr->ieta());
  int seediphi=seedtwr->iphi();

  // cluster 3x3 about the seed tower
  for(CaloTowerCollection::const_iterator iTwr = twrcoll->begin(); iTwr!=twrcoll->end(); ++iTwr) {
    int twrieta=iTwr->ieta();
    int twrietaabs=TMath::Abs(iTwr->ieta());
    int twriphi=iTwr->iphi();

    if((twrieta<=seedieta+1 && twrieta>=seedieta-1) ||
       (seedietaabs==1 && twrietaabs==1)) {
      if(twrietaabs<20) {
	if(twriphi<=seediphi+1 && twriphi>=seediphi-1)
	  towervector.push_back(iTwr);
	if(seediphi==72 && twriphi==1)
	  towervector.push_back(iTwr);
	if(seediphi==1 && twriphi==72)
	  towervector.push_back(iTwr);
	
      } else {
	if(twriphi<=seediphi+2 && twriphi>=seediphi-2)
	  towervector.push_back(iTwr);
	if(seediphi==71 && twriphi==1)
	  towervector.push_back(iTwr);
	if(seediphi==1 && twriphi==71)
	  towervector.push_back(iTwr);
      }
    }
  }
  
  return;
}

math::XYZTLorentzVector SingleParticleClusterProducer::getP4From3x3Towers(const std::vector<CaloTowerCollection::const_iterator>& twrs) const
{
  math::XYZTLorentzVector p4;
  for(std::vector<CaloTowerCollection::const_iterator>::const_iterator it=twrs.begin(); it!=twrs.end(); ++it) {
    math::XYZTLorentzVector add = (*it)->p4();
    p4 += add;
  }
  return p4;
}

// ------------ method called once each job just before starting event loop  ------------
void 
SingleParticleClusterProducer::beginJob(const edm::EventSetup& iSetup)
{
}

// ------------ method called once each job just after ending the event loop  ------------
void 
SingleParticleClusterProducer::endJob() {
}


//define this as a plug-in
DEFINE_FWK_MODULE(SingleParticleClusterProducer);

