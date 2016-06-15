// METTriggerTools includes
#include "METTriggerTools/METTriggerEventSelector.h"

#include "xAODJet/JetContainer.h"
#include "xAODTracking/VertexContainer.h"
#include "TrigConfInterfaces/ITrigConfigTool.h"
#include "TrigDecisionInterface/ITrigDecisionTool.h"

METTriggerEventSelector::METTriggerEventSelector( const std::string& name )
  : asg::AsgTool( name ),
    cacc_pileupJet("") ,
    cacc_cleanJet("")
{
  declareProperty( "DoPassOR", m_doPassOR = true );
  declareProperty( "PileupJetDecorator", m_pileupJetDec = "pileupJet" );
  declareProperty( "PassCleaningJetDecorator", m_passCleaningJetDec = "passCleaning" );
  declareProperty( "BadJetPtCut", m_badJetPtCut = 20 );
  declareProperty( "PrimaryVertexContainerName", m_primaryVertexContainerName = "PrimaryVertices" );
  declareProperty( "nPrimaryVertexTracksMin", m_nPrimaryVertexTracksMin = 2 );
  declareProperty( "TriggerTool", m_trigDecTool.handle() );
  declareProperty( "ConfigFile", m_configFile );
  declareProperty( "StoreGatePrefix", m_sgPrefix );
}


StatusCode METTriggerEventSelector::initialize() {
  ATH_MSG_INFO ("Initializing " << name() << "...");

  cacc_pileupJet = SG::AuxElement::ConstAccessor<char>(m_pileupJetDec);
  cacc_cleanJet = SG::AuxElement::ConstAccessor<char>(m_passCleaningJetDec);
  if ( m_trigDecTool.empty() ) {
    ATH_CHECK( m_trigDecTool.make("Trig::TrigDecisionTool/TrigDecisionTool") );
    if (!m_trigDecTool.isUserConfigured() ) {
      asg::AnaToolHandle<TrigConf::ITrigConfigTool> trigConfTool;
      ATH_CHECK( trigConfTool.make("TrigConf::xAODConfigTool/xAODConfigTool") );
      ATH_CHECK( trigConfTool.retrieve() );

      ATH_CHECK( m_trigDecTool.setProperty("ConfigTool", trigConfTool) );
      ATH_CHECK( m_trigDecTool.setProperty("TrigDecisionKey", "xTrigDecision") );
    }
  }
  ATH_CHECK( m_trigDecTool.retrieve() );
  //
  //Make use of the property values to configure the tool
  //Tools should be designed so that no method other than setProperty is called before initialize
  //

  return StatusCode::SUCCESS;
}

StatusCode METTriggerEventSelector::retrieveObjects()
{
  return StatusCode::SUCCESS;
}

Root::TAccept METTriggerEventSelector::passSelection(const std::string&) const
{
  return Root::TAccept();
}


Root::TAccept METTriggerEventSelector::eventVetoes(const xAOD::JetContainer* jets) const
{
  Root::TAccept accept ("EventVetoes");
  accept.addCut("PrimaryVertexCut", "Primary Vertex Cut");
  accept.addCut("BadJetCut", "Jet based event cleaning");
  accept.setCutResult("PrimaryVertexCut", m_passPrimaryVertexCut() );
  accept.setCutResult("BadJetCut", m_passJetCleaningCut(jets) );
  return accept;
}

bool METTriggerEventSelector::m_passJetCleaningCut(const xAOD::JetContainer* jets) const
{
  if (!jets) ATH_CHECK( evtStore()->retrieve(jets, m_sgPrefix + "Jets") );
  if (!jets) {
    ATH_MSG_ERROR( "Failed to retrieve jets!" );
    return false;
  }
  for (const xAOD::Jet* jet : *jets) {
    if (cacc_pileupJet(*jet) ) continue;
    if (!cacc_cleanJet(*jet) &&
        jet->pt() * GeV > m_badJetPtCut) return false;
  }
  return true;
}

bool METTriggerEventSelector::m_passPrimaryVertexCut() const
{
  const xAOD::VertexContainer* vertices(0);
  const xAOD::Vertex* priVtx(0);
  if (evtStore()->retrieve(vertices, m_primaryVertexContainerName).isSuccess() ) {
    for (const auto& vtx : *vertices) {
      if (vtx->vertexType() == xAOD::VxType::PriVtx) {
        priVtx = vtx;
        break;
      }
    }
    if (priVtx == 0) return false;
    if (priVtx->nTrackParticles() < m_nPrimaryVertexTracksMin) return false;
  }
  else {
    ATH_MSG_WARNING( "Unable to retrieve container " << m_primaryVertexContainerName );
    return false;
  }
  return true;
}

bool METTriggerEventSelector::m_passTriggers(const std::vector<std::string>& triggers, std::string mode) const
{
  if (triggers.size() == 0) return true;
  if (mode == "OR") {
    for (const std::string& trigger : triggers) {
      if (m_trigDecTool->isPassed(trigger) ) return true;
    }
    return false;
  }
  else if (mode == "AND") {
    for (const std::string& trigger: triggers) {
      if (!m_trigDecTool->isPassed(trigger) ) return false;
    }
    return true;
  }
  else {
    ATH_MSG_WARNING( "Invalid mode " << mode << " supplied to METTriggerEventSelector::m_passTriggers. Return false" );
  }
  return false;
}

TLorentzVector METTriggerEventSelector::m_compositeFourMom(TLorentzVector p4) const
{
  return p4;
}


float METTriggerEventSelector::m_mT(const TLorentzVector& p4, const xAOD::MissingET* met) const
{
  return (sqrt(2 * p4.Pt() * GeV * met->met() * GeV * (1 - cos(met->phi() - p4.Phi() ) ) ) );
}

float METTriggerEventSelector::m_mT(const xAOD::IParticle* particle, const xAOD::MissingET* met) const
{
  return m_mT(particle->p4(), met);
}

StatusCode METTriggerEventSelector::checkProperties(JSONNode& node, const std::string& context)
{
  if (node.subNodes.size() != 0) {
    ATH_MSG_ERROR( "Error. Unexpected configuration(s) provided to " << context << ":" ); 
    for (const auto& pair : node.subNodes) ATH_MSG_ERROR(pair.first);
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

void METTriggerEventSelector::fillCutflow(Root::TAccept accept, TH1* cutflow, float weight) const
{
  for (unsigned int i = 0; i < accept.getNCuts(); ++i) {
    cutflow->Fill(accept.getCutName(i), accept.getCutResult(i) ? weight : 0);
  }
}

