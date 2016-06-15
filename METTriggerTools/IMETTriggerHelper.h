#ifndef METTRIGGERTOOLS_IMETTRIGGERHELPER_H
#define METTRIGGERTOOLS_IMETTRIGGERHELPER_H 1

#include "AsgTools/IAsgTool.h"
#include "xAODTrigger/EnergySumRoI.h"
#include "xAODTrigMissingET/TrigMissingET.h"

enum class HLTMET {
  Cell,
  Topocl,
  Topocl_PS,
  Topocl_PUC,
  mht,
  FEB,
  L1
};

class IMETTriggerHelper : public virtual asg::IAsgTool {
  public:
    ASG_TOOL_INTERFACE( IMETTriggerHelper ) //declares the interface to athena

    virtual StatusCode getL1MET(const xAOD::EnergySumRoI*& l1Obj) const = 0;

    virtual StatusCode getHLTMET(const HLTMET& type, const xAOD::TrigMissingET*& hltMET) const = 0;

    virtual float getMET(const xAOD::TrigMissingET*& hltMET) const = 0;
    virtual float getMET(const xAOD::EnergySumRoI*& l1Obj) const = 0;
    virtual StatusCode getMET(const HLTMET& type, float& met) const = 0;

    virtual float getMETX(const xAOD::TrigMissingET*& hltMET) const = 0;
    virtual float getMETX(const xAOD::EnergySumRoI*& l1Obj) const = 0;
    virtual StatusCode getMETX(const HLTMET& type, float& metX) const = 0;

    virtual float getMETY(const xAOD::TrigMissingET*& hltMET) const = 0;
    virtual float getMETY(const xAOD::EnergySumRoI*& l1Obj) const = 0;
    virtual StatusCode getMETY(const HLTMET& type, float& metY) const = 0;

    virtual float getSumEt(const xAOD::TrigMissingET*& hltMET) const = 0;
    virtual float getSumEt(const xAOD::EnergySumRoI*& l1Obj) const = 0;
    virtual StatusCode getSumEt(const HLTMET& type, float& sumEt) const = 0;

    virtual const std::string& getHLTMETContainerName(const HLTMET& type) const = 0;

    virtual const std::string& getHLTMETName(const HLTMET& type) const = 0;

    virtual std::string getTriggerName(const HLTMET& type, int cut, int L1Cut = -1) const = 0;
};

#endif //> !METTRIGGERTOOLS_IMETTRIGGERHELPER_H
