TOP=../..

include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE
#========================================

# ================================================
# Build the IOC application
# ================================================
# This will attempt to build an IOC application 
# for every target including the host computer.
PROD_IOC = test

# Use the following for RTEMS Only IOCs
#PROD_IOC_RTEMS = test

# test.dbd will be created and installed
DBD += test.dbd

# test.dbd will be made up from these files:
test_DBD += base.dbd
test_DBD += devIocStats.dbd
test_DBD += autosaveSupport.dbd
test_DBD += caPutLog.dbd
test_DBD += $(CEXP_DBDS)
# ================================================================================
# The system command will be mainly used by linux hosted IOCs
# Very useful for changing kernel thread priorities for kernel drivers
# ================================================================================
test_DBD += system.dbd

# If you don't have an EVR comment the following two lines:
test_DBD += evrSupport.dbd
test_DBD += devMrfEr.dbd

# ======================================================================
# <name>_registerRecordDeviceDriver.cpp will be created from <name>.dbd
test_SRCS += test_registerRecordDeviceDriver.cpp

test_SRCS_DEFAULT += testMain.cpp

test_SRCS_vxWorks += -nil-

# ========================================================================
# ========================================================================
# Add locally compiled object code
# For example, subroutines
# ========================================================================
# test_SRCS += file1.c
# test_SRCS += file2.c

test_LIBS += asyn agilent4uhv
test_DBD += asyn.dbd drvAsynIPPort.dbd agilent4uhv.dbd

# =====================================================
# Link in the code from other libraries
# =====================================================
# Needed to link in iocAdmin support
test_LIBS += devIocStats

# Needed to link in autosave support
test_LIBS += autosave

# Needed to link caPutLog support
test_LIBS += caPutLog

# Needed to link cexpsh() support
test_LIBS += $(CEXP_LIBS)

# =============================================
# Begin to link in the EVR support for
# RTEMS, linux, and linuxRT
# If you don't have an EVR, you can comment out 
# the lines below.
# =============================================
test_LIBS += evrSupport
test_LIBS += devMrfEr
test_LIBS_RTEMS += mrfVme64x
test_LIBS_Linux += drvMrf

# ifneq ($(T_A),linuxRT-x86)
#   ifneq ($(T_A),linux-x86)
#     ifneq ($(T_A),linux-x86_64)
#       test_LIBS += mrfVme64x
#     endif
#   endif
# endif

# ifeq ($(T_A),linuxRT-x86)
#  test_LIBS += drvMrf
# endif
 
# ifeq ($(T_A),linux-x86)
#  test_LIBS += drvMrf
# endif

# ifeq ($(T_A),linux-x86_64)
#  test_LIBS += drvMrf
# endif
# =====================================================
# END EVR libraries
# =====================================================

# NOTE: To build SNL programs, SNCSEQ must be defined
# in the <top>/configure/RELEASE file

ifneq ($(SNCSEQ),)
    # This builds sncExample as a component of test
    test_SNCFLAGS += +r
    test_DBD += sncExample.dbd
    test_SRCS += sncExample.stt
    test_LIBS += seq pv

    # The following builds sncProgram as a standalone application
  ifneq ($(T_A),linuxRT-x86)
    PROD_HOST += sncProgram
    sncProgram_SNCFLAGS += +m
    sncProgram_SRCS += sncProgram.st
    sncProgram_LIBS += seq pv
    sncProgram_LIBS += $(EPICS_BASE_HOST_LIBS)
  endif
endif

test_LIBS += $(EPICS_BASE_IOC_LIBS)

#===========================

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

