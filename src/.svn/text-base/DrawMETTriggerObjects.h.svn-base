#ifndef METTRIGGERTOOLS_DRAWMETTRIGGEROBJECTS_H
#define METTRIGGERTOOLS_DRAWMETTRIGGEROBJECTS_H 1

//#include "AthAnalysisBaseComps/AthAnalysisAlgorithm.h"
#include "AthenaBaseComps/AthHistogramAlgorithm.h"
#ifdef XAOD_ANALYSIS
#include "AsgTools/AnaToolHandle.h" //use asg::AnaToolHandle instead of regular ToolHandles for full dual-use experience!
#endif



class DrawMETTriggerObjects: public ::AthHistogramAlgorithm { 
 public: 
  DrawMETTriggerObjects( const std::string& name, ISvcLocator* pSvcLocator );
  virtual ~DrawMETTriggerObjects(); 

  virtual StatusCode  initialize();
  virtual StatusCode  execute();
  virtual StatusCode  finalize();
  
  virtual StatusCode beginInputFile();

 private: 
  float GeV;
}; 

#endif //> !METTRIGGERTOOLS_DRAWMETTRIGGEROBJECTS_H
