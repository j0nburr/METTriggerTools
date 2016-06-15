// METTriggerTools includes
#include "HistDrawer.h"
#include "xAODMissingET/MissingETContainer.h"
#include "xAODTrigMissingET/TrigMissingETContainer.h"

HistDrawer::HistDrawer( const std::string& name, ISvcLocator* pSvcLocator )
  : AthAnalysisAlgorithm( name, pSvcLocator ),
  m_helper("METTriggerHelper/METTriggerHelper")
{

  declareProperty( "StoreGatePrefix", m_sgPrefix = "Cal" );
  declareProperty( "METTerm", m_metTerm = "FinalTrk" );

}


HistDrawer::~HistDrawer() {}


StatusCode HistDrawer::initialize() {
  ATH_MSG_INFO ("Initializing " << name() << "...");
  ATH_CHECK( m_helper.retrieve() );
  //
  m_metTypes.push_back(HLTMET::Cell);
  m_metTypes.push_back(HLTMET::Topocl);
  m_metTypes.push_back(HLTMET::Topocl_PS);
  m_metTypes.push_back(HLTMET::Topocl_PUC);
  m_metTypes.push_back(HLTMET::mht);
  for (const auto& type : m_metTypes) {
    std::string trigMetName = m_helper->getHLTMETName(type);
    ATH_CHECK(book(TH2F( (trigMetName + "_v_off").c_str(), (";E_{T}^{miss} (offline, no muons) [GeV];E_{T}^{miss} ("+trigMetName+") [GeV]").c_str(), 400, 0, 400, 400, 0, 400) ) );
  }
  ATH_CHECK(book(TH2F("L1_v_off", ";E_{T}^{miss} (offline, no muons) [GeV]; E_{T}^{miss} (L1) [GeV]", 400, 0, 400, 400, 0, 400) ) );


  return StatusCode::SUCCESS;
}

StatusCode HistDrawer::finalize() {
  ATH_MSG_INFO ("Finalizing " << name() << "...");
  //
  //Things that happen once at the end of the event loop go here
  //


  return StatusCode::SUCCESS;
}

StatusCode HistDrawer::execute() {  
  ATH_MSG_DEBUG ("Executing " << name() << "...");

  const xAOD::MissingETContainer* metCont;
  ATH_CHECK( evtStore()->retrieve(metCont, m_sgPrefix+"EtMissNoMu") );
  auto metItr = metCont->find(m_metTerm);
  if (metItr == metCont->end() ) {
    ATH_MSG_ERROR( "Term " << m_metTerm << " not present in container " << m_sgPrefix << "EtMissNoMu" );
    return StatusCode::FAILURE;
  }
  float met = (*metItr)->met() * 0.001;
  float trigMet;
  for (const auto& type : m_metTypes) {
    const xAOD::TrigMissingETContainer* trigMetCont;
    ATH_CHECK( evtStore()->retrieve(trigMetCont, "HLT_xAOD__TrigMissingETContainer_TrigEFMissingET") );
    ATH_MSG_DEBUG( "Getting trigger MET for " << m_helper->getHLTMETName(type) );
    ATH_CHECK( m_helper->getMET(type, trigMet) );
    trigMet *= 0.001;
    hist(m_helper->getHLTMETName(type) + "_v_off")->Fill(met, trigMet);
  }
  ATH_CHECK( m_helper->getMET(HLTMET::L1, trigMet) );
  hist("L1_v_off")->Fill(met, trigMet);


  return StatusCode::SUCCESS;
}

StatusCode HistDrawer::beginInputFile() { 
  return StatusCode::SUCCESS;
}


