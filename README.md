# CMSSW Environment Setup
In the directory where you want to create your CMSSW environment
```bash
cmsrel CMSSW_10_6_24
cd CMSSW_10_6_24/src
cmsenv
git-cms-init
git cms-merge-topic AndreaBellora:CMSSW_10_6_24_LumiAnalysis
scram b -r -j8
```

# Running PPS Lumi analyzer
To run the Lumi analysis:
```bash
cd RecoCTPPS/Lumi/
voms-proxy-init --voms cms --rfc
cp /tmp/x509up_u$UID $CMSSW_BASE/src/RecoCTPPS/Lumi/x509up_u$UID
source LumiAnalysis.sh A $CMSSW_BASE
```

Above is the example that has been set up to process the files listed in data/EraA_forLumi.dat (list obtained from DAS).
LumiAnalysis.sh should be written in a way that allows for HTCondor submission with a .sub file similar to:

```sub
executable          = $ENV(CMSSW_BASE)/src/RecoCTPPS/Lumi/LumiAnalysis.sh
arguments           = A $ENV(CMSSW_BASE)
output              = $ENV(CMSSW_BASE)/src/RecoCTPPS/Lumi/EraA.out
error               = $ENV(CMSSW_BASE)/src/RecoCTPPS/Lumi/EraA.err
log                 = $ENV(CMSSW_BASE)/src/RecoCTPPS/Lumi/EraA.log
+JobFlavour         = "testmatch"
queue
```