#ifndef METTRIGGERTOOLS_METTRIGGEREVENTSELECTOR_H
#define METTRIGGERTOOLS_METTRIGGEREVENTSELECTOR_H 1

#include "AsgTools/AsgTool.h"
#include "AsgTools/AnaToolHandle.h"

#include "METTriggerTools/IMETTriggerEventSelector.h"

#include "xAODBase/IParticle.h"
#include "xAODMissingET/MissingET.h"
#include "xAODJet/JetContainer.h"

#include "TLorentzVector.h"
#include "AthContainers/ConstDataVector.h"
#include "METTriggerTools/jsonParser.h"

namespace Trig {
  class ITrigDecisionTool;
}

class METTriggerEventSelector: public asg::AsgTool, public virtual IMETTriggerEventSelector { 
  public: 
    //constructor for athena can be created using special macro
    //Note: if you add a second interface to your tool, you must use: ASG_TOOL_CLASS2( ToolName, Interface1, Interface2) 
    ASG_TOOL_CLASS( METTriggerEventSelector, IMETTriggerEventSelector )
    //add another constructor for non-athena use cases
    METTriggerEventSelector( const std::string& name );

    /// Initialize is required by AsgTool base class
    virtual StatusCode  initialize() override;

    /// Retrieve the objects needed for selection
    virtual StatusCode retrieveObjects() override;

    /// pass a particular selection
    virtual Root::TAccept passSelection( const std::string& name) const override;

    virtual Root::TAccept eventVetoes(const xAOD::JetContainer* jets) const override final;

    virtual void fillCutflow(Root::TAccept accept, TH1* cutflow, float weight) const override final;
  protected: 

    SG::AuxElement::ConstAccessor<char> cacc_pileupJet;
    SG::AuxElement::ConstAccessor<char> cacc_cleanJet;

    bool m_doPassOR;

    float GeV = 0.001;

    bool m_passJetCleaningCut(const xAOD::JetContainer* jets = 0) const;
    bool m_passPrimaryVertexCut() const;
    
    bool m_passTriggers (const std::vector<std::string>& trigNames, std::string mode = "OR") const;

    TLorentzVector m_compositeFourMom(TLorentzVector p4) const;

    template <typename ... Ts>
    TLorentzVector m_compositeFourMom(TLorentzVector p4, const xAOD::IParticle* particle, Ts ... args) const;

    template <typename ... Ts>
    TLorentzVector m_compositeFourMom(const xAOD::IParticle* particle, Ts ... args) const;

    template <typename ... Ts>
    float m_invMass(const xAOD::IParticle* particle, Ts ... args) const;

    float m_mT(const TLorentzVector& p4, const xAOD::MissingET* met) const;

    float m_mT(const xAOD::IParticle* particle, const xAOD::MissingET* met) const;

    template <typename T>
    ConstDataVector<T> m_getSelected(const T* input, const SG::AuxElement::ConstAccessor<char>& selector) const;

    StatusCode checkProperties(JSONNode& node, const std::string& context);

    std::string m_pileupJetDec;
    std::string m_passCleaningJetDec;
    float m_badJetPtCut;

    std::string m_primaryVertexContainerName;
    unsigned int m_nPrimaryVertexTracksMin;

    asg::AnaToolHandle<Trig::ITrigDecisionTool> m_trigDecTool;

    std::string m_configFile;
    std::string m_sgPrefix;
}; 
template <typename ... Ts>
TLorentzVector METTriggerEventSelector::m_compositeFourMom(TLorentzVector p4, const xAOD::IParticle* particle, Ts ... args) const
{
  return m_compositeFourMom(p4 + particle->p4(), args...);
}

template <typename ... Ts>
TLorentzVector METTriggerEventSelector::m_compositeFourMom(const xAOD::IParticle* particle, Ts ... args) const
{
  return m_compositeFourMom(particle->p4(), args...);
}

template <typename ... Ts>
float METTriggerEventSelector::m_invMass(const xAOD::IParticle* particle, Ts ... args) const
{
  return m_compositeFourMom(particle, args...).M();
}
template <typename T>
ConstDataVector<T> METTriggerEventSelector::m_getSelected(const T* input, const SG::AuxElement::ConstAccessor<char>& selector) const
{
  ConstDataVector<T> selObjs(SG::VIEW_ELEMENTS);
  typedef typename T::base_value_type TBaseValue;
  for (const TBaseValue* obj : *input) {
    if (selector(*obj) ) selObjs.push_back(obj);
  }
  return selObjs;
}

#endif //> !METTRIGGERTOOLS_METTRIGGEREVENTSELECTOR_H
