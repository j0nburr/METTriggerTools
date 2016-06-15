#ifndef METTRIGGERTOOLS_EVENTSELECTORS_OBJECTCOUNTER_H
#define METTRIGGERTOOLS_EVENTSELECTORS_OBJECTCOUNTER_H 1

#include "METTriggerTools/METTriggerEventSelector.h"

#include "xAODEgamma/ElectronContainer.h"
#include "xAODMuon/MuonContainer.h"
#include "xAODJet/JetContainer.h"
#include "xAODEgamma/PhotonContainer.h"
#include "xAODTau/TauJetContainer.h"
#include "xAODMissingET/MissingETContainer.h"

#include <functional>

class ObjectCounter : public virtual METTriggerEventSelector 
{
  ASG_TOOL_CLASS( ObjectCounter, IMETTriggerEventSelector )
  public:
    ObjectCounter(const std::string& name);

    virtual StatusCode initialize() override;

    virtual StatusCode retrieveObjects() override;

    virtual Root::TAccept passSelection(const std::string& name) const override;
  protected:
    typedef std::pair<int, std::function<bool(const int&, const int&)>> ObjectPair;
    typedef std::pair<float, std::function<bool(const float&, const float&)>> MetPair;
    typedef std::tuple<ObjectPair, ObjectPair, ObjectPair, ObjectPair, ObjectPair, MetPair, std::vector<std::string>> SignalTuple;

    StatusCode makeBlankSignalTuple(const std::string& signalName);
    StatusCode makeObjectCut(JSONNode& node, ObjectPair& objPair) const;
    StatusCode makeMetCut(JSONNode& node, MetPair& metPair) const;

    StatusCode readConfigFile();
    
    SG::AuxElement::ConstAccessor<char> cacc_sel;
    std::string m_decorator;

    ConstDataVector<xAOD::ElectronContainer> m_selElectrons;
    ConstDataVector<xAOD::MuonContainer> m_selMuons;
    ConstDataVector<xAOD::JetContainer> m_selJets;
    ConstDataVector<xAOD::PhotonContainer> m_selPhotons;
    ConstDataVector<xAOD::TauJetContainer> m_selTaus;
    float m_met;

    // Configurables
    enum SignalTupleDescriber { N_ELE = 0,
                                N_MU = 1,
                                N_JET = 2,
                                N_GAMMA = 3,
                                N_TAU = 4,
                                METCUT = 5,
                                TRIGGERS = 6 };
    std::map<std::string, SignalTuple> m_signals;
    
    std::string m_metTerm;

    bool m_definesElectrons;
    bool m_definesMuons;
    bool m_definesJets;
    bool m_definesPhotons;
    bool m_definesTaus;
};
#endif //> !METTRIGGERTOOLS_EVENTSELECTORS_OBJECTCOUNTER_H
