TOP=../..

include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE
#=============================

#=============================
# Build the IOC application

PROD_IOC = tilt
# tilt.dbd will be created and installed
DBD += tilt.dbd

# tilt.dbd will be made up from these files:
tilt_DBD += base.dbd
tilt_DBD += asyn.dbd
tilt_DBD += stream.dbd
tilt_DBD += drvAsynIPPort.dbd
tilt_DBD += drvAsynSerialPort.dbd
tilt_DBD += asSupport.dbd
tilt_DBD += transformRecord.dbd
tilt_DBD += system.dbd
tilt_DBD += asynRecord.dbd

# Include dbd files from all support applications:
#tilt_DBD += xxx.dbd

# Add all the support libraries needed by this IOC
#tilt_LIBS += xxx
tilt_LIBS += autosave calc stream asyn

# tilt_registerRecordDeviceDriver.cpp derives from tilt.dbd
tilt_SRCS += tilt_registerRecordDeviceDriver.cpp

# Build the main IOC entry point on workstation OSs.
tilt_SRCS_DEFAULT += tiltMain.cpp
tilt_SRCS_vxWorks += -nil-

# Add support from base/src/vxWorks if needed
#tilt_OBJS_vxWorks += $(EPICS_BASE_BIN)/vxComLibrary

# Finally link to the EPICS Base libraries
tilt_LIBS += $(EPICS_BASE_IOC_LIBS)

#===========================

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

