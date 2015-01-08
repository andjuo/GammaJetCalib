#ifndef __SINGLE_PARTICLE_CLUSTER_PRODUCER_H__
#define __SINGLE_PARTICLE_CLUSTER_PRODUCER_H__

#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDProducer.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "DataFormats/Common/interface/Handle.h"
#include "DataFormats/CaloTowers/interface/CaloTowerCollection.h"
#include "DataFormats/HepMCCandidate/interface/GenParticle.h"
#include "DataFormats/Math/interface/LorentzVector.h"
#include <vector>

//
// class declaration
//

class SingleParticleClusterProducer : public edm::EDProducer
{
public:
  explicit SingleParticleClusterProducer(const edm::ParameterSet&);
  ~SingleParticleClusterProducer();
  
  
private:

  math::XYZTLorentzVector getP4From3x3Towers(const std::vector<CaloTowerCollection::const_iterator>& twrs) const;
  void get3x3TowersFromSeed(const edm::Handle<CaloTowerCollection>& twrcoll, const math::XYZTLorentzVector &p4, std::vector<CaloTowerCollection::const_iterator>& towervector) const;

  virtual void beginJob(const edm::EventSetup&) ;
  virtual void produce(edm::Event&, const edm::EventSetup&);
  virtual void endJob() ;

  // ----------member data ---------------------------
  edm::EDGetTokenT<std::vector<reco::GenParticle> > tok_GenPart_;
  edm::EDGetTokenT<CaloTowerCollection>             tok_CaloTower_; 
   
};

//
// constants, enums and typedefs
//

//
// static data member definitions
//


#endif
