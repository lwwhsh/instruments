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
PROD_IOC = vac

# Use the following for RTEMS Only IOCs
#PROD_IOC_RTEMS = vac

# vac.dbd will be created and installed
DBD += vac.dbd

# vac.dbd will be made up from these files:
vac_DBD += base.dbd
vac_DBD += devIocStats.dbd
vac_DBD += autosaveSupport.dbd
vac_DBD += caPutLog.dbd
vac_DBD += $(CEXP_DBDS)
# ================================================================================
# The system command will be mainly used by linux hosted IOCs
# Very useful for changing kernel thread priorities for kernel drivers
# ================================================================================
vac_DBD += system.dbd

# If you don't have an EVR comment the following two lines:
vac_DBD += evrSupport.dbd
vac_DBD += devMrfEr.dbd

# ======================================================================
# <name>_registerRecordDeviceDriver.cpp will be created from <name>.dbd
vac_SRCS += vac_registerRecordDeviceDriver.cpp

vac_SRCS_DEFAULT += vacMain.cpp

vac_SRCS_vxWorks += -nil-

# ========================================================================
# ========================================================================
# Add locally compiled object code
# For example, subroutines
# ========================================================================
# vac_SRCS += file1.c
# vac_SRCS += file2.c

# asyn
vac_LIBS += asyn
vac_LIBS += agilent4uhv
vac_LIBS += mks937b

vac_DBD += asyn.dbd
vac_DBD += drvAsynIPPort.dbd
vac_DBD += agilent4uhv.dbd
vac_DBD += mks937b.dbd

vac_DBD += stream.dbd
vac_LIBS += stream

# =====================================================
# Link in the code from other libraries
# =====================================================
# Needed to link in iocAdmin support
vac_LIBS += devIocStats

# Needed to link in autosave support
vac_LIBS += autosave

# Needed to link caPutLog support
vac_LIBS += caPutLog

# Needed to link cexpsh() support
vac_LIBS += $(CEXP_LIBS)

# =============================================
# Begin to link in the EVR support for
# RTEMS, linux, and linuxRT
# If you don't have an EVR, you can comment out 
# the lines below.
# =============================================
vac_LIBS += evrSupport
vac_LIBS += devMrfEr
vac_LIBS_RTEMS += mrfVme64x
vac_LIBS_Linux += drvMrf

# ifneq ($(T_A),linuxRT-x86)
#   ifneq ($(T_A),linux-x86)
#     ifneq ($(T_A),linux-x86_64)
#       vac_LIBS += mrfVme64x
#     endif
#   endif
# endif

# ifeq ($(T_A),linuxRT-x86)
#  vac_LIBS += drvMrf
# endif
 
# ifeq ($(T_A),linux-x86)
#  vac_LIBS += drvMrf
# endif

# ifeq ($(T_A),linux-x86_64)
#  vac_LIBS += drvMrf
# endif
# =====================================================
# END EVR libraries
# =====================================================

# NOTE: To build SNL programs, SNCSEQ must be defined
# in the <top>/configure/RELEASE file

ifneq ($(SNCSEQ),)
    # This builds sncExample as a component of vac
    vac_SNCFLAGS += +r
    vac_DBD += sncExample.dbd
    vac_SRCS += sncExample.stt
    vac_LIBS += seq pv

    # The following builds sncProgram as a standalone application
  ifneq ($(T_A),linuxRT-x86)
    PROD_HOST += sncProgram
    sncProgram_SNCFLAGS += +m
    sncProgram_SRCS += sncProgram.st
    sncProgram_LIBS += seq pv
    sncProgram_LIBS += $(EPICS_BASE_HOST_LIBS)
  endif
endif

vac_LIBS += $(EPICS_BASE_IOC_LIBS)

#===========================

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

