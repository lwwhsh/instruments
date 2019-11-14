#!../../bin/linux-x86_64/E5GN

## You may have to change E5GN to something else
## everywhere it appears in this file

< envPaths
epicsEnvSet("P", "BL6A:")
epicsEnvSet("R", "PGM:")

epicsEnvSet "STREAM_PROTOCOL_PATH" "$(TOP)/db"

cd "${TOP}"

## Register all support components
dbLoadDatabase "dbd/E5GN.dbd"
E5GN_registerRecordDeviceDriver pdbbase

#---------MOXA Port 2 use E5GN temp sensors--use RS485-----------
# drvAsynIPPortConfigure("L0","192.168.127.254:4003",0,0,0)
drvAsynIPPortConfigure("L0","192.168.127.254:4003")
asynOctetSetInputEos("L0", -1, "\r") 
asynOctetSetOutputEos("L0", -1, "\r") 
# asynSetTraceIOMask("L0",-1,0x2)
# asynSetTraceMask("L0",-1,0x9)

## Load record instances
dbLoadRecords("db/asynRecord.db","P=$(P), R=asyn, PORT=L0,ADDR=-1,OMAX=50,IMAX=50")
dbLoadRecords("db/devE5GN.db","P=$(P), R=$(R), PORT=L0")

cd "${TOP}/iocBoot/${IOC}"
iocInit

## Start any sequence programs
#seq sncxxx,"user=rootHost"
