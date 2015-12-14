#################################################################################
# Attention:           THIS FILE MUST BE IN UNIX Text Format
#
# Makefile for FLASH.dll
#
#################################################################################

all:     release debug i86Release
release: dllRelease
debug:   dllDebug

#************************************************************************
# add myself to dependency list
#************************************************************************
MAKEFILE_CHAIN += $(IDEA_BASE)/n4/pkg/MrServers/MrImaging/seq/FLASH/FLASH.mk

#************************************************************************
# Target naming
#************************************************************************
TARGET_NAME = FLASH
LIB_NAME    = FLASH

#************************************************************************
# Sources and destinations
#************************************************************************
SOURCES += $(IDEA_BASE)/n4/pkg/MrServers/MrImaging/seq/FLASH/FISPKernel.cpp
SOURCES += $(IDEA_BASE)/n4/pkg/MrServers/MrImaging/seq/FLASH/FLASHSampling.cpp
SOURCES += $(IDEA_BASE)/n4/pkg/MrServers/MrImaging/seq/FLASH/FLASHSamplingConfigurator.cpp
SOURCES += $(IDEA_BASE)/n4/pkg/MrServers/MrImaging/seq/FLASH/FLASH.cpp
SOURCES += $(IDEA_BASE)/n4/pkg/MrServers/MrImaging/seq/FLASH/FLASH_UI.cpp
SOURCES += $(IDEA_BASE)/n4/pkg/MrServers/MrImaging/seq/FLASH/../common/WIPParameterTool/WIPParameterTool.cpp

#************************************************************************
# Target specific CPPFLAGS
#************************************************************************
CPPFLAGS_LOCAL += -D_CONSOLE
CPPFLAGS_LOCAL += -DBUILD_SEQU
CPPFLAGS_LOCAL += -DSEQUENCE_CLASS_FLASH
CPPFLAGS_LOCAL += -DSEQ_NAMESPACE=SEQ_FLASH
CPPFLAGS_LOCAL += -DMRPROT_DEP_OPT
CPPFLAGS_LOCAL += -DTRACE_COMPONENT_NAME=MrImaging
CPPFLAGS_LOCAL += -DWPT_NAMESPACE=FLASH_WPT
CPPFLAGS_LOCAL += -DBUILD_WIPParameterTool

#************************************************************************
# LIBs
#************************************************************************
LDLIBS += libRT.lib
LDLIBS += libSBB.lib
LDLIBS += libCSL.lib
LDLIBS += libSL.lib
LDLIBS += libSeqSysProp.lib
LDLIBS += libSeqUT.lib
LDLIBS += libSeqUtil.lib
LDLIBS += libGSL.lib
LDLIBS += libSSL.lib
LDLIBS += libUICtrl.lib
LDLIBS += MeasNucleiBase.lib
LDLIBS += MeasNucleiIF.lib
LDLIBS += MrGenericDC.lib
LDLIBS += MrParc.lib
LDLIBS += MrProt.lib
LDLIBS += MrProtocolData.lib
LDLIBS += MrTrace.lib
LDLIBS += UTrace.lib
LDLIBS += SeqBuffer.lib
LDLIBS += Sequence.lib
LDLIBS += libMath.lib
LDLIBS += libKSpace.lib
LDLIBS += libKSA.lib
LDLIBS += libUILink.lib

#************************************************************************
# INCLUDE General rules for sequences
#************************************************************************
include $(IDEA_BASE)/n4/pkg/MrApplications/MrIDEA/utils/MakeSeq.mk
