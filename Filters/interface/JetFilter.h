// Implementation:
//   <Notes on implementation>
//
//
// Original Author:  "John Paul Chou"
//         Created:  Mon Apr 20 20:03:30 CDT 2009
// $Id$
//
//

#ifndef __JET_FILTER_HH__
#define __JET_FILTER_HH__



// system include files
#include <memory>
#include <string>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDFilter.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "DataFormats/JetReco/interface/CaloJetCollection.h"

//
// class declaration
//

class JetFilter : public edm::EDFilter {
public:
  explicit JetFilter(const edm::ParameterSet&);
  ~JetFilter();
  
private:
  virtual void beginJob(const edm::EventSetup&) ;
  virtual bool filter(edm::Event&, const edm::EventSetup&);
  virtual void endJob() ;

  
  // ----------parameters ---------------------------

  int minNumJets_;
  int maxNumJets_;

  double minFirstJetEt_;
  double maxFirstJetEt_;
  double minSecondJetEt_;
  double maxSecondJetEt_;
  double minThirdJetEt_;
  double maxThirdJetEt_;
  double minFourthJetEt_;
  double maxFourthJetEt_;
  double minRestJetEt_;
  double maxRestJetEt_;

  std::string caloJetCollName_;
  edm::EDGetTokenT<reco::CaloJetCollection>         tok_CaloJet_;
 };

//
// constants, enums and typedefs
//

//
// static data member definitions
//


#endif

