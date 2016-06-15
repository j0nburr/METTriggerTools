// METTriggerTools includes
#include "GetMETTriggerObjects.h"
#include "xAODMissingET/MissingETAuxContainer.h"


GetMETTriggerObjects::GetMETTriggerObjects( const std::string& name, ISvcLocator* pSvcLocator ) : AthAlgorithm( name, pSvcLocator ){

  //declareProperty( "Property", m_nProperty ); //example property declaration
  declareProperty("IsData", isData = true);
  declareProperty("OutputModifier", m_outMod = "Cal");
  declareProperty("ConfigFiles", m_configFiles);
  declareProperty("FullDebug", m_fullDebug = false);
  //declareProperty("AnaTool", m_anaTool);
}


GetMETTriggerObjects::~GetMETTriggerObjects() {}


StatusCode GetMETTriggerObjects::initialize() {
  ATH_MSG_INFO ("Initializing " << name() << "...");
  ATH_CHECK(m_anaTool.make("METTriggerAnalysisTool/AnaTool") );
  ATH_CHECK(m_anaTool.setProperty("ConfigFiles", m_configFiles ) );
  ATH_CHECK(m_anaTool.setProperty("IsData", isData) );
  ATH_CHECK(m_anaTool.setProperty("OutputLevel", msg().level() ) );
  if (m_fullDebug) ATH_CHECK(m_anaTool.setProperty("SubToolOutputLevel", msg().level() ) );
  ATH_CHECK(m_anaTool.retrieve() );
  return StatusCode::SUCCESS;
}

StatusCode GetMETTriggerObjects::finalize() {
  ATH_MSG_INFO ("Finalizing " << name() << "...");

  return StatusCode::SUCCESS;
}

StatusCode GetMETTriggerObjects::execute() {  
  ATH_MSG_DEBUG ("Executing " << name() << "...");

  if (m_anaTool->getPrimaryVertex() == 0) {
    setFilterPassed(false);
    return StatusCode::SUCCESS;
  }

  xAOD::ElectronContainer* electrons;
  xAOD::ShallowAuxContainer* electronsAux;
  ATH_CHECK( m_anaTool->getElectrons(electrons, electronsAux, false) );
  ATH_CHECK( evtStore()->record(electrons, m_outMod + "Electrons") );
  ATH_CHECK( evtStore()->record(electronsAux, m_outMod + "ElectronsAux") );

  //int nEle = 0;
  //int nEle_ID = 0;
  //int nEle_iso = 0;
  //int nEle_WZCommon = 0;
  //for (const auto& ele : *electrons) {
    //++nEle;
    //if (ele->auxdata<char>("ID:MediumLH") ) ++nEle_ID;
    //if (ele->auxdata<char>("iso:Gradient") ) ++nEle_iso;
    //if (ele->auxdata<char>("WZCommon") ) ++ nEle_WZCommon;
  //}
  //ATH_MSG_INFO("nEle: " << nEle);
  //ATH_MSG_INFO("nEle_ID: " << nEle_ID);
  //ATH_MSG_INFO("nEle_iso: " << nEle_iso);
  //ATH_MSG_INFO("nEle_WZCommon: " << nEle_WZCommon);

  xAOD::MuonContainer* muons;
  xAOD::ShallowAuxContainer* muonsAux;
  ATH_CHECK( m_anaTool->getMuons(muons, muonsAux, false) );
  ATH_CHECK( evtStore()->record(muons, m_outMod + "Muons") );
  ATH_CHECK( evtStore()->record(muonsAux, m_outMod + "MuonsAux") );

  //int nMu = 0;
  //int nMu_ID = 0;
  //int nMu_iso = 0;
  //int nMu_WZCommon = 0;
  //for (const auto& mu : *muons) {
    //++nMu;
    //if (mu->auxdata<char>("ID:Medium") ) ++nMu_ID;
    //if (mu->auxdata<char>("iso:Gradient") ) ++nMu_iso;
    //if (mu->auxdata<char>("WZCommon") ) ++ nMu_WZCommon;
  //}
  //ATH_MSG_INFO("nMu: " << nMu);
  //ATH_MSG_INFO("nMu_ID: " << nMu_ID);
  //ATH_MSG_INFO("nMu_iso: " << nMu_iso);
  //ATH_MSG_INFO("nMu_WZCommon: " << nMu_WZCommon);

  xAOD::JetContainer* jets;
  xAOD::ShallowAuxContainer* jetsAux;
  ATH_CHECK( m_anaTool->getJets(jets, jetsAux, false) );
  ATH_CHECK( evtStore()->record(jets, m_outMod + "Jets") );
  ATH_CHECK( evtStore()->record(jetsAux, m_outMod + "JetsAux") );
  
  xAOD::PhotonContainer* photons;
  xAOD::ShallowAuxContainer* photonsAux;
  ATH_CHECK( m_anaTool->getPhotons(photons, photonsAux, false) );
  ATH_CHECK( evtStore()->record(photons, m_outMod + "Photons") );
  ATH_CHECK( evtStore()->record(photonsAux, m_outMod + "PhotonsAux") );

  xAOD::TauJetContainer* taus;
  xAOD::ShallowAuxContainer* tausAux;
  ATH_CHECK( m_anaTool->getTaus(taus, tausAux, false) );
  ATH_CHECK( evtStore()->record(taus, m_outMod + "Taus") );
  ATH_CHECK( evtStore()->record(tausAux, m_outMod + "TausAux") );

  ATH_MSG_DEBUG("About to remove overlaps");
  ATH_CHECK( m_anaTool->removeOverlaps(electrons, muons, jets, photons, taus) );

  xAOD::MissingETContainer* met = new xAOD::MissingETContainer();
  xAOD::MissingETAuxContainer* metAux = new xAOD::MissingETAuxContainer();
  met->setStore(metAux);
  ATH_CHECK( m_anaTool->getMET(*met, jets, electrons, muons, photons, taus) );
  ATH_CHECK( evtStore()->record(met, m_outMod + "EtMiss") );
  ATH_CHECK( evtStore()->record(metAux, m_outMod + "EtMissAux") );

  xAOD::MissingETContainer* metNoMu = new xAOD::MissingETContainer();
  xAOD::MissingETAuxContainer* metNoMuAux = new xAOD::MissingETAuxContainer();
  metNoMu->setStore(metNoMuAux);
  ATH_CHECK( m_anaTool->getMET(*metNoMu, jets, electrons, 0, photons, taus) );
  ATH_CHECK( evtStore()->record(metNoMu, m_outMod + "EtMissNoMu") );
  ATH_CHECK( evtStore()->record(metNoMuAux, m_outMod + "EtMissNoMuAux") );

  return StatusCode::SUCCESS;
}


