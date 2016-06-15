#include "METTriggerTools/METTriggerAnalysisTool.h"

#include "PathResolver/PathResolver.h"

//Tool interface includes
#include "JetCalibTools/IJetCalibrationTool.h"
#include "JetInterface/IJetSelector.h"
#include "JetInterface/IJetUpdateJvt.h"

#include "MuonSelectorTools/IMuonSelectionTool.h"
#include "MuonMomentumCorrections/IMuonCalibrationAndSmearingTool.h"
#include "ElectronPhotonSelectorTools/IAsgElectronLikelihoodTool.h"
#include "ElectronPhotonSelectorTools/IAsgPhotonIsEMSelector.h"
#include "ElectronPhotonFourMomentumCorrection/IEgammaCalibrationAndSmearingTool.h"
#include "TauAnalysisTools/ITauSelectionTool.h"
#include "IsolationSelection/IIsolationSelectionTool.h"

#include "METInterface/IMETMaker.h"

#include "TrigConfInterfaces/ITrigConfigTool.h"

#include "AssociationUtils/IOverlapTool.h"
#include "AssociationUtils/IOverlapRemovalTool.h"
#include "AssociationUtils/OverlapRemovalInit.h"

#include "IsolationSelection/IsolationSelectionTool.h"
#include "TrigDecisionInterface/ITrigDecisionTool.h"

