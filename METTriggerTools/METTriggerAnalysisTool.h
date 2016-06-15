#ifndef METTRIGGERTOOLS_METTRIGGERANALYSISTOOL_H
#define METTRIGGERTOOLS_METTRIGGERANALYSISTOOL_H 1
#include "AsgTools/AsgTool.h"
#include "AsgTools/AnaToolHandle.h"
#include "METTriggerTools/IMETTriggerAnalysisTool.h"
#include "METTriggerTools/jsonParser.h"
#include "METTriggerTools/SignalObjectSelector.h"
#include "AssociationUtils/ORToolBox.h"

#include "xAODEgamma/Electron.h"
#include "xAODEgamma/Photon.h"
#include "xAODJet/Jet.h"
#include "xAODTau/TauJet.h"
#include "xAODMuon/Muon.h"

#include <set>

class JSONNode;

// Tool interfaces

class IJetCalibrationTool;
class IJetSelector;
class IJetUpdateJvt;

class IAsgElectronLikelihoodTool;
class IAsgPhotonIsEMSelector;

class IMETMaker;

namespace CP {
  class IEgammaCalibrationAndSmearingTool;
  class IMuonSelectionTool;
  class IMuonCalibrationAndSmearingTool;
  class IIsolationSelectionTool;
}

namespace TauAnalysisTools {
  class ITauSelectionTool;
}

namespace TrigConf {
  class ITrigConfigTool;
}

namespace Trig {
  class ITrigDecisionTool;
}

namespace ORUtils {
  class IOverlapRemovalTool;
  class IOverlapTool;
  //class ToolBox;
}
template <typename T>
class AuxElementMapComparisonOperator {
  public:
    bool operator() (const T& lhs, const T& rhs) {
      return lhs.auxid() < rhs.auxid();
    }
};

class METTriggerAnalysisTool: public asg::AsgTool, public virtual IMETTriggerAnalysisTool { 
  public: 
    /**
      * Create a proper constructor for athena
      */
    ASG_TOOL_CLASS( METTriggerAnalysisTool, IMETTriggerAnalysisTool )
    /**
      * Standard constructor with name
      */
    METTriggerAnalysisTool( const std::string& name );

    /**
      * \brief Configure the object and signal selection
      *
      * This is called automatically by initialize and shouldn't be called by the user
      * The configuration is set by the config files provided to the tool (through set options or
      * the python constructor in an athena jobOptions). It will fail if an invalid json file is
      * provided or if no configuration is provided for the selection of objects for met rebuilding
      */ 
    virtual StatusCode configure() override;


    /**
      * \brief Initialize the subtools
      * 
      * Initializes all the subtools. Each tool is named according to the following pattern:
      * <analysisToolName>_<subToolName>_<workingPoint(optional)>
      * The working point is added when there are multiple instances of the same tool 
      * (for example multiple electron selection tools for different object definitions).
      * Calls configure() so can fail on the same conditions. Can also fail if any of the subtools
      * fail to initialize.
      */
    virtual StatusCode initialize() override;

    virtual StatusCode finalize() override;

    /**
      * \brief Retrieve, calibrate and select electrons
      * 
      * Retrieves the electrons using the container key provided to the tool ("Electrons" by
      * default). If the 'key' parameter is provided it is used instead. The calibrated electrons
      * are stored in copy and copyAux (which should be passed as empty pointers - if they own
      * memory then you risk a memory leak). By default the user is responsible for clearing the 
      * memory allocated here. If recordSG is set to true then they will be recorded  to the 
      * StoreGate service (which will be cleared after each event). If this is done then the memory
      * should not be manually freed.
      * They will be stored with the names:
      * <analysisToolName>_<key> and
      * <analysisToolName>_<key>"Aux".
      * Extra information will be stored as decorators:
      * The ID information is stored as "ID:"<WorkingPoint> (e.g. "ID:LooseLH")
      * The isolation information is stored as "iso:"<WorkingPoint> (e.g. "iso:Gradient")
      * If available z0 sin(theta), d0 sig and d0 will be stored using the keys provided to the
      * tool (by default: "z0sintheta", "d0sig" and "d0".
      * Signal selections are stored by the names given in the configuration files.
      * 
      * Can fail if recordSG is set to true and that name has already been registered or if
      * somehow the tool fails to retrieve the EventInfo collection.
      */
    StatusCode getElectrons(xAOD::ElectronContainer*& copy,
        xAOD::ShallowAuxContainer*& copyAux,
        const bool recordSG,
        const std::string& key) const override;

