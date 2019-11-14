TOP=../..

include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE
#========================================

LIBRARY_IOC = agilent4uhv

DBD += agilent4uhv.dbd

agilent4uhv_SRCS_DEFAULT += agilent4uhvMain.cpp

agilent4uhv_SRCS += drvAgilent4uhv.cpp

agilent4uhv_LIBS += $(EPICS_BASE_IOC_LIBS)

INC += drvAgilent4uhv.h

#===========================

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

