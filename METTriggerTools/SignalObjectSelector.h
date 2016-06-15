#ifndef METTriggerAnalysisTool_SignalObjectSelector_H
#define METTriggerAnalysisTool_SignalObjectSelector_H

#include <xAODEgamma/Electron.h>
#include <xAODEgamma/Photon.h>
#include <xAODMuon/Muon.h>
#include <xAODJet/Jet.h>
#include <xAODTau/TauJet.h>
#include <xAODBase/IParticle.h>

#include <string>
#include <tuple>

class SignalObjectSelector
{
public:
  SignalObjectSelector(const std::string& name);
  ~SignalObjectSelector();

  void initialize();
  
  float GeV = 0.001;
  std::string name;
  SG::AuxElement::Decorator<char> decorator;
  SG::AuxElement::ConstAccessor<char> pileup;
  SG::AuxElement::ConstAccessor<char> passCleaning;
  SG::AuxElement::ConstAccessor<float> z0sintheta;
  SG::AuxElement::ConstAccessor<float> d0sig;
  SG::AuxElement::ConstAccessor<float> d0;

  SG::AuxElement::ConstAccessor<char> cacc_eleIso;
  SG::AuxElement::ConstAccessor<char> cacc_eleID;
  SG::AuxElement::ConstAccessor<char> cacc_muID;
  SG::AuxElement::ConstAccessor<char> cacc_muIso;
  SG::AuxElement::ConstAccessor<char> cacc_gammaIso;
  SG::AuxElement::ConstAccessor<char> cacc_gammaID;
  SG::AuxElement::ConstAccessor<char> cacc_tauID;

  void selectElectron(const xAOD::Electron& electron) const;
  void selectMuon(const xAOD::Muon& muon) const;
  void selectJet(const xAOD::Jet& jet) const;
  void selectPhoton(const xAOD::Photon& photon) const;
  void selectTau(const xAOD::TauJet& tau) const;

  bool doElectrons;
  bool doElectronID;
  std::string electronID;
  float electronPt;
  float electronEta;
  bool doElectronIso;
  std::string electronIso;
  float electronZ0sintheta;
  float electronD0sig;
  float electronD0;
  bool doMuons;
  bool doMuonID;
  std::string muonID;
  float muonPt;
  float muonEta;
  bool doMuonIso;
  std::string muonIso;
  float muonZ0sintheta;
  float muonD0sig;
  float muonD0;
  bool doJets;
  float jetPt;
  float jetEta;
  bool doJetPileup;
  bool doJetBad;
  bool doPhotons;
  bool doPhotonID;
  std::string photonID;
  float photonPt;
  float photonEta;
  bool doPhotonIso;
  std::string photonIso;
  bool doTaus;
  bool doTauID;
  std::string tauID;
  float tauPt;
  float tauEta;
};
#endif