    /**
      * \brief Retrieve, calibrate and select photons
      *
      * Retrieves the photons using the container key provided to the tool ("Photons" by default).
      * If the 'key' parameter is provided it is used instead. The calibrated photons are stored in
      * copy and copyAux (which should be passed as empty pointers - if they own memory then you
      * risk a memory leak). By default the user is responsible for clearing the memory allocated
      * here. If recordSG is set to true then they will be recorded to the StoreGate service (which
      * will be cleared after each event). If this is done then the memory should not be manually
      * freed.
      * They will be stored with the names:
      * <analysisToolName>_<key> and
      * <analysisToolName>_<key>"Aux".
      * Extra information will be stored as decorators:
      * The ID information is stored as "ID:"<WorkingPoint> (e.g. "ID:Tight")
      * The isolation information is stored as "iso:"<WorkingPoint> (e.g. "iso:FixedCutLoose")
      * Signal selections are stored by the anmes given in the configuration files.
      *
      * If the photon author isn't "Photon" or "Ambiguous" then the object is not calibrated and
      * the signal decorators are set to false.
      *
      * Can fail if recordSG is set to true and that name has already been registed.
      */
    StatusCode getPhotons(xAOD::PhotonContainer*& copy,
        xAOD::ShallowAuxContainer*& copyAux,
        const bool recordSG,
        const std::string& key) const override;

    /**
      * \brief Retrieve, calibrate and select muons
      *
      * Retrieves the muons using the container key provided to the tool ("Muons" by default). If
      * the 'key' parameter is provided it is used instead. The calibrated muons are then stored in
      * copy and copyAux (which should be passed as empty pointers - if they own memory then you 
      * risk a memory leak). By default the user is responsible for clearing the memory allocated
      * here. If recordSG is set to true then they will be recorded to the StoreGate service (which
      * will be cleared after each event). If this is done then the memory should not be manually
      * freed.
      * They will be stored with the names:
      * <analysisToolName>_<key> and
      * <analysisToolName>_<key>"Aux".
      * Extra information will be stored as decorators:
      * The ID information is stored as "ID:"<WorkingPoint> (e.g. "ID:Medium")
      * The isolation information is stored as "iso:"<WorkingPoint> (e.g. "iso:Gradient")
      * If available z0 sin(theta), d0 sig and d0 will be stored using the keys provided to the
      * tool (by default: "z0sintheta", "d0sig" and "d0".
      * Signal selections are stored by the names given in the configuration files.
      * 
      * Can fail if recordSG is set to true and that name has already been registered or if somehow
      * the tool fails to retrieve the EventInfo collection.
      */

    StatusCode getMuons(xAOD::MuonContainer*& copy,
        xAOD::ShallowAuxContainer*& copyAux,
        const bool recordSG,
        const std::string& key) const override;

    /**
      * \brief Retrieve, calibrate and select jets
      *
      * Retrieves the jets using the container key provided to the tool ("AntiKt4EMTopoJets" by 
      * default). If the 'key' parameter is provided it is used instead. The calibrated jets are 
      * then stored in copy and copyAux (which should be passed as empty pointers - if they own
      * memory then you risk a memory leak). By default the user is responsible for clearing the 
      * memory allocated here. If recordSG is set to true then they will be recorded to the 
      * StoreGate service (which will be cleared after each event). If this is done then the memory
      * should not be manually freed.
      * They will be stored with the names:
      * <analysisToolName>_<key> and
      * <analysisToolName>_<key>"Aux".
      * Extra information will be stored as decorators:
      * Signal selections are stored by the names given in the configuration files.
      * 
      * Can fail if recordSG is set to true and that name has already been registered.
      */
    StatusCode getJets(xAOD::JetContainer*& copy,
        xAOD::ShallowAuxContainer*& copyAux,
        const bool recordSG,
        const std::string& key) const override;

