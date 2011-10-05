CONFIG += release
#CONFIG += debug_and_release

macx {

	INCLUDEPATH += . gui backend ../cfitsio/include ../libwcs/ ../eigen
	INCLUDEPATH += ../XMP-Toolkit-SDK-5.1.2/public/include
	INCLUDEPATH += ../xpa-2.1.13/include

	LIBS += -L../cfitsio/lib -lcfitsio
	LIBS += -L../libwcs -lwcs -lm
	LIBS += -L../XMP-Toolkit-SDK-5.1.2/public/libraries/macintosh/release
	LIBS += -lXMPCoreStaticRelease -lXMPFilesStaticRelease
	LIBS += -framework CoreServices
	LIBS += -L../xpa-2.1.13/lib -lxpa

	DEFINES += MAC_ENV="1"
}

unix:!macx {

	INCLUDEPATH += . gui backend ../cfitsio/include ../libwcs/ ../eigen
	INCLUDEPATH += ../XMP-Toolkit-SDK-5.1.2/public/include
	INCLUDEPATH += ../xpa-2.1.13/include

	LIBS += -L../cfitsio/lib -lcfitsio
	LIBS += -L../libwcs -lwcs -lm
	LIBS += ../XMP-Toolkit-SDK-5.1.2/public/libraries/i80386linux/release/staticXMPCore.ar
	LIBS += ../XMP-Toolkit-SDK-5.1.2/public/libraries/i80386linux/release/staticXMPFiles.ar
	LIBS += -L../xpa-2.1.13/lib -lxpa

	DEFINES += UNIX_ENV="1"
}

win32 {

	INCLUDEPATH += . gui backend ../cfitsio/include ../libwcs/ ../eigen
	INCLUDEPATH += ../XMP-Toolkit-SDK-5.1.2/public/include
	INCLUDEPATH += ../xpa-2.1.13/include

	DEFINES += WIN_ENV="1"
}
