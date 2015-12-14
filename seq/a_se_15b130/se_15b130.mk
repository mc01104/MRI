#################################################################################
# Attention:           THIS FILE MUST BE IN UNIX Text Format
#
# Makefile for se_15b130.dll
#
#################################################################################

all:     release debug i86Release
release: dllRelease
debug:   dllDebug

#************************************************************************
# add myself to dependency list
#************************************************************************
MAKEFILE_CHAIN += $(IDEA_BASE)/n4/pkg/MrServers/MrImaging/seq/a_se_15b130/se_15b130.mk

#************************************************************************
# Target naming
#************************************************************************
TARGET_NAME = se_15b130
LIB_NAME    = se_15b130

#************************************************************************
# Sources and destinations
#************************************************************************
SOURCES += $(IDEA_BASE)/n4/pkg/MrServers/MrImaging/seq/a_se_15b130/a_se_b130_15000.cpp
SOURCES += $(IDEA_BASE)/n4/pkg/MrServers/MrImaging/seq/a_se_15b130/a_se_b130_15000_UI.cpp

#************************************************************************
# Target specific CPPFLAGS
#************************************************************************
CPPFLAGS_LOCAL += -DSEQUENCE_CLASS_SE15B130
CPPFLAGS_LOCAL += -DSEQ_NAMESPACE=SE_15B130
CPPFLAGS_LOCAL += -DBUILD_SEQU
CPPFLAGS_LOCAL += -D_CONSOLE

#************************************************************************
# LIBs
#************************************************************************
LDLIBS += MrTrace.lib
LDLIBS += MrGenericDC.lib
LDLIBS += MrParc.lib
LDLIBS += MrProt.lib
LDLIBS += libSeqSysProp.lib
LDLIBS += MeasSections.lib
LDLIBS += Sequence.lib
LDLIBS += libMES.lib
LDLIBS += MeasNucleiBase.lib
LDLIBS += libSSL.lib
LDLIBS += libRT.lib
LDLIBS += libSBB.lib
LDLIBS += libSeqUtil.lib
LDLIBS += libSeqUT.lib
LDLIBS += libUICtrl.lib
LDLIBS += libUILink.lib

#************************************************************************
# INCLUDE General rules for sequences
#************************************************************************
include $(IDEA_BASE)/n4/pkg/MrApplications/MrIDEA/utils/MakeSeq.mk
