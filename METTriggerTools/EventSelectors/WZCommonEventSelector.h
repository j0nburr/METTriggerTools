#ifndef METTRIGGERTOOLS_EVENTSELECTORS_WZCOMMONEVENTSELECTOR_H
#define METTRIGGERTOOLS_EVENTSELECTORS_WZCOMMONEVENTSELECTOR_H 1

#include "METTriggerTools/METTriggerEventSelector.h"

#include "xAODEgamma/ElectronContainer.h"
#include "xAODMuon/MuonContainer.h"
#include "xAODMissingET/MissingETContainer.h"

class WZCommonEventSelector : public virtual METTriggerEventSelector 
{
  ASG_TOOL_CLASS( WZCommonEventSelector, IMETTriggerEventSelector )
  public:
    WZCommonEventSelector(const std::string& name);

    virtual StatusCode initialize() override;

    virtual StatusCode retrieveObjects() override;

    virtual Root::TAccept passSelection(const std::string& name) const override;
  protected:

    StatusCode readConfigFile();
    
    SG::AuxElement::ConstAccessor<char> cacc_sel;
    std::string m_decorator;

    Root::TAccept passWenu() const;
    Root::TAccept passWmunu() const;
    Root::TAccept passZmumu() const;
    Root::TAccept passZee() const;

    ConstDataVector<xAOD::ElectronContainer> m_selElectrons;
    ConstDataVector<xAOD::MuonContainer> m_selMuons;
    const xAOD::MissingETContainer* m_met;

    // Configurables
    float m_Wenu_mT;
    float m_Wenu_met;
    std::vector<std::string> m_Wenu_triggers;

    float m_Wmunu_mT;
    float m_Wmunu_met;
    std::vector<std::string> m_Wmunu_triggers;

    float m_Zmumu_mllLow;
    float m_Zmumu_mllHigh;
    std::vector<std::string> m_Zmumu_triggers;

    float m_Zee_mllLow;
    float m_Zee_mllHigh;
    std::vector<std::string> m_Zee_triggers;

    std::string m_metTerm;
};
#endif //> !METTRIGGERTOOLS_EVENTSELECTORS_WZCOMMONEVENTSELECTOR_H