StatusCode METTriggerAnalysisTool::toolsInit()
{
  if (m_toolsInitDone) {
    ATH_MSG_ERROR("Trying to reinitialise tools!");
    return StatusCode::FAILURE;
  }
  
  if (m_subToolOutputLevel == -1) m_subToolOutputLevel = msg().level();
  
  ATH_MSG_DEBUG( "In METTriggerAnalysisTool::toolsInit()");
    
  ATH_MSG_DEBUG( "Initialising JetCalibrationTool" );
  std::string JES_config_file("JES_MC15Prerecommendation_April2015.config");
  std::string calibseq("JetArea_Origin_Residual_EtaJES");
  if (m_jetInputType == xAOD::JetInput::EMTopo) {
    calibseq += "_GSC";
    if (m_isData) calibseq += "_Insitu";
    else if (m_isAtlfast) JES_config_file = "JES_MC15Prerecommendation_AFII_June2015.config";
  }
  else if (m_jetInputType == xAOD::JetInput::EMPFlow) {
    JES_config_file = "JES_MC15Prerecommendation_PFlow_July2015.config";
    calibseq = "JetArea_Residual_EtaJES_GSC";
  }
  std::string thisName(name() );
  if (thisName.find("ToolSvc.") != std::string::npos) thisName = thisName.substr(8);
  ATH_CHECK(m_jetCalibrationTool.make("JetCalibrationTool/"+thisName + "_JetCalibTool_"+m_jetPrefix) );
  ATH_CHECK(m_jetCalibrationTool.setProperty("JetCollection", m_jetPrefix) );
  ATH_CHECK(m_jetCalibrationTool.setProperty("ConfigFile", JES_config_file) );
  ATH_CHECK(m_jetCalibrationTool.setProperty("CalibSequence", calibseq) );
  ATH_CHECK(m_jetCalibrationTool.setProperty("IsData", m_isData) );
  ATH_CHECK(m_jetCalibrationTool.setProperty("OutputLevel", m_subToolOutputLevel) );
  ATH_CHECK(m_jetCalibrationTool.retrieve() );

  ATH_CHECK(m_jetCleaningTool.make("JetCleaningTool/" + thisName + "_JetCleaningTool") );
  ATH_CHECK(m_jetCleaningTool.setProperty("CutLevel", "LooseBad") );
  ATH_CHECK(m_jetCleaningTool.setProperty("OutputLevel", m_subToolOutputLevel) );
  ATH_CHECK(m_jetCleaningTool.retrieve() );

  ATH_CHECK(m_jvtTool.make("JetVertexTaggerTool/"+thisName+"_JetVertexTaggerTool") );
  ATH_CHECK(m_jvtTool.setProperty("JVTFileName", "JetMomentTools/JVTlikelihood_20140805.root") );
  ATH_CHECK(m_jvtTool.setProperty("OutputLevel", m_subToolOutputLevel) );
  ATH_CHECK(m_jvtTool.retrieve() );

  for (const std::string& muSel : m_requestedMuonIDs) {
    int muQual = 0;
    if (muSel == "VeryLoose") muQual = int(xAOD::Muon::VeryLoose);
    else if (muSel == "Loose") muQual = int(xAOD::Muon::Loose);
    else if (muSel == "Medium") muQual = int(xAOD::Muon::Medium);
    else if (muSel == "Tight") muQual = int(xAOD::Muon::Tight);
    else {
      ATH_MSG_ERROR( "Invalid muon working point " << muSel << " provided! Cannot initialise!" );
      return StatusCode::FAILURE;
    }
    auto& theTool = m_muonSelectionTools["ID:"+muSel];
    ATH_CHECK(theTool.make("CP::MuonSelectionTool/"+thisName+"_MuonSelectionTool_"+muSel) );
    ATH_CHECK(theTool.setProperty("MuQuality", muQual) );
    ATH_CHECK(theTool.setProperty("OutputLevel", m_subToolOutputLevel) );
    ATH_CHECK(theTool.retrieve() );
  }

  ATH_CHECK(m_muonCalibrationTool.make("CP::MuonCalibrationAndSmearingTool/"+thisName+"_MuonCalibrationAndSmearingTool") );
  ATH_CHECK(m_muonCalibrationTool.setProperty("OutputLevel", m_subToolOutputLevel) );
  ATH_CHECK(m_muonCalibrationTool.retrieve() );

  if (m_ORDoTaus) m_requestedElectronIDs.insert("LooseLH"); // Tau overlap removal is done with LooseLH electrons. May need a further look TODO
  std::string egammaToolConfigDir = "ElectronPhotonSelectorTools/offline/mc15_20150712/";
  for (const std::string& eleSel : m_requestedElectronIDs) {
    std::string fileName = "ElectronLikelihood"+eleSel.substr(0, eleSel.size()-2)+"OfflineConfig2015.conf";
    if (eleSel == "LooseAndBLayerLH") fileName = "ElectronLikelihoodLooseOfflineConfig2015_CutBL.conf";
    auto& theTool = m_electronSelectionTools["ID:"+eleSel];
    ATH_CHECK(theTool.make("AsgElectronLikelihoodTool/"+thisName+"_AsgElectronLikelihoodTool_"+eleSel) );
    ATH_CHECK(theTool.setProperty("ConfigFile", egammaToolConfigDir+fileName) );
    ATH_CHECK(theTool.setProperty("primaryVertexContainer", "PrimaryVertices") );
    ATH_CHECK(theTool.setProperty("OutputLevel", m_subToolOutputLevel) );
    ATH_CHECK(theTool.retrieve() );
    //m_electronSelectionTools.push_back(std::make_pair("ID:"+eleSel, std::move(theTool) ) );
    // TODO add support for AsgElectronIsEMSelector
  }
  for (const std::string& gammaSel : m_requestedPhotonIDs) {
    unsigned int photonID = 0;
    if (gammaSel == "Tight") photonID = static_cast<unsigned int>(egammaPID::PhotonTight);
    else if (gammaSel == "Medium") photonID = static_cast<unsigned int>(egammaPID::PhotonMedium);
    else if (gammaSel == "Loose") photonID =  static_cast<unsigned int>(egammaPID::PhotonLoose);
    else {
      ATH_MSG_ERROR( "Invalid photon working point: " << gammaSel << " provided! Cannot initialise!" );
      return StatusCode::FAILURE;
    }
    std::string fileName = "PhotonIsEM"+gammaSel+"SelectorCutDefs.conf";
    auto& theTool = m_photonSelectionTools["ID:"+gammaSel];
    ATH_CHECK(theTool.make("AsgPhotonIsEMSelector/"+thisName+"_AsgElectronLikelihoodTool_"+gammaSel) );
    ATH_CHECK(theTool.setProperty("ConfigFile", egammaToolConfigDir+fileName) );
    ATH_CHECK(theTool.setProperty("isEMMask", photonID) );
    ATH_CHECK(theTool.setProperty("OutputLevel", m_subToolOutputLevel) );
    ATH_CHECK(theTool.retrieve() );
  }

  ATH_CHECK(m_egammaCalibrationTool.make("CP::EgammaCalibrationAndSmearingTool/"+thisName+"_EgammaCalibrationAndSmearingTool") );
  ATH_CHECK(m_egammaCalibrationTool.setProperty("ESModel", "es2015PRE") );
  ATH_CHECK(m_egammaCalibrationTool.setProperty("decorrelationModel", "1NP_v1") );
  if (m_isAtlfast) ATH_CHECK(m_egammaCalibrationTool.setProperty("useAFII", true) );
  ATH_CHECK(m_egammaCalibrationTool.setProperty("OutputLevel", m_subToolOutputLevel) );
  ATH_CHECK(m_egammaCalibrationTool.retrieve() );

// TODO This still doesn't apply anything other than default tau selection
  for (const std::string& tauSel : m_requestedTauIDs) {
    auto& theTool = m_tauSelectionTools["ID:"+tauSel];
    ATH_CHECK(theTool.make("TauAnalysisTools::TauSelectionTool/"+thisName+"_TauSelectionTool_"+tauSel) );
    ATH_CHECK(theTool.setProperty("OutputLevel", m_subToolOutputLevel) );
    ATH_CHECK(theTool.retrieve() );
  }
  for (const std::string& eleIso : m_requestedElectronIsos) {
    auto& theTool = m_electronIsolationTools["iso:"+eleIso];
    ATH_MSG_DEBUG("Setting eleIso " << eleIso);
    ATH_CHECK(theTool.make("CP::IsolationSelectionTool/"+thisName+"_EleIsoTool_"+eleIso) );
    ATH_CHECK(theTool.setProperty("ElectronWP", eleIso) );
    ATH_CHECK(theTool.setProperty("OutputLevel", m_subToolOutputLevel) );
    ATH_CHECK(theTool.retrieve() );
  }
  for (const std::string& muIso : m_requestedMuonIsos) {
    auto& theTool = m_muonIsolationTools["iso:"+muIso];
    ATH_MSG_DEBUG("Setting muIso " << muIso);
    ATH_CHECK(theTool.make("CP::IsolationSelectionTool/"+thisName+"_MuIsoTool_"+muIso) );
    ATH_CHECK(theTool.setProperty("MuonWP", muIso) );
    ATH_CHECK(theTool.setProperty("OutputLevel", m_subToolOutputLevel) );
    ATH_CHECK(theTool.retrieve() );
  }
  for (const std::string& gammaIso : m_requestedPhotonIsos) {
    //m_photonIsolationTools.push_back(std::make_pair(SG::AuxElement::Decorator<char>("iso:"+gammaIso),
                                                    //asg::AnaToolHandle<CP::IIsolationSelectionTool>("") ) );
    //auto& theTool = m_photonIsolationTools.back().second;
    auto& theTool = m_photonIsolationTools["iso:"+gammaIso];
    ATH_CHECK(theTool.make("CP::IsolationSelectionTool/"+thisName+"_GammaIsoTool_"+gammaIso) );
    ATH_CHECK(theTool.setProperty("PhotonWP", gammaIso) );
    ATH_CHECK(theTool.setProperty("OutputLevel", m_subToolOutputLevel) );
    ATH_CHECK(theTool.retrieve() );
  }

  ATH_CHECK(m_metMaker.make("met::METMaker/"+thisName+"_METMaker") );
  if (m_jetInputType == xAOD::JetInput::EMPFlow) {
    ATH_CHECK(m_metMaker.setProperty("DoPFlow", true) );
    ATH_CHECK(m_metMaker.setProperty("JetJvtCut", -1.) );
  }
  ATH_CHECK(m_metMaker.setProperty("OutputLevel", m_subToolOutputLevel) );
  ATH_CHECK(m_metMaker.retrieve() );

  if (m_trigDec.empty() ) {
    ATH_CHECK(m_trigConf.make("TrigConf::xAODConfigTool/xAODConfigTool") );
    ATH_CHECK(m_trigConf.setProperty("OutputLevel", m_subToolOutputLevel) );
    ATH_CHECK(m_trigConf.retrieve() );

    ATH_CHECK(m_trigDec.make("Trig::TrigDecisionTool/TrigDecisionTool") );
    ATH_CHECK(m_trigDec.setProperty("ConfigTool", m_trigConf.getHandle() ) );
    ATH_CHECK(m_trigDec.setProperty("TrigDecisionKey", "xTrigDecision") );
    ATH_CHECK(m_trigDec.setProperty("OutputLevel", m_subToolOutputLevel) );
    ATH_CHECK(m_trigDec.retrieve() );
  }
  for (const auto& sel : m_objSelectors) {
    ATH_MSG_INFO( "Initialising ORTools for " << sel.name );
//#ifdef XAOD_STANDALONE //RootCore
    ORUtils::ORFlags orFlags(thisName + "_" + sel.name + "_ORTool", sel.name, sel.name+":"+m_orOutput);
    orFlags.outputPassValue = true;
    orFlags.doPhotons = m_ORDoPhotons;
    orFlags.doTaus = m_ORDoTaus;
    ORUtils::ToolBox toolBox;
    ATH_CHECK( ORUtils::recommendedTools(orFlags, toolBox) );
    ATH_CHECK( toolBox.tauEleORT.setProperty("ElectronID", "ID:LooseLH") );
    ATH_CHECK( toolBox.setGlobalProperty("OutputLevel", m_subToolOutputLevel) );
    ATH_CHECK( toolBox.initialize() );
    //ATH_CHECK( toolBox.masterTool.retrieve() );
    //m_orTools.emplace(sel.name, std::move(toolBox.masterTool) );
    auto& theTool = m_orTools[sel.name];
    theTool.handle().setTypeAndName("ORUtils::OverlapRemovalTool/"+toolBox.masterTool.name() );
    if (!theTool.isUserConfigured() ) {
      ATH_MSG_ERROR( "Tool " << theTool.name() << " should already be configured!" );
      ATH_MSG_ERROR( "pointer name:" << toolBox.masterTool->name() );
      ATH_MSG_ERROR( "handle name: " << toolBox.masterTool.name() );
      return StatusCode::FAILURE;
    }
    ATH_CHECK( theTool.retrieve() );

    //ATH_CHECK( toolBox.masterTool.retrieve() );
    //auto& theTool = toolBox.masterTool;
    //ATH_CHECK( theTool.retrieve() );
    //ATH_MSG_INFO( "About to emplace" );
    //m_orTools.push_back(std::move(theTool) );
    //m_orTools.emplace_back(theTool.name() );
    //ATH_MSG_INFO( "About to retrieve again" );
    //ATH_CHECK( m_orTools.back().retrieve() );
    //ATH_MSG_INFO( "Done");
//#else // Athena
#if 0
    std::vector<std::string> subTools;
    //m_orTools.emplace(sel.name, asg::AnaToolHandle<IOverlapRemovalTool>("ORUtils::OverlapRemovalTool/"+name() + "_" + sel.name, this) );
    auto& theTool = m_orTools[sel.name];
    ATH_CHECK( theTool.make("ORUtils::OverlapRemovalTool/"+name() + "_" + sel.name) );
    ATH_CHECK( theTool.setProperty("EleJetORT", "ORUtils::EleJetOverlapTool/EleJetORT") );
    subTools.push_back( "EleJetORT" );
    ATH_CHECK( theTool.setProperty("EleMuORT", "ORUtils::EleMuSharedTrkOverlapTool/EleMuORT") );    
    subTools.push_back( "EleMuORT" );
    ATH_CHECK( theTool.setProperty("MuJetORT", "ORUtils::MuJetOverlapTool/MuJetORT") );
    subTools.push_back( "MuJetORT" );

    if (m_ORDoPhotons) {
      ATH_CHECK( theTool.setProperty("PhoEleORT", "ORUtils::DeltaROverlapTool/PhoEleORT") );
      subTools.push_back( "PhoEleORT" );
      ATH_CHECK( theTool.setProperty("PhoMuORT", "ORUtils::DeltaROverlapTool/PhoMuORT") );
      subTools.push_back( "PhoMuORT" );
      ATH_CHECK( theTool.setProperty("PhoJetORT", "ORUtils::DeltaROverlapTool/PhoJetORT") );
      subTools.push_back( "PhoJetORT" );
    }

    if (m_ORDoTaus) {
      ATH_CHECK( theTool.setProperty("TauEleORT", "ORUtils::TauLooseEleOverlapTool/TauEleORT") );
      ATH_CHECK( theTool.setProperty("TauEleORT.ElectronID", "ID:LooseLH") );
      subTools.push_back( "TauEleORT" );
      ATH_CHECK( theTool.setProperty("TauMuORT", "ORUtils::TauLooseMuOverlapTool/TauMuORT") );
      subTools.push_back( "TauMuORT" );
      ATH_CHECK( theTool.setProperty("TauJetORT", "ORUtils::DeltaROverlapTool/TauJetORT") );
      subTools.push_back( "TauJetORT");
    }

    ATH_CHECK( theTool.setProperty("InputLabel", sel.name) );
    ATH_CHECK( theTool.setProperty("OutputLabel", sel.name + ":" + m_orOutput) );
    ATH_CHECK( theTool.setProperty("OutputPassValue", true) );
    for (const auto& subToolName : subTools) {
      ATH_CHECK( theTool.setProperty(subToolName + ".InputLabel", sel.name) );
      ATH_CHECK( theTool.setProperty(subToolName + ".OutputLabel", sel.name + ":" + m_orOutput) );
      ATH_CHECK( theTool.setProperty(subToolName + ".OutputPassValue", true) );
    }
    ATH_CHECK( theTool.retrieve() );

#endif
  }

  m_metObjectSelector.initialize();
  for (auto sel : m_objSelectors) sel.initialize();

  m_toolsInitDone = true;
  ATH_MSG_INFO( "All subtools initiailised!" );
  return StatusCode::SUCCESS;
}