    /**
      * \brief Retrieve, calibrate and select taus
      *
      * Retrieves the taus using the container key provided to the tool ("TauJets" by default). If
      * the 'key' parameter is provided it is used instead. The calibrated taus are then stored in
      * copy and copyAux (which should be passed as empty pointers - if they own memory then you
      * risk a memory leak). By default the user is responsible for clearing the memory allocated
      * here. If recordSG is set to true then they will be recorded to the StoreGate service (which
      * will be cleared after each event). If this is done then teh memory should not be manually
      * freed.
      * They will be stored with the names:
      * <analysisToolName>_<key> and
      * <analysisToolName>_<key>"Aux".
      * Extra information will be stored as decorators:
      * The ID information is stored as "ID:"<WorkingPoint> (e.g. "ID:Tight")
      * Signal selections are stored by the names given in the configuration files.
      *
      * Can fail if recordSG is set to true and that name has already been registered.
      */
    StatusCode getTaus(xAOD::TauJetContainer*& copy,
        xAOD::ShallowAuxContainer*& copyAux,
        const bool recordSG,
        const std::string& key) const override;

    /**
      * \brief Rebuild the MET
      *
      * Rebuilds the MET using the containers provided. A jet container has to be provided. If a
      * container is not supplied then the MET is rebuilt without that container. doTST is
      * currently useless and may be removed in a future update. doJVTCut controls whether a JVT cut
      * is applied to the jets before being used to rebuild the met (to reduce the effect of pileup)
      * This is the recommended way to rebuild met.
      * The objects used to rebuild the MET will be those within the container that have the met
      * decorator applied to them (this is set in the MET config file). A different decorator can be
      * supplied using the decorator argument. All jets will be used regardless of decorator (this
      * is the recommended behaviour by the met group).
      * The output container will contain elements containing the various contributions. The names
      * of these are properties that can be set (with the exception of the soft components which are
      * hardcoded though this may change in the future). The rebuilt met values will be contained in
      * elements named using the m_outTSTMetTerm and m_outCSTMetTerm data members (which are 
      * settable properties)
      */
    StatusCode getMET(xAOD::MissingETContainer& met,
        const xAOD::JetContainer* jets,
        const xAOD::ElectronContainer* electrons,
        const xAOD::MuonContainer* muons,
        const xAOD::PhotonContainer* photons,
        const xAOD::TauJetContainer* taus,
        bool doTST, bool doJVTCut) const override;

    /**
      * \brief Remove overlaps between objects
      *
      * Removes overlaps between signal objects. For each signal defintion provided in config files 
      * this removes overlaps between them decorating objects with a decorator <dec>:passOR (where
      * <dec> is the specified decorator name). There is currently no way of removing overlaps
      * between different signal specifications.
      */
    StatusCode removeOverlaps(const xAOD::ElectronContainer* electrons,
        const xAOD::MuonContainer* muons,
        const xAOD::JetContainer* jets,
        const xAOD::PhotonContainer* photons,
        const xAOD::TauJetContainer* taus) const override;
    /** 
      * \brief Retrieves the primary vertex for the event (if it exists)
      */
    const xAOD::Vertex* getPrimaryVertex() const override;

  private:

    /**
      * \brief Calibrate and select electrons
      *
      * called by getElectrons
      */
    StatusCode fillElectron(xAOD::Electron& ele, const xAOD::Vertex* priVtx) const;
    /**
      * \brief Calibrate and select photons
      *
      * called by getPhotons
      */
    StatusCode fillPhoton(xAOD::Photon& photon) const;

    /**
      * \brief Calibrate and select jets
      *
      * called by getJets
      */
      StatusCode fillJet(xAOD::Jet& jet) const;
      
      /**
      * \brief Calibrate and select muons
      * 
      * called by getMuons
      */
      StatusCode fillMuon(xAOD::Muon& muon, const xAOD::Vertex* priVtx) const;
      
      /**
        * \brief Calibrate and select taus
        *
        * called by getTaus
        */
      StatusCode fillTau(xAOD::TauJet& tau) const;
        
      StatusCode toolsInit();

      StatusCode readConfigFile(const std::string& configName);
      StatusCode checkProperties(JSONNode& node, const std::string& context);
      StatusCode configureMET(JSONNode& configNode);
      StatusCode configureSignalObjects(JSONNode& objDefNode, SignalObjectSelector& sel);
      bool m_configured;
      bool m_configuredMET;
      bool m_toolsInitDone;

      bool m_isData;
      bool m_isAtlfast;

      std::vector<std::string> m_configFiles;

      // Input container names
      std::string m_electronContainerName;
      std::string m_muonContainerName;
      std::string m_photonContainerName;
      std::string m_tauContainerName;
      std::string m_jetPrefix; // Careful - jets are treated a little differently
      std::string m_metCorePrefix;
      std::string m_metAssocPrefix;

