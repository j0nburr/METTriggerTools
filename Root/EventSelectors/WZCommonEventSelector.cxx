#include "METTriggerTools/EventSelectors/WZCommonEventSelector.h"
#include <fstream>
#include "PathResolver/PathResolver.h"

WZCommonEventSelector::WZCommonEventSelector(const std::string& name)
  : METTriggerEventSelector(name),
    cacc_sel(""),
    m_selElectrons(SG::VIEW_ELEMENTS),
    m_selMuons(SG::VIEW_ELEMENTS)
{
  declareProperty( "METTerm", m_metTerm = "FinalTrk" );
}

StatusCode WZCommonEventSelector::initialize()
{
  ATH_CHECK( METTriggerEventSelector::initialize() );
  ATH_CHECK( readConfigFile() );
  cacc_sel = SG::AuxElement::ConstAccessor<char>(m_doPassOR ? m_decorator+":passOR" : m_decorator);
  return StatusCode::SUCCESS;
}


StatusCode WZCommonEventSelector::readConfigFile()
{
  m_configFile = PathResolver::find_file(m_configFile, "DATAPATH");

  std::ifstream confstream(m_configFile);
  JSONParser parser(confstream, msg().level() < MSG::INFO);
  JSONNode& rootNode = parser.rootNode();

  // Make sure that this is the right sort of config file!
  std::string type;
  if (!rootNode.get("type", type) ) {
    ATH_MSG_ERROR( "No 'type' node in config file " << m_configFile );
    return StatusCode::FAILURE;
  }
  if (type != "WZCommon") {
    ATH_MSG_ERROR( "Type of config file is " << type << ". It should be 'WZCommon'" );
    return StatusCode::FAILURE; // Might not really need this anymore? TODO
  }



  JSONNode node;
  // Find the decorator
  if (!rootNode.get("objDefs", node) ) {
    ATH_MSG_ERROR( "No 'objDefs' node in config file " << m_configFile );
    return StatusCode::FAILURE;
  }
  if (node.vectorSubNodes.size() != 1) {
    ATH_MSG_ERROR( node.vectorSubNodes.size() << " object definitions provided in config file " << m_configFile << ". WZCommon only expects 1" );
    return StatusCode::FAILURE;
  }
  if (!node.vectorSubNodes.front().get("decorator", m_decorator) ) {
    ATH_MSG_ERROR( "No decorator provided for 'objDef' node in config file " << m_configFile );
    return StatusCode::FAILURE;
  }

  // Wenu
  if (rootNode.get("Wenu", node) ) {
    node.get("mT", m_Wenu_mT);
    node.get("met", m_Wenu_met);
    node.get("triggers", m_Wenu_triggers);
    ATH_CHECK( checkProperties(node, "Wenu selection") );
  }
  // Wmunu
  if (rootNode.get("Wmunu", node) ) {
    node.get("mT", m_Wmunu_mT);
    node.get("met", m_Wmunu_met);
    node.get("triggers", m_Wmunu_triggers);
    ATH_CHECK( checkProperties(node, "Wmunu selection") );
  }
  // Zee
  if (rootNode.get("Zee", node) ) {
    node.get("mllLow", m_Zee_mllLow);
    node.get("mllHigh", m_Zee_mllHigh);
    node.get("triggers", m_Zee_triggers);
    ATH_CHECK( checkProperties(node, "Zee selection") );
  }
  // Zmumu
  if (rootNode.get("Zmumu", node) ) {
    node.get("mllLow", m_Zmumu_mllLow);
    node.get("mllHigh", m_Zmumu_mllHigh);
    node.get("triggers", m_Zmumu_triggers);
    ATH_CHECK( checkProperties(node, "Zmumu selection") );
  }

  ATH_CHECK( checkProperties(rootNode, "WZCommon") );

  return StatusCode::SUCCESS;
}

StatusCode WZCommonEventSelector::retrieveObjects()
{
  ATH_MSG_DEBUG( "In WZCommonEventSelector::retrieveObjects" );

  const xAOD::ElectronContainer* electrons;
  ATH_CHECK( evtStore()->retrieve(electrons, m_sgPrefix + "Electrons") );
  m_selElectrons = m_getSelected(electrons, cacc_sel);
  ATH_MSG_DEBUG( electrons->size() << " electrons, of which " << m_selElectrons.size() << " selected" );

  const xAOD::MuonContainer* muons;
  ATH_CHECK( evtStore()->retrieve(muons, m_sgPrefix + "Muons") );
  m_selMuons = m_getSelected(muons, cacc_sel);
  ATH_MSG_DEBUG( muons->size() << " muons, of which " << m_selMuons.size() << " selected" );

  ATH_CHECK( evtStore()->retrieve(m_met, m_sgPrefix + "EtMiss") );

  if (m_met->find(m_metTerm) == m_met->end() ) {
    ATH_MSG_ERROR( "No " << m_metTerm << " term in container " << m_sgPrefix + "EtMiss"); return StatusCode::FAILURE;
  }
  
  return StatusCode::SUCCESS;
}

Root::TAccept WZCommonEventSelector::passSelection(const std::string& name) const
{
  if (name == "Wenu") return passWenu();
  if (name == "Wmunu") return passWmunu();
  if (name == "Zmumu") return passZmumu();
  if (name == "Zee") return passZee();
  ATH_MSG_WARNING( "WARNING. Unrecognised selection " << name << " requested from WZCommonEventSelector. Return blank accept" );
  return Root::TAccept("blank");
}

