#src/Makefile

bin_PROGRAMS = finalmetronome

finalmetronome_CPPFLAGS = -I$(abs_top_srcdir)/libopentime -I$(top_srcdir)/ -DDATADIR=\"$(pkgdatadir)\"

finalmetronome_SOURCES = \
	BeatMgr.cpp \
	BeatSoundMapDlg.cpp \
	MetronomeApp.cpp \
	MetronomeDlg.cpp \
	SoundManager.cpp \
	SoundManagerFMOD.cpp \
	SoundTimerFunc.cpp \
	WindowAnimation.cpp \
	XmlParser.cpp \
	XmlParserConfig.cpp

finalmetronome_LDADD = -lfmodex-`$(abs_top_builddir)/fmodgetver/fmodgetver` ../libopentime/libopentime.a

# The -rpath option tell ld (GNU linker) to search for the binary shared libraries files (.so files) also in the current directory
finalmetronome_LDFLAGS=-Wl,-rpath=.
