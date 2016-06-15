#ifndef METTRIGGERTOOLS_METTRIGGERHELPER_H
#define METTRIGGERTOOLS_METTRIGGERHELPER_H 1


#include "AsgTools/AsgTool.h"

#include "METTriggerTools/IMETTriggerHelper.h"



class METTriggerHelper: public asg::AsgTool, public virtual IMETTriggerHelper { 
  public: 
    //constructor for athena can be created using special macro
    //Note: if you add a second interface to your tool, you must use: ASG_TOOL_CLASS2( ToolName, Interface1, Interface2) 
    ASG_TOOL_CLASS( METTriggerHelper , IMETTriggerHelper )
      //add another constructor for non-athena use cases
    METTriggerHelper( const std::string& name );

    /// Initialize is required by AsgTool base class
    virtual StatusCode  initialize() override;

    virtual StatusCode getL1MET(const xAOD::EnergySumRoI*& l1Obj) const override final;

    virtual StatusCode getHLTMET(const HLTMET& type, const xAOD::TrigMissingET*& hltMET) const override final;

    virtual float getMET(const xAOD::TrigMissingET*& hltMET) const override final;
    virtual float getMET(const xAOD::EnergySumRoI*& l1Obj) const override final;
    virtual StatusCode getMET(const HLTMET& type, float& met) const override final;

    virtual float getMETX(const xAOD::TrigMissingET*& hltMET) const override final;
    virtual float getMETX(const xAOD::EnergySumRoI*& l1Obj) const override final;
    virtual StatusCode getMETX(const HLTMET& type, float& metX) const override final;

    virtual float getMETY(const xAOD::TrigMissingET*& hltMET) const override final;
    virtual float getMETY(const xAOD::EnergySumRoI*& l1Obj) const override final;
    virtual StatusCode getMETY(const HLTMET& type, float& metY) const override final;

    virtual float getSumEt(const xAOD::TrigMissingET*& hltMET) const override final;
    virtual float getSumEt(const xAOD::EnergySumRoI*& l1Obj) const override final;
    virtual StatusCode getSumEt(const HLTMET& type, float& sumEt) const override final;

    virtual const std::string& getHLTMETContainerName(const HLTMET& type) const override final;

    virtual const std::string& getHLTMETName(const HLTMET& type) const override final;

    virtual std::string getTriggerName(const HLTMET& type, int cut, int L1Cut) const override final;
  protected:
    std::map<HLTMET, std::string> m_metContNames;
    std::map<HLTMET, std::string> m_metNames;
    std::map<HLTMET, std::pair<std::string, std::string>> m_splitMetNames;
}; 

#endif //> !METTRIGGERTOOLS_METTRIGGERHELPER_H