      int m_jetInputType;
      int m_muonID;

      std::string m_metObjectKey;
      SG::AuxElement::ConstAccessor<char> cacc_met;

      // Met terms
      std::string m_electronMetTerm;
      std::string m_muonMetTerm;
      std::string m_photonMetTerm;
      std::string m_tauMetTerm;
      std::string m_jetMetTerm;
      std::string m_outTSTMetTerm;
      std::string m_outCSTMetTerm;

      // Met object cuts
      SignalObjectSelector m_metObjectSelector;

      //std::map<std::string, SignalObjectSelector> m_objSelectors;
      std::vector<SignalObjectSelector> m_objSelectors;

      std::set<std::string> m_requestedElectronIDs;
      std::set<std::string> m_requestedElectronIsos;
      std::set<std::string> m_requestedMuonIDs;
      std::set<std::string> m_requestedMuonIsos;
      std::set<std::string> m_requestedPhotonIDs;
      std::set<std::string> m_requestedPhotonIsos;
      std::set<std::string> m_requestedTauIDs;

      // Subtool output level (by default the same as the tool's)
      int m_subToolOutputLevel;
      
      // pileup cuts
      float m_JVTCut;
      float m_pileupEtaCut;
      float m_pileupPtCut;
      
      // for OverlapRemoval
      bool m_ORDoTaus;
      bool m_ORDoPhotons;
      std::string m_orInput;
      std::string m_orOutput;

      float GeV;

      SG::AuxElement::Decorator<float> dec_z0sintheta;
      SG::AuxElement::Decorator<float> dec_d0sig;
      SG::AuxElement::Decorator<float> dec_d0;

      // Tools
      asg::AnaToolHandle<IJetCalibrationTool> m_jetCalibrationTool;
      asg::AnaToolHandle<IJetSelector> m_jetCleaningTool;
      asg::AnaToolHandle<IJetUpdateJvt> m_jvtTool;

      typedef AuxElementMapComparisonOperator<SG::AuxElement::Decorator<char>> AUDecCharComp;
      std::map<SG::AuxElement::Decorator<char>, asg::AnaToolHandle<IAsgElectronLikelihoodTool>, AUDecCharComp> m_electronSelectionTools;
      std::map<SG::AuxElement::Decorator<char>, asg::AnaToolHandle<IAsgPhotonIsEMSelector>, AUDecCharComp> m_photonSelectionTools;
      asg::AnaToolHandle<CP::IEgammaCalibrationAndSmearingTool> m_egammaCalibrationTool;
      std::map<SG::AuxElement::Decorator<char>, asg::AnaToolHandle<CP::IMuonSelectionTool>, AUDecCharComp> m_muonSelectionTools;
      asg::AnaToolHandle<CP::IMuonCalibrationAndSmearingTool> m_muonCalibrationTool;
      std::map<SG::AuxElement::Decorator<char>, asg::AnaToolHandle<TauAnalysisTools::ITauSelectionTool>, AUDecCharComp> m_tauSelectionTools;
      std::map<SG::AuxElement::Decorator<char>, asg::AnaToolHandle<CP::IIsolationSelectionTool>, AUDecCharComp> m_electronIsolationTools;
      std::map<SG::AuxElement::Decorator<char>, asg::AnaToolHandle<CP::IIsolationSelectionTool>, AUDecCharComp> m_muonIsolationTools;
      std::map<SG::AuxElement::Decorator<char>, asg::AnaToolHandle<CP::IIsolationSelectionTool>, AUDecCharComp> m_photonIsolationTools;

      asg::AnaToolHandle<IMETMaker> m_metMaker;

      asg::AnaToolHandle<Trig::ITrigDecisionTool> m_trigDec;
      asg::AnaToolHandle<TrigConf::ITrigConfigTool> m_trigConf;

      //std::vector<ORUtils::ToolBox> m_orTools;
      //std::vector<asg::AnaToolHandle<ORUtils::IOverlapRemovalTool>> m_orTools;
      //std::vector<asg::AnaToolHandle<ORUtils::IOverlapTool>> m_orSubTools; // Maybe needed to make the subtools stick around long enough...
      std::map<std::string, asg::AnaToolHandle<ORUtils::IOverlapRemovalTool>> m_orTools;

};


// define

#endif //> !METTRIGGERTOOLS_METTRIGGERANALYSISTOOL_H
