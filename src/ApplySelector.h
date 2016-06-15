#ifndef METTRIGGERTOOLS_APPLYSELECTOR_H
#define METTRIGGERTOOLS_APPLYSELECTOR_H 1

#include "AthAnalysisBaseComps/AthAnalysisAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "AthContainers/AuxElement.h"

#include <vector>
#include <string>

class IMETTriggerEventSelector;

class ApplySelector: public ::AthAnalysisAlgorithm { 
public: 
  ApplySelector( const std::string& name, ISvcLocator* pSvcLocator );
  virtual ~ApplySelector(); 

  virtual StatusCode  initialize();
  virtual StatusCode  execute();
  virtual StatusCode  finalize();
  
  virtual StatusCode beginInputFile();

private: 
  std::vector<std::string> m_signalsToDo;
  ToolHandle<IMETTriggerEventSelector> m_selector;
  std::map<std::string, SG::AuxElement::Decorator<char>> m_decorators;
  bool m_setFilter;
  std::string m_adPrefix;
}; 
#endif //> !METTRIGGERTOOLS_APPLYSELECTOR_H
