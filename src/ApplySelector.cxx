// METTriggerTools includes
#include "ApplySelector.h"
#include "METTriggerTools/IMETTriggerEventSelector.h"
#include "xAODEventInfo/EventInfo.h"

//uncomment the line below to use the HistSvc for outputting trees and histograms
//#include "GaudiKernel/ITHistSvc.h"
//#include "TTree.h"
//#include "TH1D.h"



ApplySelector::ApplySelector( const std::string& name, ISvcLocator* pSvcLocator )
  : AthAnalysisAlgorithm( name, pSvcLocator ),
    m_selector("", this)
{
  declareProperty( "Selector", m_selector );
  declareProperty( "Signals", m_signalsToDo );
  declareProperty( "SetFilter", m_setFilter = true ); // if true the filter will be set to false iff none of the signals pass
  declareProperty( "AuxDataPrefix", m_adPrefix = "Pass_" );
  //declareProperty( "Property", m_nProperty ); //example property declaration

}


ApplySelector::~ApplySelector() {}


StatusCode ApplySelector::initialize() {
  ATH_MSG_INFO ("Initializing " << name() << "...");
  //
  //This is called once, before the start of the event loop
  //Retrieves of tools you have configured in the joboptions go here
  //

  //HERE IS AN EXAMPLE
  //We will create a histogram and a ttree and register them to the histsvc
  //Remember to uncomment the configuration of the histsvc stream in the joboptions
  //
  //ServiceHandle<ITHistSvc> histSvc("THistSvc",name());
  //TH1D* myHist = new TH1D("myHist","myHist",10,0,10);
  //CHECK( histSvc->regHist("/MYSTREAM/myHist", myHist) ); //registers histogram to output stream (like SetDirectory in EventLoop)
  //TTree* myTree = new TTree("myTree","myTree");
  //CHECK( histSvc->regTree("/MYSTREAM/SubDirectory/myTree", myTree) ); //registers tree to output stream (like SetDirectory in EventLoop) inside a sub-directory
  ATH_CHECK( m_selector.retrieve() );

  for (const std::string& signal : m_signalsToDo) {
    ATH_CHECK( book(TH1F( (signal + "_cutflow").c_str(), signal.c_str(), 1, 0, 1) ) );
    m_decorators.emplace(signal, SG::AuxElement::Decorator<char>(m_adPrefix + signal) );
  }

  return StatusCode::SUCCESS;
}

StatusCode ApplySelector::finalize() {
  ATH_MSG_INFO ("Finalizing " << name() << "...");
  //
  //Things that happen once at the end of the event loop go here
  //


  return StatusCode::SUCCESS;
}

StatusCode ApplySelector::execute() {  
  ATH_MSG_DEBUG ("Executing " << name() << "...");
  if (m_setFilter) setFilterPassed(false); //optional: start with algorithm not passed

  const xAOD::EventInfo* evtInfo(0);
  ATH_CHECK( evtStore()->retrieve(evtInfo, "EventInfo") );
  ATH_CHECK( m_selector->retrieveObjects() );
  for (const auto& pair : m_decorators) {
    Root::TAccept accept = m_selector->passSelection(pair.first);
    pair.second(*evtInfo) = accept ? true : false;
    m_selector->fillCutflow(accept, hist(pair.first+"_cutflow") );
    if (accept) setFilterPassed(true);
  }
  //setFilterPassed(true); //if got here, assume that means algorithm passed
  return StatusCode::SUCCESS;
}

StatusCode ApplySelector::beginInputFile() { 
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


