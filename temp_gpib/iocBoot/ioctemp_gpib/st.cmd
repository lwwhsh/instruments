#!../../bin/linux-x86_64/temp_gpib
#
< envPaths
#
cd ${TOP}
#
## Register all support components
dbLoadDatabase("dbd/temp_gpib.dbd",0,0)
temp_gpib_registerRecordDeviceDriver(pdbbase)
#
## Load record instances
dbLoadRecords("db/devtemp_gpib.db","P=BL6A:PGM:, R=TEMP1, L=10,A=1")
dbLoadRecords("db/devtemp_gpib.db","P=BL6A:PGM:, R=TEMP2, L=10,A=2")
dbLoadRecords("db/devtemp_gpib.db","P=BL6A:PGM:, R=TEMP3, L=10,A=3")
dbLoadRecords("db/devtemp_gpib.db","P=BL6A:PGM:, R=TEMP4, L=10,A=4")

#
#==================================================================================
# Serial Port Settings for BL6A MPK PGM TEMPERATURE MEASSUREMENT MODEL E5GN.
#	Buad Rate 		: 9600 bps.
#	Parity	  		: NONE.
#	Data Bit  		: 8 Bit.
#	Stop Bit  		: 1 Bit.
#	Flow Control 	: NONE.
#==================================================================================
#
#Using TCP/IP(Serial to Ethernet converter ex:MOXA5650) Port:4003= E5GN Temperature controller(DCM) test
drvAsynIPPortConfigure("L10","192.168.127.254:4003",0,0,0)
asynOctetConnect("L10", "L10")
asynOctetSetInputEos("L10",0,"\r")
asynOctetSetOutputEos("L10",0,"\r")
#
#
## if you want debug mode enable folow two Line
asynSetTraceMask("L10",-1,0x9)
asynSetTraceIOMask("L10",-1,0x2)
#
#
cd ${TOP}/iocBoot/${IOC}
iocInit()

## Start any sequence programs
#seq sncxxx,"user=rootHost"
#
