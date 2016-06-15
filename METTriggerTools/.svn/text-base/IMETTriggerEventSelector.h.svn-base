#ifndef METTRIGGERTOOLS_IMETTRIGGEREVENTSELECTOR_H
#define METTRIGGERTOOLS_IMETTRIGGEREVENTSELECTOR_H 1

#include "AsgTools/IAsgTool.h"

#include "xAODJet/JetContainer.h"

#include "PATCore/TAccept.h"
#include "TH1.h"

class IMETTriggerEventSelector : public virtual asg::IAsgTool {
  public:
    ASG_TOOL_INTERFACE( IMETTriggerEventSelector ) //declares the interface to athena

    virtual StatusCode initialize() = 0;

    virtual StatusCode retrieveObjects() = 0;
    
    virtual Root::TAccept passSelection( const std::string& name ) const = 0;

    virtual Root::TAccept eventVetoes(const xAOD::JetContainer* jets = 0) const = 0;

    virtual void fillCutflow(Root::TAccept accept, TH1* cutflow, float weight = 1.) const = 0;
};

#endif //> !METTRIGGERTOOLS_IMETTRIGGEREVENTSELECTOR_H
