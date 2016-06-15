#Skeleton joboption for a simple analysis job

output = vars().get("output", "output")
theApp.EvtMax=vars().get("nEvents", -1)                                         #says how many events to run over. Set to -1 for all events

import AthenaPoolCnvSvc.ReadAthenaPool                   #sets up reading of POOL files (e.g. xAODs)
svcMgr.EventSelector.InputCollections=["/data/atlas/atlasdata3/burr/xAOD/testFiles/data15_13TeV.00284285.physics_Main.merge.AOD.r7562_p2521/AOD.07687825._003463.pool.root.1"]   #insert your list of input files here

# algseq = CfgMgr.AthSequencer("AthAlgSeq")                #gets the main AthSequencer
algseq = CfgMgr.AthSequencer("MyAlgSeq")                #gets the main AthSequencer
outputLevel = INFO
algseq += CfgMgr.GetMETTriggerObjects("Getter", IsData = vars().get("isData", True), OutputModifier = "Cal", ConfigFiles = ["WZCommon.json", "defaultMetConfig.json"], OutputLevel = outputLevel)                                 #adds an instance of your alg to it

# signal = "Wenu"
# selector = CfgMgr.WZCommonEventSelector(signal + "EventSelector", ConfigFile = "WZCommon.json", StoreGatePrefix = "Cal")
# ToolSvc += selector
# algseq += CfgMgr.ApplySelector(signal
for signal in ["Wenu", "Wmunu", "Zmumu"]:
# for signal in []:
  sigseq = CfgMgr.AthSequencer(signal+"Seq")
  selector = CfgMgr.WZCommonEventSelector(signal+"EventSelector", ConfigFile = "WZCommon.json", StoreGatePrefix = "Cal")
  sigseq += CfgMgr.ApplySelector(signal+"Selector", Selector = selector, Signals = [signal], RootStreamName = "CUTFLOW", RootDirName = signal)
  ToolSvc += selector
  sigseq += CfgMgr.HistDrawer(signal + "Drawer", StoreGatePrefix = "Cal", RootStreamName = "HIST", RootDirName = signal, OutputLevel = outputLevel)

  algseq += sigseq

masterseq = CfgMgr.AthSequencer("AthAlgSeq")
masterseq += algseq

##--------------------------------------------------------------------
## This section shows up to set up a HistSvc output stream for outputing histograms and trees
## See https://twiki.cern.ch/twiki/bin/viewauth/AtlasProtected/AthAnalysisBase#How_to_output_trees_and_histogra for more details and examples

if not hasattr(svcMgr, 'THistSvc'): svcMgr += CfgMgr.THistSvc() #only add the histogram service if not already present (will be the case in this jobo)
svcMgr.THistSvc.Output += ["CUTFLOW DATAFILE='"+output+"_cutflows.root' OPT='RECREATE'"] #add an output root file stream
svcMgr.THistSvc.Output += ["HIST DATAFILE='"+output+"_hists.root' OPT='RECREATE'"] #add an output root file stream


##--------------------------------------------------------------------


include("AthAnalysisBaseComps/SuppressLogging.py")              #Optional include to suppress as much athena output as possible. Keep at bottom of joboptions so that it doesn't suppress the logging of the things you have configured above
MessageSvc.Format = "% F%50W%S%7W%R%T %0W%M"
MessageSvc.defaultLimit = 9999999999