Root::TAccept WZCommonEventSelector::passWenu() const
{
  ATH_MSG_DEBUG( "In WZCommonEventSelector::passWenu" );
  //Root::TAccept accept = eventVetoes();
  Root::TAccept accept("Wenu");
  accept.addCut("PrimaryVertexCut", "Passes a primary vertex cut");
  accept.addCut("BadJetCut", "Passes jet cleaning cut");
  accept.addCut("Triggers", "pass triggers");
  accept.addCut("OneElectron", "Exactly 1 electron, 0 muons");
  accept.addCut("mT", "mT cut");
  accept.addCut("met", "met cut");
  if (!m_passPrimaryVertexCut() ) return accept;
  accept.setCutResult("PrimaryVertexCut", true);
  if (!m_passJetCleaningCut() ) return accept;
  accept.setCutResult("BadJetCut", true);
  if (!m_passTriggers(m_Wenu_triggers) ) return accept;
  accept.setCutResult("Triggers", true);
  if (m_selElectrons.size() != 1 || m_selMuons.size() != 0) return accept;
  accept.setCutResult("OneElectron", true);
  if (m_mT(m_selElectrons.at(0), (*m_met)[m_metTerm]) < m_Wenu_mT) return accept;
  accept.setCutResult("mT", true);
  if ( (*m_met)[m_metTerm]->met() * GeV < m_Wenu_met) return accept;
  accept.setCutResult("met", true);
  return accept;
}

Root::TAccept WZCommonEventSelector::passWmunu() const
{
  ATH_MSG_DEBUG( "In WZCommonEventSelector::passWmunu" );
  //Root::TAccept accept = eventVetoes();
  Root::TAccept accept("Wmunu");
  accept.addCut("PrimaryVertexCut", "Passes a primary vertex cut");
  accept.addCut("BadJetCut", "Passes jet cleaning cut");
  accept.addCut("Triggers", "pass triggers");
  accept.addCut("OneMuon", "Exactly 1 muon, 0 electrons");
  accept.addCut("mT", "mT cut");
  accept.addCut("met", "met cut");
  if (!m_passPrimaryVertexCut() ) return accept;
  accept.setCutResult("PrimaryVertexCut", true);
  if (!m_passJetCleaningCut() ) return accept;
  accept.setCutResult("BadJetCut", true);
  if (!m_passTriggers(m_Wmunu_triggers) ) return accept;
  accept.setCutResult("Triggers", true);
  if (m_selMuons.size() != 1 || m_selElectrons.size() != 0) return accept;
  accept.setCutResult("OneMuon", true);
  if (m_mT(m_selMuons.at(0), (*m_met)[m_metTerm]) < m_Wenu_mT) return accept;
  accept.setCutResult("mT", true);
  if ( (*m_met)[m_metTerm]->met() * GeV < m_Wenu_met) return accept;
  accept.setCutResult("met", true);
  return accept;
}

Root::TAccept WZCommonEventSelector::passZee() const
{
  ATH_MSG_DEBUG( "In WZCommonEventSelector::passZee" );
  //Root::TAccept accept = eventVetoes();
  Root::TAccept  accept("Zee");
  accept.addCut("PrimaryVertexCut", "Passes a primary vertex cut");
  accept.addCut("BadJetCut", "Passes jet cleaning cut");
  accept.addCut("Triggers", "pass triggers");
  accept.addCut("TwoElectrons", "Exactly 2 electrons of opposite signs, 0 muons");
  accept.addCut("mll", "mll cut");
  if (!m_passPrimaryVertexCut() ) return accept;
  accept.setCutResult("PrimaryVertexCut", true);
  if (!m_passJetCleaningCut() ) return accept;
  accept.setCutResult("BadJetCut", true);
  if (!m_passTriggers(m_Zee_triggers) ) return accept;
  accept.setCutResult("Triggers", true);
  if (m_selElectrons.size() != 2 || m_selMuons.size() != 0 ||
      (m_selElectrons.at(0)->charge() * m_selElectrons.at(1)->charge() ) > 0 ) return accept;
  accept.setCutResult("TwoElectrons", true);
  float mll = m_invMass(m_selElectrons.at(0), m_selElectrons.at(1) );
  if (mll > m_Zee_mllHigh || mll < m_Zee_mllLow) return accept;
  accept.setCutResult("mll", true);
  return accept;
}

Root::TAccept WZCommonEventSelector::passZmumu() const
{
  ATH_MSG_DEBUG( "In WZCommonEventSelector::passZmumu" );
  //Root::TAccept accept = eventVetoes();
  Root::TAccept accept("Zmumu");
  accept.addCut("PrimaryVertexCut", "Passes a primary vertex cut");
  accept.addCut("BadJetCut", "Passes jet cleaning cut");
  accept.addCut("Triggers", "pass triggers");
  accept.addCut("TwoMuons", "Exactly 2 muons of opposite signs, 0 electrons");
  accept.addCut("mll", "mll cut");
  if (!m_passPrimaryVertexCut() ) return accept;
  accept.setCutResult("PrimaryVertexCut", true);
  if (!m_passJetCleaningCut() ) return accept;
  accept.setCutResult("BadJetCut", true);
  if (!m_passTriggers(m_Zmumu_triggers) ) return accept;
  accept.setCutResult("Triggers", true);
  if (m_selMuons.size() != 2 || m_selElectrons.size() != 0 ||
      (m_selMuons.at(0)->charge() * m_selMuons.at(1)->charge() ) > 0 ) return accept;
  accept.setCutResult("TwoMuons", true);
  float mll = m_invMass(m_selMuons.at(0), m_selMuons.at(1) );
  if (mll > m_Zmumu_mllHigh || mll < m_Zmumu_mllLow) return accept;
  accept.setCutResult("mll", true);
  return accept;
}
