#include "METTriggerTools/EventSelectors/ObjectCounter.h"
#include <fstream>
#include "PathResolver/PathResolver.h"

ObjectCounter::ObjectCounter(const std::string& name)
  : METTriggerEventSelector(name),
    cacc_sel(""),
    m_selElectrons(SG::VIEW_ELEMENTS),
    m_selMuons(SG::VIEW_ELEMENTS),
    m_selJets(SG::VIEW_ELEMENTS),
    m_selPhotons(SG::VIEW_ELEMENTS),
    m_selTaus(SG::VIEW_ELEMENTS),
    m_definesElectrons(false),
    m_definesMuons(false),
    m_definesJets(false),
    m_definesPhotons(false),
    m_definesTaus(false)
{
  declareProperty( "METTerm", m_metTerm = "FinalTrk" );
}

StatusCode ObjectCounter::initialize()
{
  ATH_CHECK( METTriggerEventSelector::initialize() );
  ATH_CHECK( readConfigFile() );
  cacc_sel = SG::AuxElement::ConstAccessor<char>(m_doPassOR ? m_decorator+":passOR" : m_decorator);
  return StatusCode::SUCCESS;
}

StatusCode ObjectCounter::makeBlankSignalTuple(const std::string& signalName) {
  if (m_signals.find(signalName) != m_signals.end() ) {
    ATH_MSG_ERROR( "Attempting to make a signal called '" << signalName << "'. This already exists in tool " << name() );
    return StatusCode::FAILURE;
  }
  SignalTuple& tup = m_signals[signalName];
  std::get<0>(tup) = ObjectPair(0, std::greater_equal<int>() );
  std::get<1>(tup) = ObjectPair(0, std::greater_equal<int>() );
  std::get<2>(tup) = ObjectPair(0, std::greater_equal<int>() );
  std::get<3>(tup) = ObjectPair(0, std::greater_equal<int>() );
  std::get<4>(tup) = ObjectPair(0, std::greater_equal<int>() );
  std::get<5>(tup) = MetPair(0, std::greater_equal<float>() );
  return StatusCode::SUCCESS;
}

template <typename T>
bool parseOperator(const std::string& opString, std::function<bool(const T&, const T&)>& opFunc) {
  if (opString == "=") opFunc = std::equal_to<T>();
  else if (opString == "!=") opFunc = std::not_equal_to<T>();
  else if (opString == ">") opFunc = std::greater<T>();
  else if (opString == "<") opFunc = std::less<T>();
  else if (opString == ">=") opFunc = std::greater_equal<T>();
  else if (opString == "<=") opFunc = std::less_equal<T>();
  else return false;
  return true;
}

StatusCode ObjectCounter::makeObjectCut(JSONNode& node, ObjectPair& objPair) const
{
  int value;
  if (!node.get("value", value) ) {
    ATH_MSG_ERROR( "No 'value' node in node " << node.name << " in config file " << m_configFile );
    return StatusCode::FAILURE;
  }

  std::string opString;
  if (!node.get("operator", opString) ) {
    ATH_MSG_ERROR( "No 'operator' node in node " << node.name << " in config file " << m_configFile );
    return StatusCode::FAILURE;
  }

  if (!parseOperator(opString, objPair.second) ) {
    ATH_MSG_ERROR( "Invalid operator '" << opString << "' provided in node " << node.name );
    return StatusCode::FAILURE;
  }
  objPair.first = value;
  return StatusCode::SUCCESS;
}

StatusCode ObjectCounter::makeMetCut(JSONNode& node, MetPair& metPair) const
{
  float value;
  if (!node.get("value", value) ) {
    ATH_MSG_ERROR( "No 'value' node in node " << node.name << " in config file " << m_configFile );
    return StatusCode::FAILURE;
  }

  std::string opString;
  if (!node.get("operator", opString) ) {
    ATH_MSG_ERROR( "No 'operator' node in node " << node.name << " in config file " << m_configFile );
    return StatusCode::FAILURE;
  }

  if (!parseOperator(opString, metPair.second) ) {
    ATH_MSG_ERROR( "Invalid operator '" << opString << "' provided in node " << node.name );
    return StatusCode::FAILURE;
  }
  metPair.first = value;
  return StatusCode::SUCCESS;
}


