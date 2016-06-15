// METTriggerTools includes
#include "DrawMETTriggerObjects.h"

#include "xAODMissingET/MissingETContainer.h"
#include "xAODTrigger/EnergySumRoI.h"
#include "xAODEventInfo/EventInfo.h"

  DrawMETTriggerObjects::DrawMETTriggerObjects( const std::string& name, ISvcLocator* pSvcLocator )
: AthHistogramAlgorithm( name, pSvcLocator ),
  GeV(0.001)
{

  //declareProperty( "Property", m_nProperty ); //example property declaration

}


DrawMETTriggerObjects::~DrawMETTriggerObjects() {}


StatusCode DrawMETTriggerObjects::initialize() {
  ATH_MSG_INFO ("Initializing " << name() << "...");
  //
  //This is called once, before the start of the event loop
  //Retrieves of tools you have configured in the joboptions go here
  //

  // Make histograms
  //ATH_CHECK( book(TH2F("h_L1_v_off", ";E_{T}^{miss} (offline, no muons) [GeV]; E_{T}^{miss} (L1)", 400, 0, 400, 400, 0, 400) ) );
  bookGetPointer(TH2F("h_L1_v_off", ";E_{T}^{miss} (offline, no muons) [GeV]; E_{T}^{miss} (L1)", 400, 0, 400, 400, 0, 400) )->Sumw2();
  //ATH_CHECK( book(TH2F("h_L1x_v_offx", ";E_{x}^{miss} (offline, no muons) [GeV]; E_{x}^{miss} (L1)", 300, -300, 300, 300, -300, 300) ) );
  bookGetPointer(TH2F("h_L1x_v_offx", ";E_{x}^{miss} (offline, no muons) [GeV]; E_{x}^{miss} (L1)", 300, -300, 300, 300, -300, 300) )->Sumw2();
  //ATH_CHECK( book(TH2F("h_L1y_v_offy", ";E_{y}^{miss} (offline, no muons) [GeV]; E_{y}^{miss} (L1)", 300, -300, 300, 300, -300, 300) ) );
  bookGetPointer(TH2F("h_L1y_v_offy", ";E_{y}^{miss} (offline, no muons) [GeV]; E_{y}^{miss} (L1)", 300, -300, 300, 300, -300, 300) )->Sumw2();

  return StatusCode::SUCCESS;
}

StatusCode DrawMETTriggerObjects::finalize() {
  ATH_MSG_INFO ("Finalizing " << name() << "...");
  //
  //Things that happen once at the end of the event loop go here
  //


  return StatusCode::SUCCESS;
}

StatusCode DrawMETTriggerObjects::execute() {  
  ATH_MSG_DEBUG ("Executing " << name() << "...");

  const xAOD::MissingETContainer* met;
  ATH_CHECK( evtStore()->retrieve(met, "CalEtMissNoMu") );

  auto metItr = met->find("FinalTrk");
  if (metItr == met->end() ) {
    ATH_MSG_ERROR( "CalEtMissNoMu container doesn't contain 'FinalTrk'!" );
    return StatusCode::FAILURE;
  }
  float offMetX = (*metItr)->mpx() * GeV;
  float offMetY = (*metItr)->mpy() * GeV;
  float offMet = (*metItr)->met() * GeV;

  const xAOD::EnergySumRoI* l1Cont;
  ATH_CHECK( evtStore()->retrieve(l1Cont, "LVL1EnergySumRoI") );
  float L1MetX = l1Cont->exMiss() * GeV;
  float L1MetY = l1Cont->eyMiss() * GeV;
  float L1Met = sqrt(L1MetX*L1MetX + L1MetY*L1MetY);

  const xAOD::EventInfo* evtInfo;
  ATH_CHECK( evtStore()->retrieve(evtInfo, "EventInfo") );
  float evtWeight = 1.;
  if (evtInfo->eventType(xAOD::EventInfo::IS_SIMULATION) ) {
    evtWeight *= evtInfo->mcEventWeights().at(0);
  }

  hist2d("h_L1_v_off")->Fill(offMet, L1Met, evtWeight);
  hist2d("h_L1x_v_offx")->Fill(offMetX, L1MetX, evtWeight);
  hist2d("h_L1y_v_offy")->Fill(offMetY, L1MetY, evtWeight);

  return StatusCode::SUCCESS;
}

StatusCode DrawMETTriggerObjects::beginInputFile() { 
  //
  //This method is called at the start of each input file, even if
  //the input file contains no events. Accumulate metadata information here
  //

  //example of retrieval of CutBookkeepers: (remember you will need to include the necessary header files and use statements in requirements file)
  // const xAOD::CutBookkeeperContainer* bks = 0;
  // CHECK( inputMetaStore()->retrieve(bks, "CutBookkeepers") );

  //example of IOVMetaData retrieval (see https://twiki.cern.ch/twiki/bin/viewauth/AtlasProtected/AthAnalysisBase#How_to_access_file_metadata_in_C)
  //float beamEnergy(0); CHECK( retrieveMetadata("/TagInfo","beam_energy",beamEnergy) );
  //std::vector<float> bunchPattern; CHECK( retrieveMetadata("/Digitiation/Parameters","BeamIntensityPattern",bunchPattern) );



  return StatusCode::SUCCESS;
}


