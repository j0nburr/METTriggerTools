#ifndef METTRIGGERTOOLS_HISTDRAWER_H
#define METTRIGGERTOOLS_HISTDRAWER_H 1

#include "AthAnalysisBaseComps/AthAnalysisAlgorithm.h"
#ifdef XAOD_ANALYSIS
#include "AsgTools/AnaToolHandle.h" //use asg::AnaToolHandle instead of regular ToolHandles for full dual-use experience!
#endif
#include "METTriggerTools/IMETTriggerHelper.h"
#include <vector>

class HistDrawer: public ::AthAnalysisAlgorithm { 
 public: 
  HistDrawer( const std::string& name, ISvcLocator* pSvcLocator );
  virtual ~HistDrawer(); 

  virtual StatusCode  initialize();
  virtual StatusCode  execute();
  virtual StatusCode  finalize();
  
  virtual StatusCode beginInputFile();

 private: 
  std::string m_sgPrefix;
  std::string m_metTerm;
  asg::AnaToolHandle<IMETTriggerHelper> m_helper;
  std::vector<HLTMET> m_metTypes;
}; 

#endif //> !METTRIGGERTOOLS_HISTDRAWER_H