StatusCode ObjectCounter::readConfigFile()
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
  if (type != "ObjectCounter") {
    ATH_MSG_ERROR( "Type of config file is " << type << ". It should be 'ObjectCounter'" );
    return StatusCode::FAILURE; // Might not really need this anymore? TODO
  }



  JSONNode node;
  // Find the decorator
  if (!rootNode.get("objDefs", node) ) {
    ATH_MSG_ERROR( "No 'objDefs' node in config file " << m_configFile );
    return StatusCode::FAILURE;
  }
  if (node.vectorSubNodes.size() != 1) {
    ATH_MSG_ERROR( node.vectorSubNodes.size() << " object definitions provided in config file " << m_configFile << ". ObjectCounter only expects 1" );
    return StatusCode::FAILURE;
  }
  if (!node.vectorSubNodes.front().get("decorator", m_decorator) ) {
    ATH_MSG_ERROR( "No decorator provided for 'objDef' node in config file " << m_configFile );
    return StatusCode::FAILURE;
  }

  JSONNode subNode;
  //m_definesElectrons = node.get("electron", subNode);
  //m_definesMuons = node.get("muon", subNode);
  //m_definesJets = node.get("jet", subNode);
  //m_definesPhotons = node.get("photon", subNode);
  //m_definesTaus = node.get("tau", subNode);

  for (auto& nodePair : rootNode.subNodes) {
    ATH_CHECK( makeBlankSignalTuple(nodePair.first) );
    SignalTuple& tup = m_signals.at(nodePair.first);
    if (nodePair.second.get("electron", node) ) {
      m_definesElectrons = true;
      ATH_CHECK( makeObjectCut(node, std::get<N_ELE>(tup) ) );
    }
    if (nodePair.second.get("muon", node) ) {
      m_definesMuons = true;
      ATH_CHECK( makeObjectCut(node, std::get<N_MU>(tup) ) );
    }
    if (nodePair.second.get("jet", node) ) {
      ATH_CHECK( makeObjectCut(node, std::get<N_JET>(tup) ) );
      m_definesJets = true;
    }
    if (nodePair.second.get("photon", node) ) {
      m_definesPhotons = true;
      ATH_CHECK( makeObjectCut(node, std::get<N_GAMMA>(tup) ) );
    }
    if (nodePair.second.get("tau", node) ) {
      m_definesTaus = true;
      ATH_CHECK( makeObjectCut(node, std::get<N_TAU>(tup) ) );
    }
    if (nodePair.second.get("met", node) ) ATH_CHECK( makeMetCut(node, std::get<METCUT>(tup) ) );
    nodePair.second.get("triggers", std::get<TRIGGERS>(tup) );
    ATH_CHECK( checkProperties(nodePair.second, m_configFile) );
  }

  return StatusCode::SUCCESS;
}

