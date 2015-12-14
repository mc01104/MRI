CC=g++
CFLAGS=-O3 -g -fvisibility=hidden --sysroot=/vobs/MR_Extern_GCC/GCC4.4.3/amd64_linux/ -fmessage-length=0 -fdiagnostics-show-location=once -fPIC -pipe -m64 -W -Wall -Wno-uninitialized -Wundef -Wpointer-arith -Wwrite-strings -Wconversion -Wsign-compare -Wmissing-noreturn -Wmissing-format-attribute
CDEFINES=-DNDEBUG -D_REENTRANT -DACE_HAS_AIO_CALLS -DACE_HAS_EXCEPTIONS -DBUILD_PLATFORM_LINUX -DMRVISTA -DPARC_MODULE_NAME=MiniFLASH -DMR_MAIN_PKG=MrImaging -D_CONSOLE -DBUILD_SEQU -DSEQUENCE_CLASS -DSEQUENCE_CLASS_MiniFLASH -DMRPROT_DEP_OPT -DSEQ_NAMESPACE=SEQ_MINIFLASH -DBUILD_MiniFLASH
INCLUDES=-I/vobs/n4/pkg -I/vobs/n4/interfaces/deli/n4/pkg -I/vobs/MR/public -I/vobs/n4/interfaces/deli/MR/public -I/vobs/n4/linux/prod/include -I/vobs/n4/x86/prod/include -I/vobs/n4/linux/delivery/include -I/vobs/n4/x86/delivery/include -I/vobs/n4/x86/extsw/include -I/vobs/MR_N4/targets/x64-WinNT5.02/Debug/DoBin/bin/extern -I/vobs/n4/x86/extsw/MedCom/include -I/vobs/n4_extsw/x86/extsw/MedCom/include -I/vobs/n4/pkg/MrServers/MrImaging/seq/a_MiniFLASH -I/vobs/n4/linux/prod/release/share/MrServers/MrImaging/seq/a_MiniFLASH
LDFLAGS=-shared --sysroot=/vobs/MR_Extern_GCC/GCC4.4.3/amd64_linux/ -Wl,--no-undefined
LDPATHS=-L/vobs/MR_BaseSystem_01_REL/targets/ace/linux/Release/bin/public -L/vobs/MR_BaseSystem_01_REL/targets/ace/linux -L/vobs/n4/linux/prod/lib -L/vobs/n4/linux/delivery/lib -L/vobs/n4/linux/extsw/lib
LDLIBS=-lpthread -lSequence -llibSBB -llibSSL -llibRT -lSeqBuffer -llibSeqUtil -llibSeqUT -llibUICtrl -lMrProt -lMeasSections -llibGSL -lCoilIF -lMrTrace -lMeasNucleiBase -llibSeqSysProp -lMrParc
SRCROOT=/vobs/n4/pkg
DOPATH=/vobs/n4/linux/prod/release/share
LIBPATH=/vobs/n4/linux/prod/lib/
SOURCES=/vobs/n4/pkg/MrServers/MrImaging/seq/a_MiniFLASH/a_MiniFLASH.cpp /vobs/n4/pkg/MrServers/MrImaging/seq/a_MiniFLASH/a_MiniFLASH_UI.cpp
OBJECTS=$(subst $(SRCROOT),$(DOPATH),$(SOURCES:.cpp=.o))
LIBRARY=libMiniFLASH.so
PATHENV=PATH=/vobs/MR_Extern_GCC/GCC4.4.3/amd64_linux/usr/lib/gcc/x86_64-linux-gnu/4.4:$(PATH)

$(LIBPATH)$(LIBRARY): $(OBJECTS)
	$(PATHENV) $(CC) $(LDFLAGS) $(LDPATHS) $(LDLIBS) $(OBJECTS) -o $@

$(OBJECTS): $(DOPATH)%.o: $(SRCROOT)%.cpp
	$(PATHENV) $(CC) $(CFLAGS) $(CDEFINES) $(INCLUDES) -c $< -o $@

$(LIBRARY): $(LIBPATH)$(LIBRARY)

clean:
	$(RM) $(OBJECTS)
	$(RM) $(LIBPATH)$(LIBRARY)
