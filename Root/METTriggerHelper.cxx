// METTriggerTools includes
#include "METTriggerTools/METTriggerHelper.h"
#include "xAODTrigMissingET/TrigMissingETContainer.h"



METTriggerHelper::METTriggerHelper( const std::string& name )
  : asg::AsgTool( name ),
    m_metContNames( { {HLTMET::Cell, "HLT_xAOD__TrigMissingETContainer_TrigEFMissingET"},
                      {HLTMET::Topocl, "HLT_xAOD__TrigMissingETContainer_TrigEFMissingET_topocl"},
                      {HLTMET::Topocl_PS, "HLT_xAOD__TrigMissingETContainer_TrigEFMissingET_topocl_PS"},
                      {HLTMET::Topocl_PUC, "HLT_xAOD__TrigMissingETContainer_TrigEFMissingET_topocl_PUC"},
                      {HLTMET::mht, "HLT_xAOD__TrigMissingETContainer_TrigEFMissingET_mht"},
                      {HLTMET::FEB, "HLT_xAOD__TrigMissingETContainer_TrigEFMissingET_FEB"},
                      {HLTMET::L1, "LVL1EnergySumRoI"} } ),
    m_metNames( { {HLTMET::Cell, "cell"},
                  {HLTMET::Topocl, "topocl"},
                  {HLTMET::Topocl_PS, "topocl_PS"},
                  {HLTMET::Topocl_PUC, "topocl_PUC"},
                  {HLTMET::mht, "mht"},
                  {HLTMET::FEB, "FEB"},
                  {HLTMET::L1, "L1"} } ),
    m_splitMetNames( { {HLTMET::Cell, {"HLT_xe", ""} },
                       {HLTMET::Topocl, {"HLT_xe", "_tc_lcw"} },
                       {HLTMET::Topocl_PS, {"HLT_xe", "_tc_pueta"} },
                       {HLTMET::Topocl_PUC, {"HLT_xe", "_tc_pufit"} },
                       {HLTMET::mht, {"HLT_xe", "_tc_mht"} },
                       {HLTMET::FEB, {"HLT_xe", "_feb"} }, // included for completeness - no FEB triggers exist
                       {HLTMET::L1, {"L1_XE", ""} } } )
{

}


StatusCode METTriggerHelper::initialize() {
  ATH_MSG_INFO ("Initializing " << name() << "...");
  //
  //Make use of the property values to configure the tool
  //Tools should be designed so that no method other than setProperty is called before initialize
  //

  return StatusCode::SUCCESS;
}

StatusCode METTriggerHelper::getL1MET(const xAOD::EnergySumRoI*& l1Obj) const
{
  return evtStore()->retrieve(l1Obj, "LVL1EnergySumRoI");
}

StatusCode METTriggerHelper::getHLTMET(const HLTMET& type, const xAOD::TrigMissingET*& hltMET) const
{
  if (type == HLTMET::L1) {
    ATH_MSG_ERROR("HLTMET::L1 given to getHLTMET! Use getL1MET instead.");
    return StatusCode::FAILURE;
  }
  const xAOD::TrigMissingETContainer* trigMetCont(0);
  ATH_CHECK( evtStore()->retrieve(trigMetCont, m_metContNames.at(type) ) );
  if (trigMetCont->size() != 1) {
    ATH_MSG_ERROR( "Trigger MET container " << m_metContNames.at(type) << " has size " << trigMetCont->size() << ". This should be 1" );
    return StatusCode::FAILURE;
  }
  hltMET = trigMetCont->front();
  return StatusCode::SUCCESS;
}

float METTriggerHelper::getMET(const xAOD::TrigMissingET*& hltMET) const
{
  return sqrt(hltMET->ex() * hltMET->ex() + hltMET->ey() * hltMET->ey() );
}

float METTriggerHelper::getMET(const xAOD::EnergySumRoI*& l1Obj) const
{
  return sqrt(l1Obj->exMiss() * l1Obj->exMiss() + l1Obj->eyMiss() * l1Obj->eyMiss() );
}

StatusCode METTriggerHelper::getMET(const HLTMET& type, float& met) const
{
  if (type == HLTMET::L1) {
    const xAOD::EnergySumRoI* l1Obj(0);
    ATH_CHECK( getL1MET(l1Obj) );
    met = getMET(l1Obj);
  }
  else {
    const xAOD::TrigMissingET* hltMET(0);
    ATH_CHECK( getHLTMET(type, hltMET) );
    met = getMET(hltMET);
  }
  return StatusCode::SUCCESS;
}

float METTriggerHelper::getMETX(const xAOD::TrigMissingET*& hltMET) const
{
  return hltMET->ex();
}

float METTriggerHelper::getMETX(const xAOD::EnergySumRoI*& l1Obj) const
{
  return l1Obj->exMiss();
}

StatusCode METTriggerHelper::getMETX(const HLTMET& type, float& metX) const
{
  if (type == HLTMET::L1) {
    const xAOD::EnergySumRoI* l1Obj;
    ATH_CHECK( getL1MET(l1Obj) );
    metX = getMETX(l1Obj);
  }
  else {
    const xAOD::TrigMissingET* hltMET;
    ATH_CHECK( getHLTMET(type, hltMET) );
    metX = getMETX(hltMET);
  }
  return StatusCode::SUCCESS;
}

float METTriggerHelper::getMETY(const xAOD::TrigMissingET*& hltMET) const
{
  return hltMET->ex();
}

float METTriggerHelper::getMETY(const xAOD::EnergySumRoI*& l1Obj) const
{
  return l1Obj->exMiss();
}

StatusCode METTriggerHelper::getMETY(const HLTMET& type, float& metY) const
{
  if (type == HLTMET::L1) {
    const xAOD::EnergySumRoI* l1Obj;
    ATH_CHECK( getL1MET(l1Obj) );
    metY = getMETY(l1Obj);
  }
  else {
    const xAOD::TrigMissingET* hltMET;
    ATH_CHECK( getHLTMET(type, hltMET) );
    metY = getMETY(hltMET);
  }
  return StatusCode::SUCCESS;
}

float METTriggerHelper::getSumEt(const xAOD::TrigMissingET*& hltMET) const
{
  return hltMET->sumEt();
}

float METTriggerHelper::getSumEt(const xAOD::EnergySumRoI*& l1Obj) const
{
  return l1Obj->energyT();
}

StatusCode METTriggerHelper::getSumEt(const HLTMET& type, float& sumEt) const
{
  if (type == HLTMET::L1) {
    const xAOD::EnergySumRoI* l1Obj;
    ATH_CHECK( getL1MET(l1Obj) );
    sumEt = getSumEt(l1Obj);
  }
  else {
    const xAOD::TrigMissingET* hltMET;
    ATH_CHECK( getHLTMET(type, hltMET) );
    sumEt = getSumEt(hltMET);
  }
  return StatusCode::SUCCESS;
}

const std::string& METTriggerHelper::getHLTMETContainerName(const HLTMET& type) const
{
  return m_metContNames.at(type);
}

const std::string& METTriggerHelper::getHLTMETName(const HLTMET& type) const
{
  return m_metNames.at(type);
}

std::string METTriggerHelper::getTriggerName(const HLTMET& type, int cut, int L1Cut) const
{
  std::string L1part = "";
  if (L1Cut > 0) L1part = "_L1XE" + std::to_string(L1Cut);
  return m_splitMetNames.at(type).first + std::to_string(cut) + m_splitMetNames.at(type).second + L1part;
}