StatusCode ObjectCounter::retrieveObjects()
{
  ATH_MSG_DEBUG( "In ObjectCounter::retrieveObjects" );

  if (m_definesElectrons) {
    const xAOD::ElectronContainer* electrons;
    ATH_CHECK( evtStore()->retrieve(electrons, m_sgPrefix + "Electrons") );
    m_selElectrons = m_getSelected(electrons, cacc_sel);
  }

  if (m_definesMuons) {
    const xAOD::MuonContainer* muons;
    ATH_CHECK( evtStore()->retrieve(muons, m_sgPrefix + "Muons") );
    m_selMuons = m_getSelected(muons, cacc_sel);
  }

  if (m_definesJets) {
    const xAOD::JetContainer* jets;
    ATH_CHECK( evtStore()->retrieve(jets, m_sgPrefix+"Jets") );
    m_selJets = m_getSelected(jets, cacc_sel);
  }

  if (m_definesPhotons) {
    const xAOD::PhotonContainer* photons;
    ATH_CHECK( evtStore()->retrieve(photons, m_sgPrefix+"Photons") );
    m_selPhotons = m_getSelected(photons, cacc_sel);
  }

  if (m_definesTaus) {
    const xAOD::TauJetContainer* taus;
    ATH_CHECK( evtStore()->retrieve(taus, m_sgPrefix+"Taus") );
    m_selTaus = m_getSelected(taus, cacc_sel);
  }

  const xAOD::MissingETContainer* metCont;
  ATH_CHECK( evtStore()->retrieve(metCont, m_sgPrefix + "EtMiss") );
  auto metItr = metCont->find(m_metTerm);
  if (metItr == metCont->end() ) {
    ATH_MSG_ERROR( "No " << m_metTerm << " term in container " << m_sgPrefix + "EtMiss");
    return StatusCode::FAILURE;
  }
  m_met = (*metItr)->met() * GeV;
  
  return StatusCode::SUCCESS;
}

Root::TAccept ObjectCounter::passSelection(const std::string& name) const
{
  if (m_signals.find(name) == m_signals.end() ) {
    ATH_MSG_WARNING( "WARNING. Unrecognised selection " << name << " requested from ObjectCounter. Return blank accept" );
    return Root::TAccept("blank");
  }
  const SignalTuple& tup = m_signals.at(name);
  Root::TAccept accept(name.c_str() );
  accept.addCut("nEvents", "Start number of events");
  accept.setCutResult("nEvents", true);
  if (std::get<TRIGGERS>(tup).size() != 0) accept.addCut("TriggerCut", "Passes triggers");
  if (m_definesElectrons) accept.addCut("ElectronCut", "Passes electron requirements");
  if (m_definesMuons) accept.addCut("MuonCut", "Passes muon requirements");
  if (m_definesJets) accept.addCut("JetCut", "Passes jet requirements");
  if (m_definesPhotons) accept.addCut("PhotonCut", "Passes photon requirements");
  if (m_definesTaus) accept.addCut("TauCut", "Passes tau requirements");
  accept.addCut("METCut", "Passes met requirements");

  if (std::get<TRIGGERS>(tup).size() != 0) {
    if (!m_passTriggers(std::get<TRIGGERS>(tup) ) ) return accept;
    accept.setCutResult("TriggerCut", true);
  }
  if (m_definesElectrons) {
    const ObjectPair& objPair = std::get<N_ELE>(tup);
    if (!objPair.second(m_selElectrons.size(), objPair.first) ) return accept;
    accept.setCutResult("ElectronCut", true);
  }
  if (m_definesMuons) {
    const ObjectPair& objPair = std::get<N_MU>(tup);
    if (!objPair.second(m_selMuons.size(), objPair.first) ) return accept;
    accept.setCutResult("MuonCut", true);
  }
  if (m_definesJets) {
    const ObjectPair& objPair = std::get<N_JET>(tup);
    if (!objPair.second(m_selJets.size(), objPair.first) ) return accept;
    accept.setCutResult("JetCut", true);
  }
  if (m_definesPhotons) {
    const ObjectPair& objPair = std::get<N_GAMMA>(tup);
    if (!objPair.second(m_selPhotons.size(), objPair.first) ) return accept;
    accept.setCutResult("PhotonCut", true);
  }
  if ( m_definesTaus) {
    const ObjectPair& objPair = std::get<N_TAU>(tup);
    if (!objPair.second(m_selTaus.size(), objPair.first) ) return accept;
    accept.setCutResult("TauCut", true);
  }
  const MetPair& metPair = std::get<METCUT>(tup);
  if (!metPair.second( m_met, metPair.first) ) return accept;
  accept.setCutResult("METCut", true);
  return accept;
}
