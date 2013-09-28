include Common/CommonDefs.mak

.PHONY: all

NETPROJ = 
ifneq "$(realpath /usr/bin/gmcs)" ""
	NETPROJ += SimpleRead.net
	NETPROJ += SimpleViewer.net
	NETPROJ += UserTracker.net
endif

all: $(NETPROJ) NiHandTracker NiConvertXToONI NiSkeletonBenchmark NiSimpleRead UserTracker.java NiUserTracker SimpleRead.java SimpleViewer.java NiAudioSample NiUserSelection NiRecordSynthetic NiSampleModule NiViewer NiBackRecorder NiSimpleCreate NiSimpleViewer NiCRead NiSimpleSkeleton 


.PHONY: NiHandTracker
NiHandTracker:
	$(MAKE) -C ../NiHandTracker

.PHONY: SimpleRead.net
SimpleRead.net:
	$(MAKE) -C ../SimpleRead.net

.PHONY: NiConvertXToONI
NiConvertXToONI:
	$(MAKE) -C ../NiConvertXToONI

.PHONY: NiSkeletonBenchmark
NiSkeletonBenchmark:
	$(MAKE) -C ../NiSkeletonBenchmark

.PHONY: NiSimpleRead
NiSimpleRead:
	$(MAKE) -C ../NiSimpleRead

.PHONY: UserTracker.java
UserTracker.java:
	$(MAKE) -C ../UserTracker.java

.PHONY: SimpleViewer.net
SimpleViewer.net:
	$(MAKE) -C ../SimpleViewer.net

.PHONY: NiUserTracker
NiUserTracker:
	$(MAKE) -C ../NiUserTracker

.PHONY: SimpleRead.java
SimpleRead.java:
	$(MAKE) -C ../SimpleRead.java

.PHONY: SimpleViewer.java
SimpleViewer.java:
	$(MAKE) -C ../SimpleViewer.java

.PHONY: NiAudioSample
NiAudioSample:
	$(MAKE) -C ../NiAudioSample

.PHONY: NiUserSelection
NiUserSelection:
	$(MAKE) -C ../NiUserSelection

.PHONY: NiRecordSynthetic
NiRecordSynthetic:
	$(MAKE) -C ../NiRecordSynthetic

.PHONY: UserTracker.net
UserTracker.net:
	$(MAKE) -C ../UserTracker.net

.PHONY: NiSampleModule
NiSampleModule:
	$(MAKE) -C ../NiSampleModule

.PHONY: NiViewer
NiViewer:
	$(MAKE) -C ../NiViewer

.PHONY: NiBackRecorder
NiBackRecorder:
	$(MAKE) -C ../NiBackRecorder

.PHONY: NiSimpleCreate
NiSimpleCreate:
	$(MAKE) -C ../NiSimpleCreate

.PHONY: NiSimpleViewer
NiSimpleViewer:
	$(MAKE) -C ../NiSimpleViewer

.PHONY: NiCRead
NiCRead:
	$(MAKE) -C ../NiCRead

.PHONY: NiSimpleSkeleton
NiSimpleSkeleton:
	$(MAKE) -C ../NiSimpleSkeleton
