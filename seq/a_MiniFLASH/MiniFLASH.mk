#################################################################################
# Attention:           THIS FILE MUST BE IN UNIX Text Format
#
# Makefile for MiniFLASH.dll
#
#################################################################################

all:     release debug i86Release
release: dllRelease
debug:   dllDebug

#************************************************************************
# add myself to dependency list
#************************************************************************
MAKEFILE_CHAIN += $(IDEA_BASE)/n4/pkg/MrServers/MrImaging/seq/a_MiniFLASH/MiniFLASH.mk

#************************************************************************
# Target naming
#************************************************************************
TARGET_NAME = MiniFLASH
LIB_NAME    = MiniFLASH

#************************************************************************
# Sources and destinations
#************************************************************************
SOURCES += $(IDEA_BASE)/n4/pkg/MrServers/MrImaging/seq/a_MiniFLASH/a_MiniFLASH.cpp
SOURCES += $(IDEA_BASE)/n4/pkg/MrServers/MrImaging/seq/a_MiniFLASH/a_MiniFLASH_UI.cpp

#************************************************************************
# Target specific CPPFLAGS
#************************************************************************
CPPFLAGS_LOCAL += -D_CONSOLE
CPPFLAGS_LOCAL += -DBUILD_SEQU
CPPFLAGS_LOCAL += -DSEQUENCE_CLASS
CPPFLAGS_LOCAL += -DSEQUENCE_CLASS_MiniFLASH
CPPFLAGS_LOCAL += -DMRPROT_DEP_OPT
CPPFLAGS_LOCAL += -DSEQ_NAMESPACE=SEQ_MINIFLASH

#************************************************************************
# LIBs
#************************************************************************
LDLIBS += Sequence.lib
LDLIBS += libSBB.lib
LDLIBS += libSSL.lib
LDLIBS += libRT.lib
LDLIBS += SeqBuffer.lib
LDLIBS += libSeqUtil.lib
LDLIBS += libSeqUT.lib
LDLIBS += libUICtrl.lib
LDLIBS += libUILink.lib
LDLIBS += MrProt.lib
LDLIBS += MeasSections.lib
LDLIBS += libGSL.lib
LDLIBS += CoilIF.lib
LDLIBS += MrTrace.lib
LDLIBS += MeasNucleiBase.lib
LDLIBS += libSeqSysProp.lib
LDLIBS += MrParc.lib

#************************************************************************
# INCLUDE General rules for sequences
#************************************************************************
include $(IDEA_BASE)/n4/pkg/MrApplications/MrIDEA/utils/MakeSeq.mk
