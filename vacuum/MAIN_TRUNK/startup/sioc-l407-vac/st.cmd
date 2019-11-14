#!../../bin/linux-x86_64/vac

## You may have to change vac to something else
## everywhere it appears in this file

< envPaths


cd ${TOP}

epicsEnvSet("IOC_NAME","HL4:VL407")
## Register all support components
dbLoadDatabase("dbd/vac.dbd")
vac_registerRecordDeviceDriver(pdbbase)


# ====================================================================
# Setup some additional environment variables
# ====================================================================
# Setup environment variables
epicsEnvSet("ENGINEER","Geonyeong Mun")
epicsEnvSet("LOCATION","L407")

# tag log messages with IOC name
# How to escape the "sioc-l407-vac" as the PERL program
# will try to repplace it.
# So, uncomment the following and remove the backslash
#epicsEnvSet("EPICS\_IOC\_LOG_CLIENT_INET","${IOC}")

# ========================================================
# Support Large Arrays/Waveforms; Number in Bytes
# Please calculate the size of the largest waveform
# that you support in your IOC.  Do not just copy numbers
# from other apps.  This will only lead to an exhaustion
# of resources and problems with your IOC.
# The default maximum size for a channel access array is
# 16K bytes.
# ========================================================
#epicsEnvSet("EPICS_CA_MAX_ARRAY_BYTES", "65536")

# END: Additional environment variables
# ====================================================================

########################################################################
# BEGIN: Load the record databases
#######################################################################
# =====================================================================
# Load iocAdmin databases to support IOC Health and monitoring
# =====================================================================
dbLoadRecords("db/iocAdminSoft.db","IOC=${IOC_NAME}")
dbLoadRecords("db/iocAdminScanMon.db","IOC=${IOC_NAME}")

# The following database is a result of a python parser
# which looks at RELEASE_SITE and RELEASE to discover
# versions of software your IOC is referencing
# The python parser is part of iocAdmin
dbLoadRecords("db/iocRelease.db","IOC=${IOC_NAME}")

# =====================================================================
# Load database for autosave status
# =====================================================================
dbLoadRecords("db/save_restoreStatus.db", "P=${IOC_NAME}:")


# =====================================================================
#Load Additional databases:
# =====================================================================
## Load record instances

drvAsynIPPortConfigure("A1","ts-l407-dev1:4007 TCP",0,0,0)
drvAgilent4uhvConfigure("4uhv_1","A1",0x80,2,0)

drvAsynIPPortConfigure("A2","ts-l407-dev1:4008 TCP",0,0,0)
drvAgilent4uhvConfigure("4uhv_2","A2",0x80,2,0)

drvAsynIPPortConfigure("M1","ts-l407-dev1:4005 TCP",0,0,0)
drvMks937bConfigure("mks1","M1",253,2,0)

#asynOctetSetInputEos("M1",0,";FF")
#asynOctetSetOutputEos("M1",0,";FF")

dbLoadTemplate("db/l407_agilent4uhv.substitutions")
dbLoadTemplate("db/l407_mks937b.substitutions")
# END: Loading the record databases
########################################################################

# =====================================================================
## Begin: Setup autosave/restore
# =====================================================================

# ============================================================
# If all PVs don't connect continue anyway
# ============================================================
save_restoreSet_IncompleteSetsOk(1)

# ============================================================
# created save/restore backup files with date string
# useful for recovery.
# ============================================================
save_restoreSet_DatedBackupFiles(1)

# ============================================================
# Where to find the list of PVs to save
# ============================================================
set_requestfile_path("${IOC_DATA}/${IOC}/autosave-req")

# ============================================================
# Where to write the save files that will be used to restore
# ============================================================
set_savefile_path("${IOC_DATA}/${IOC}/autosave")

# ============================================================
# Prefix that is use to update save/restore status database
# records
# ============================================================
save_restoreSet_status_prefix("${IOC_NAME}:")

cd("${IOC_DATA}/${IOC}/autosave-req")
makeAutosaveFiles()

## Restore datasets
set_pass0_restoreFile("info_settings.sav")
set_pass1_restoreFile("info_settings.sav")

# =====================================================================
# End: Setup autosave/restore
# =====================================================================

# =====================================================================
# Channel Access Security:
# This is required if you use caPutLog.
# Set access security file
# Load common LCLS Access Configuration File
< ${ACF_INIT}


cd ${TOP}/startup/${IOC}
iocInit()

# =====================================================
# Turn on caPutLogging:
# Log values only on change to the iocLogServer:
#caPutLogInit(getenv("EPICS_CA_PUT_LOG_ADDR"))
#caPutLogShow(2)
# =====================================================

## Start any sequence programs
#seq sncExample,"user=oneHost"

## Start autosave process:
create_monitor_set("info_settings.req",60,"")

cd("${TOP}")
pwd()
