#ifndef METTRIGGERTOOLS_IMETTRIGGERANALYSISTOOL_H
#define METTRIGGERTOOLS_IMETTRIGGERANALYSISTOOL_H 1

#include "AsgTools/IAsgTool.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODEgamma/PhotonContainer.h"
#include "xAODMuon/MuonContainer.h"
#include "xAODJet/JetContainer.h"
#include "xAODTau/TauJetContainer.h"
#include "xAODMissingET/MissingETContainer.h"
#include "xAODMissingET/MissingET.h"
#include "xAODCore/ShallowCopy.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODTracking/Vertex.h"
#include "xAODTrigger/EnergySumRoI.h"
#include "xAODTrigMissingET/TrigMissingET.h"

//#include "xAODBase/IParticle.h"

class IMETTriggerAnalysisTool : public virtual asg::IAsgTool {
  public:
    ASG_TOOL_INTERFACE( IMETTriggerAnalysisTool ) //declares the interface to athena
    // Functions to configure and initialize the subtools
    virtual StatusCode configure() = 0;
    virtual StatusCode initialize() = 0;


    virtual StatusCode getElectrons(xAOD::ElectronContainer*& copy,
                                    xAOD::ShallowAuxContainer*& copyAux,
                                    const bool recordSG = true,
                                    const std::string& key = "") const = 0;

    virtual StatusCode getPhotons(xAOD::PhotonContainer*& copy,
                                  xAOD::ShallowAuxContainer*& copyAux,
                                  const bool recordSG = true,
                                  const std::string& key = "") const = 0;

    virtual StatusCode getMuons(xAOD::MuonContainer*& copy,
                                xAOD::ShallowAuxContainer*& copyAux,
                                const bool recordSG = true,
                                const std::string& key = "") const = 0;

    virtual StatusCode getJets(xAOD::JetContainer*& copy,
                              xAOD::ShallowAuxContainer*& copyAux,
                              const bool recordSG = true,
                              const std::string& key = "") const = 0;

    virtual StatusCode getTaus(xAOD::TauJetContainer*& copy,
                              xAOD::ShallowAuxContainer*& copyAux,
                              const bool recordSG = true,
                              const std::string& key = "") const = 0;

    virtual StatusCode getMET(xAOD::MissingETContainer& met,
                              const xAOD::JetContainer* jets = 0,
                              const xAOD::ElectronContainer* electrons = 0,
                              const xAOD::MuonContainer* muons = 0,
                              const xAOD::PhotonContainer* photons = 0,
                              const xAOD::TauJetContainer* taus = 0,
                              bool doTST = true, bool doJVTCut = true) const = 0;

    virtual StatusCode removeOverlaps(const xAOD::ElectronContainer* electrons,
                                      const xAOD::MuonContainer* muons,
                                      const xAOD::JetContainer* jets,
                                      const xAOD::PhotonContainer* photons,
                                      const xAOD::TauJetContainer* taus) const = 0;

    virtual const xAOD::Vertex* getPrimaryVertex() const = 0;

};

#endif //> !METTRIGGERTOOLS_IMETTRIGGERANALYSISTOOL_H
