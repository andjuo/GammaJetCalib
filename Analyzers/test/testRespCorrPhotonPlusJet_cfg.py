import FWCore.ParameterSet.Config as cms
process = cms.Process('ANALYSIS')

process.load('FWCore.MessageService.MessageLogger_cfi')
process.load('Configuration.StandardSequences.Services_cff')
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')

from Configuration.AlCa.autoCond import autoCond
process.load("Configuration.Geometry.GeometryIdeal_cff")
process.GlobalTag.globaltag=autoCond['startup']

# Specify IdealMagneticField ESSource (needed for CMSSW 730)
process.load("MagneticField.Engine.autoMagneticFieldProducer_cfi")

#load the response corrections calculator
process.load('HcalClosureTest.Analyzers.calcrespcorrphotonplusjet_cfi')
#  needed for nonCHS
process.load('JetMETCorrections.Configuration.JetCorrectionProducers_cff')
# needed for the correctors
process.load('JetMETCorrections.Configuration.JetCorrectionServicesAllAlgos_cff')

# run over files
process.calcrespcorrphotonplusjet.rootHistFilename = cms.string('PhoJet_tree_CHS.root')
process.calcrespcorrphotonplusjet.doCaloJets = cms.bool(False)
process.calcrespcorrphotonplusjet.doPFJets = cms.bool(True)
process.calcrespcorrphotonplusjet.doGenJets = cms.bool(True)
# trigger names should not end with '_'
process.calcrespcorrphotonplusjet.photonTriggers = cms.vstring(
    'HLT_Photon20_CaloIdVL_IsoL','HLT_Photon30_CaloIdVL_IsoL',
    'HLT_Photon50_CaloIdVL_IsoL','HLT_Photon75_CaloIdVL_IsoL',
    'HLT_Photon90_CaloIdVL_IsoL','HLT_Photon135',
    'HLT_Photon150','HLT_Photon160')
# triggers for CMSSW 730
#process.calcrespcorrphotonplusjet.photonTriggers += cms.vstring(
#    'HLT_Photon22', 'HLT_Photon30', 'HLT_Photon36',
#    'HLT_Photon50', 'HLT_Photon75',
#    'HLT_Photon90', 'HLT_Photon120', 'HLT_Photon175',
#    'HLT_Photon250_NoHE', 'HLT_Photon300_NoHE'
#)
# to disable photonTriggers assign an empty vstring
process.calcrespcorrphotonplusjet.photonTriggers = cms.vstring()

# a clone without CHS
process.calcrespcorrphotonplusjet_noCHS= process.calcrespcorrphotonplusjet.clone()
process.calcrespcorrphotonplusjet_noCHS.rootHistFilename = cms.string('PhoJet_tree_nonCHS.root')
# for 7XY use ak4* instead of ak5
process.calcrespcorrphotonplusjet_noCHS.pfJetCollName = cms.string('ak5PFJets')
process.calcrespcorrphotonplusjet_noCHS.pfJetCorrName = cms.string('ak5PFL2L3')
#process.calcrespcorrphotonplusjet_noCHS.pfJetCorrName = cms.string('ak4PFL2L3')

# Load file list
# Summer12_DR53X production G_Pt_XtoY
import FWCore.Utilities.FileUtils as FileUtils
#listFileName='fileinfo_GJet/makepy_Summer12_DR53X_G_Pt_170to300.txt'
#listFileName='selection_tmp.txt'
listFileName='fileInfo_RelVal_5_3_14_PhotonJets.txt'
#listFileName='fileInfo_RelVal_7_3_0_PhotonJets.txt'
mylist = FileUtils.loadListFromFile(listFileName)
# to add additional files from another list, define that list and
# uncomment the next line
#mylist.extend( FileUtils.loadListFromFile(listFileName2) )
readFiles = cms.untracked.vstring( *mylist )

process.source = cms.Source("PoolSource", 
#fileNames = cms.untracked.vstring('file:/uscms/home/lovedeep/eos/RelValPhotonJets_Pt_10_CMSSW_5_3_12_patch2_A4609359-9E2B-E311-B331-0025905964A6.root')
                            fileNames= readFiles

#fileNames = cms.untracked.vstring(
#    '/store/mc/Summer12_DR53X/G_Pt-170to300_TuneZ2star_8TeV_pythia6/GEN-SIM-RECO/PU_S10_START53_V7A-v1/00000/5846302F-1A18-E211-A060-00266CF2AE10.root',
##    '/store/mc/Summer12_DR53X/G_Pt-170to300_TuneZ2star_8TeV_pythia6/GEN-SIM-RECO/PU_S10_START53_V7A-v1/00000/586126E2-0F18-E211-9323-0030487D864B.root',
##    '/store/mc/Summer12_DR53X/G_Pt-170to300_TuneZ2star_8TeV_pythia6/GEN-SIM-RECO/PU_S10_START53_V7A-v1/00000/A80FB82E-1018-E211-B444-0025904B130E.root',
##    '/store/mc/Summer12_DR53X/G_Pt-170to300_TuneZ2star_8TeV_pythia6/GEN-SIM-RECO/PU_S10_START53_V7A-v1/00000/5809400A-F917-E211-8D3D-0030487F1C51.root',
##    '/store/mc/Summer12_DR53X/G_Pt-170to300_TuneZ2star_8TeV_pythia6/GEN-SIM-RECO/PU_S10_START53_V7A-v1/00000/A40C5492-F917-E211-AB13-002481E0DC82.root'
#    )

)

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(5000) )
process.MessageLogger.cerr.FwkReport.reportEvery=cms.untracked.int32(1000)
process.options = cms.untracked.PSet( wantSummary = cms.untracked.bool(False) )

# timing
#process.Timing = cms.Service('Timing')

# Load pfNoPileUP
process.load("CommonTools.ParticleFlow.pfNoPileUp_cff")
process.load("CommonTools.ParticleFlow.PF2PAT_cff")

process.p = cms.Path(
process.ak4PFJets+  # needed for pfJetTracksAssociatorAtVertex
process.PF2PAT
+(process.ak5PFJetsCHS
+process.calcrespcorrphotonplusjet)
#+process.calcrespcorrphotonplusjet_noCHS
)
