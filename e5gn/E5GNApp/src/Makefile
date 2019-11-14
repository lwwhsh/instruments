TOP=../..

include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE
#=============================

#=============================
# Build the IOC application

PROD_IOC = E5GN
# E5GN.dbd will be created and installed
DBD += E5GN.dbd

# E5GN.dbd will be made up from these files:
E5GN_DBD += base.dbd
E5GN_DBD += asyn.dbd
E5GN_DBD += stream.dbd
E5GN_DBD += drvAsynIPPort.dbd
E5GN_DBD += drvAsynSerialPort.dbd
E5GN_DBD += asSupport.dbd
E5GN_DBD += transformRecord.dbd
E5GN_DBD += system.dbd
E5GN_DBD += asynRecord.dbd

# Include dbd files from all support applications:
#E5GN_DBD += xxx.dbd

# Add all the support libraries needed by this IOC
#E5GN_LIBS += xxx
E5GN_LIBS += autosave calc stream asyn

# E5GN_registerRecordDeviceDriver.cpp derives from E5GN.dbd
E5GN_SRCS += E5GN_registerRecordDeviceDriver.cpp

# Build the main IOC entry point on workstation OSs.
E5GN_SRCS_DEFAULT += E5GNMain.cpp
E5GN_SRCS_vxWorks += -nil-

# Add support from base/src/vxWorks if needed
#E5GN_OBJS_vxWorks += $(EPICS_BASE_BIN)/vxComLibrary

# Finally link to the EPICS Base libraries
E5GN_LIBS += $(EPICS_BASE_IOC_LIBS)

#===========================

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

