####################################################
#                                                  #
# Copyright (c) 2000 by Michal 'Khorne' Rzechonek  #
# Copyright (c) 2006 by Seweryn Habdank-Wojewodzki #
#                                                  #
####################################################

# Target binaries
PROGS=knnl_demo

# Source files' extension
EXT=cpp

# Compiler
GCC=g++
 
## Libraries to link against
## example:
LIBS+=-lboost_program_options-mt

## Local include paths
INCLUDE=-I.

## Linker flags
LFLAGS+= -z combreloc 
#LFLAGS+= -static

define debug_core_info
	## Debugging flags
	GFLAGS+= -pg
	GFLAGS+= -ggdb3
	GFLAGS+= -fprofile-arcs
	GFLAGS+= -ftest-coverage
	## To add "press any key section" to work with:
	## pmap `pgrep ${PROGS}
	GFLAGS += -DPMAP
endef

define tdebug_info
	#GFLAGS+= -DFTDEBUG
	#GFLAGS+= -DTDEBUG
endef

define release_info
	CFLAGS+= -fomit-frame-pointer # incompatibiles with -pg
	## To erase "assert"
	# GFLAGS += -DNDEBUG
endef

define core_info
	## Compilation flags
	CFLAGS+= -pedantic -ansi 
	#CFLAGS+= -Wconversion -Wpointer-arith 
	#CFLAGS+= -Wcast-align
	#CFLAGS+= -Wwrite-strings 
	CFLAGS+= -pipe 
	CFLAGS+= -O3
	#CFLAGS+= -mtune=i686
	#CFLAGS+= -ffloat-store
	CFLAGS+= -finline-functions
	CFLAGS+= -funroll-all-loops
	#CFLAGS+= -ftree-vectorize 
	#CFLAGS+= -ftree-vectorizer-verbose=5
	CFLAGS+= -fno-strict-aliasing 
	CFLAGS+= -fPIC
	CFLAGS+= -msse
endef

CFLAGS+= -Wall 

release:
	@ echo ""
	@ echo '----------------------------------=#=----------------------------------'
	@ echo ""
	@ echo '                           RELEASE VERSION'
	@ echo ""
	@ echo '----------------------------------=#=----------------------------------'
	@ echo ""
	@ $(eval $(call release_info))
	@ $(eval $(call core_info))
	@ $(MAKE) program "CFLAGS=${CFLAGS}"

tdrelease:
	@ echo ""
	@ echo '----------------------------------=#=----------------------------------'
	@ echo ""
	@ echo '                      RELEASE VERSION WITH LOGGER'
	@ echo ""
	@ echo '----------------------------------=#=----------------------------------'
	@ echo ""
	@ $(eval $(call tdebug_info))
	@ $(eval $(call release_info))
	@ $(eval $(call core_info))
	@ $(MAKE) program "CFLAGS=${CFLAGS}" "GFLAGS=${GFLAGS}"

debug:
	@ echo ""
	@ echo '----------------------------------=#=----------------------------------'
	@ echo ""
	@ echo '                      DEBUG VERSION WITH LOGGER'
	@ echo ""
	@ echo '----------------------------------=#=----------------------------------'
	@ echo ""
	@ $(eval $(call tdebug_info))
	@ $(eval $(call debug_core_info))
	@ $(eval $(call core_info))
	@ $(MAKE) program "CFLAGS=${CFLAGS}" "GFLAGS=${GFLAGS}"

######################################
# You shouldn't need to modify these #
######################################

CC=${GCC} ${INCLUDE} ${CFLAGS} ${GFLAGS}
LINK=${GCC} ${GFLAGS} ${LFLAGS}
GENDEP=${GCC} ${GFLAGS} ${CFLAGS} -MM ${INCLUDE} 

OBJS:=../../source/main.o ../../source/configuration.o
DEPS:=../../source/main.d ../../source/configuration.d

.PHONY: program
program: ${PROGS}
	@ echo ""
	@ echo '----------------------------------=#=----------------------------------'
	@ echo ""
	@ echo 'Build complete'
	@ echo LFLAGS = ${LFLAGS}
	@ echo CFLAGS = ${CFLAGS}
	@ echo GFLAGS = ${GFLAGS}
	@ echo LIBS = ${LIBS}
	@ echo ""
	@ echo '----------------------------------=#=----------------------------------'
	@ echo ""

.%.d: %.${EXT}
	@ echo DEP $<
	@ ${GENDEP} $< | sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' > $@

${PROGS}: ${DEP} ${OBJS}
	@ echo LINK $@ 
	@ ${LINK} ${OBJS} ${LIBS} -o $@.debug
	@ strip -R .comment -R .note -R .note.ABI-tag -s $@.debug -o $@

${OBJS}: %.o: %.${EXT}
	@ echo CC $<
	@ ${CC} -c $< -o $@

REALDEPS:=$(wildcard .*.d)
ifneq ($(strip $(REALDEPS)),)
	include $(REALDEPS)
endif

.PHONY: clean
clean:
	@ echo CLEAN ${PROGS}
	@ rm -f ${PROGS}
	@ rm -f ${PROGS}.debug

	@ echo CLEAN ${OBJS}
	@ rm -f ${OBJS}

	@ echo CLEAN ${DEPS}
	@ rm -f ${DEPS}

