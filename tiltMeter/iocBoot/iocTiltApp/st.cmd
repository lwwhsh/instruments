#!../../bin/linux-x86_64/tilt

## You may have to change Tilt to something else
## everywhere it appears in this file

< envPaths
epicsEnvSet("P", "BL6A:")
epicsEnvSet("R", "PGM:")

epicsEnvSet "STREAM_PROTOCOL_PATH" "$(TOP)/db"

cd "${TOP}"

## Register all support components
dbLoadDatabase "dbd/tilt.dbd"
tilt_registerRecordDeviceDriver pdbbase

#---------MOXA Port 4 use Tilt temp sensors--use RS485-----------
# drvAsynIPPortConfigure("L4","192.168.127.254:4003",0,0,0)
drvAsynIPPortConfigure("L4","192.168.127.254:4004")
asynOctetSetInputEos("L4", -1, "\n") 
asynOctetSetOutputEos("L4", -1, "\n") 
#
# asynSetTraceIOMask("L4",-1,0x2)
# asynSetTraceMask("L4",-1,0x9)

## Load record instances
dbLoadRecords("db/asynRecord.db","P=$(P), R=asyn, PORT=L4,ADDR=-1,OMAX=50,IMAX=50")
dbLoadRecords("db/devTilt.db","P=$(P), R=$(R), PORT=L4")

cd "${TOP}/iocBoot/${IOC}"
iocInit

## Start any sequence programs
#seq sncxxx,"user=rootHost"
