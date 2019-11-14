TOP=..
include $(TOP)/configure/CONFIG
#=======================================

# Build devtemp_gpib as a library for an IOC:
LIBRARY_IOC += devtemp_gpib

# Library Source files
devtemp_gpib_SRCS += devtemp_gpib.c

# Link with the asyn and base libraries
devtemp_gpib_LIBS += asyn
devtemp_gpib_LIBS += $(EPICS_BASE_IOC_LIBS)

# Install .dbd and .db files
DBD += devtemp_gpib.dbd
### DB_INSTALLS += devtemp_gpib.db

#=======================================
include $(TOP)/configure/RULES
