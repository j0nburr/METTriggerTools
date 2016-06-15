
#include "GaudiKernel/DeclareFactoryEntries.h"

#include "METTriggerTools/METTriggerAnalysisTool.h"

DECLARE_TOOL_FACTORY( METTriggerAnalysisTool )


#include "METTriggerTools/METTriggerEventSelector.h"
DECLARE_TOOL_FACTORY( METTriggerEventSelector )

#include "METTriggerTools/EventSelectors/WZCommonEventSelector.h"
DECLARE_TOOL_FACTORY( WZCommonEventSelector )

#include "METTriggerTools/EventSelectors/ObjectCounter.h"
DECLARE_TOOL_FACTORY( ObjectCounter )

#include "../GetMETTriggerObjects.h"
DECLARE_ALGORITHM_FACTORY( GetMETTriggerObjects )


#include "../DrawMETTriggerObjects.h"
DECLARE_ALGORITHM_FACTORY( DrawMETTriggerObjects )


#include "../ApplySelector.h"
DECLARE_ALGORITHM_FACTORY( ApplySelector )


#include "../HistDrawer.h"
DECLARE_ALGORITHM_FACTORY( HistDrawer )


#include "METTriggerTools/METTriggerHelper.h"
DECLARE_TOOL_FACTORY( METTriggerHelper )

#include "METTriggerTools/EventSelectors/ObjectCounter.h"
DECLARE_TOOL_FACTORY( ObjectCounter );

DECLARE_FACTORY_ENTRIES( METTriggerTools ) 
{
  DECLARE_TOOL( ObjectCounter );
  DECLARE_TOOL( METTriggerHelper );
  DECLARE_ALGORITHM( HistDrawer );
  DECLARE_ALGORITHM(  ApplySelector );
  DECLARE_ALGORITHM( DrawMETTriggerObjects );
  DECLARE_ALGORITHM( GetMETTriggerObjects );
  DECLARE_TOOL( METTriggerEventSelector );
  DECLARE_TOOL( WZCommonEventSelector );
  DECLARE_TOOL( ObjectCounter );
  DECLARE_TOOL( METTriggerAnalysisTool );
}
