#!../../bin/linux-x86_64/test

## You may have to change test to something else
## everywhere it appears in this file

< envPaths


cd ${TOP}

## Register all support components
dbLoadDatabase("dbd/test.dbd")
test_registerRecordDeviceDriver(pdbbase)

drvAsynIPPortConfigure("A1","ts-gun-dev1:4007 TCP",0,0,1)
drvAgilent4uhvConfigure("4uhv","A1",0x80,0.2,0)

#asynSetTraceIOMask("A1",0,2)
#asynSetTraceMask("A1",0,255)

dbLoadRecords("$(AGILENT4UHV)/db/agilent4uhv.db","LOCA=INJ:GUN,DEV=VAC,PORT=4uhv,ADDR=0,TIMEOUT=0.2")



cd ${TOP}/startup/${IOC}
iocInit()

