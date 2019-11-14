TOP=../..

include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE
#=============================

#==================================================
# Build an IOC support library

LIBRARY_IOC += xxxSupport

# xxxRecord.h will be created from xxxRecord.dbd
DBDINC += xxxRecord
# install devXxxSoft.dbd into <top>/dbd
DBD += xxxSupport.dbd

# The following are compiled and added to the Support library
xxxSupport_SRCS += xxxRecord.c
xxxSupport_SRCS += devXxxSoft.c

xxxSupport_LIBS += $(EPICS_BASE_IOC_LIBS)

#=============================
# build an ioc application

PROD_IOC = temp_gpib
# temp_gpib.dbd will be created and installed
DBD += temp_gpib.dbd

# temp_gpib.dbd will be made up from these files:
temp_gpib_DBD += base.dbd
temp_gpib_DBD += xxxSupport.dbd
temp_gpib_DBD += dbSubExample.dbd
temp_gpib_DBD += temp_gpibHello.dbd
# Added LWW
temp_gpib_DBD += asyn.dbd
temp_gpib_DBD += devtemp_gpib.dbd
temp_gpib_DBD += drvAsynSerialPort.dbd
temp_gpib_DBD += drvAsynIPPort.dbd
# 

# <name>_registerRecordDeviceDriver.cpp will be created from <name>.dbd
temp_gpib_SRCS += temp_gpib_registerRecordDeviceDriver.cpp
temp_gpib_SRCS_DEFAULT += temp_gpibMain.cpp
temp_gpib_SRCS_vxWorks += -nil-

# Add locally compiled object code
temp_gpib_SRCS += dbSubExample.c
temp_gpib_SRCS += temp_gpibHello.c

# The following adds support from base/src/vxWorks
temp_gpib_OBJS_vxWorks += $(EPICS_BASE_BIN)/vxComLibrary

temp_gpib_LIBS += xxxSupport

# NOTE: To build SNL programs, SNCSEQ must be defined
# in the <top>/configure/RELEASE file

ifneq ($(SNCSEQ),)
    # This builds sncExample as a component of temp_gpib
    temp_gpib_SNCFLAGS += +r
    temp_gpib_DBD += sncExample.dbd
    temp_gpib_SRCS += sncExample.stt
    temp_gpib_LIBS += seq pv

    # The following builds sncProgram as a standalone application
    PROD_HOST += sncProgram
    sncProgram_SNCFLAGS += +m
    sncProgram_SRCS += sncProgram.st
    sncProgram_LIBS += seq pv
    sncProgram_LIBS += $(EPICS_BASE_HOST_LIBS)
endif

temp_gpib_LIBS += $(EPICS_BASE_IOC_LIBS)

## Aded LWW
temp_gpib_LIBS += asyn
temp_gpib_LIBS += devtemp_gpib
 
#===========================

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

