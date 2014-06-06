import ROOT,os,sys,getopt

mcEngine  = "TGeant4"
simEngine = "Pythia8"
nEvents   = 100
#simEngine = "Genie"
inclusive = False  # True = all processes if False only ccbar -> HNL
eventDisplay = False

try:
        opts, args = getopt.getopt(sys.argv[1:], "o:D:FHPu:n:x:c:hqv:sl:A",["Pythia6","Pythia8","Genie","nEvents=", "display"])
except getopt.GetoptError:
        # print help information and exit:
        print ' enter --Pythia8/6 to generate events with Pythia8/6 or --Genie for reading and processing neutrino interactions'  
        sys.exit()
for o, a in opts:
        if o in ("--display"):
            eventDisplay = True
        if o in ("--Pythia6"):
            simEngine = "Pythia6"
        if o in ("--Pythia8"):
            simEngine = "Pythia8"
        if o in ("--Genie"):
            simEngine = "Genie"
        if o in ("-n", "--nEvents="):
            nEvents = int(a)

print "FairShip setup for",simEngine,"to produce",nEvents,"events"

#
import shipunit as u
import ShipGeo,shipRoot_conf
shipRoot_conf.configure()

# Output file name
tag = simEngine+"-"+mcEngine
if eventDisplay: tag = tag+'_D'
outFile ="ship."+tag+".root"  
os.system("rm *."+tag+".root")
# Parameter file name
parFile="ship.params."+tag+".root"
 
# In general, the following parts need not be touched
# ========================================================================

# -----Timer--------------------------------------------------------
timer = ROOT.TStopwatch()
timer.Start()
# ------------------------------------------------------------------------

# -----Create simulation run----------------------------------------
run = ROOT.FairRunSim()
run.SetName(mcEngine)# Transport engine
run.SetOutputFile(outFile) # Output file
rtdb = run.GetRuntimeDb() 
# -----Create geometry----------------------------------------------
import shipDet_conf
shipDet_conf.configure(run)
# -----Create PrimaryGenerator--------------------------------------
primGen = ROOT.FairPrimaryGenerator()

if simEngine == "Pythia8":
 primGen.SetTarget(ShipGeo.target.z0, 0.) 
# -----Pythia8--------------------------------------
 P8gen = ROOT.Pythia8Generator()
 import pythia8_conf
 pythia8_conf.configure(P8gen,inclusive)
 primGen.AddGenerator(P8gen)
 if inclusive: 
  # check presence of HNL
  P8gen.GetPythiaInstance(9900014)
if simEngine == "Genie":
# Genie
 pointZero =  -ShipGeo.decayVolume.length/2. - 1.*u.cm  # nu interaction in last 10% of interactionLength of mushield
 # pointZero =   0.  # for testing
 primGen.SetTarget(pointZero, 0.)
 Geniegen = ROOT.GenieGenerator()
 Geniegen.Init('Genie-mu-_anti_nu_mu-gntp.113.gst.root') 
 primGen.AddGenerator(Geniegen)
 nEvents = min(nEvents,Geniegen.GetNevents())
 print 'Generate ',nEvents,' with Genie input'
#  add tungsten to PDG
 pdg = ROOT.TDatabasePDG.Instance()
 pdg.AddParticle('W','Ion', 1.71350e+02, True, 0., 74, 'XXX', 1000741840)
#
 run.SetPythiaDecayer('DecayConfigPy8.C') # this does not work !! It insists of using DecayConfig.C 
 # this requires writing a C macro, would have been easier to do directly in python! 
 # for i in [431,421,411,-431,-421,-411]:
 # ROOT.gMC.SetUserDecay(i) # Force the decay to be done w/external decayer	
run.SetGenerator(primGen)

# ------------------------------------------------------------------------
 
#---Store the visualiztion info of the tracks, this make the output file very large!!
#--- Use it only to display but not for production!
if eventDisplay: run.SetStoreTraj(ROOT.kTRUE)
else:            run.SetStoreTraj(ROOT.kFALSE)
# -----Initialize simulation run------------------------------------
run.Init()
fStack = ROOT.gMC.GetStack()
fStack.SetEnergyCut(100.*u.MeV)
# ------------------------------------------------------------------------
if simEngine != "Genie" :  
# -----Runtime database---------------------------------------------
 kParameterMerged = ROOT.kTRUE
 parOut = ROOT.FairParRootFileIo(kParameterMerged)
 parOut.open(parFile)
 rtdb.setOutput(parOut)
 rtdb.saveOutput()   # for the moment, it blocks when using Genie, no idea why
 rtdb.printParamContexts()
# -----Start run----------------------------------------------------
run.Run(nEvents)
# ------------------------------------------------------------------------
run.CreateGeometryFile("geofile_full."+tag+".root")  
# -----Finish-------------------------------------------------------
timer.Stop()
rtime = timer.RealTime()
ctime = timer.CpuTime()
print ' ' 
print "Macro finished succesfully." 
print "Output file is ",  outFile 
print "Parameter file is ",parFile
print "Real time ",rtime, " s, CPU time ",ctime,"s"

# ------------------------------------------------------------------------