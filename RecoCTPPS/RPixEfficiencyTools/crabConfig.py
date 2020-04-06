from CRABClient.UserUtilities import config, getUsernameFromSiteDB
config = config()

config.General.requestName = 'ctpps_2017D_pixelefficiency_singleele'
config.General.workArea = '/tmp/abellora/ctpps_2017Dreal_pixelefficiency_singleele'
config.General.transferOutputs = True
config.General.transferLogs = True


config.JobType.pluginName = 'analysis'
config.JobType.psetName = 'test/skimFilesForCRAB.py'

config.Data.inputDataset = '/SingleElectron/Run2017D-09Aug2019_UL2017-v1/AOD'
# '/SingleElectron/Run2017C-17Nov2017-v1/AOD'
config.Data.allowNonValidInputDataset = True # CAREFUL WITH THIS

config.Data.inputDBS = 'global'

config.Data.splitting = 'FileBased'      
# config.Data.splitting = 'Automatic'                                                                                         
config.Data.unitsPerJob = 21                                                                                                      
config.Data.lumiMask = '/eos/project/c/ctpps/Operations/DataExternalConditions/2017/combined_RPIN_CMS_EraD.json'
# config.Data.totalUnits = 10

config.Data.outLFNDirBase = '/store/group/dpg_ctpps/comm_ctpps/2017_PixelEfficiency'
config.Data.publication = False
config.Data.outputDatasetTag = 'ctpps_2017D_pixelefficiency_singleele'

config.Site.storageSite = 'T2_CH_CERN'
config.Site.blacklist = ['T3_UK_London_QMUL','T3_UK_London_RHUL','T2_PL_Swierk','T2_FR_GRIF_IRFU']

config.JobType.allowUndistributedCMSSW = True
