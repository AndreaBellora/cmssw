from CRABClient.UserUtilities import config, getUsernameFromSiteDB
config = config()

config.General.requestName = 'ctpps_2018D_pixelefficiency_EGamma'
config.General.workArea = '/tmp/abellora/ctpps_2018Dreal_pixelefficiency_singleele'
config.General.transferOutputs = True
config.General.transferLogs = True


config.JobType.pluginName = 'analysis'
config.JobType.psetName = 'test/ReRecoForCRAB.py'

config.Data.inputDataset = '/EGamma/Run2018D-22Jan2019-v2/AOD'

config.Data.inputDBS = 'global'

config.Data.splitting = 'FileBased'      
# config.Data.splitting = 'Automatic'                                                                                         
config.Data.unitsPerJob = 21                                                                                                      
config.Data.lumiMask = '/eos/project/c/ctpps/Operations/DataExternalConditions/2018/CMSgolden_2RPGood_anyarms_EraD.json'
# config.Data.totalUnits = 10

config.Data.outLFNDirBase = '/store/group/dpg_ctpps/comm_ctpps/2018_PixelEfficiency'
config.Data.publication = False
config.Data.outputDatasetTag = 'ctpps_2018D_pixelefficiency_singleele'

config.Site.storageSite = 'T2_CH_CERN'
config.Site.blacklist = ['T3_UK_London_QMUL','T3_UK_London_RHUL','T2_PL_Swierk','T2_FR_GRIF_IRFU']
