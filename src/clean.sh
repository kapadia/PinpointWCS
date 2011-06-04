rm -rf build/
rm *.plist
rm moc_*.cpp
rm ui_*.h
rm -rf *.xcodeproj
rm qrc_*.cpp
rm -rf  *.app


#
#	Get revision number from mercurial
#

#echo "#define VERSION \"1.0.0\"" > version.h
#echo "#define REVISION "\"`/Library/Frameworks/Python.framework/Versions/Current/bin/hg id -n`\" >> version.h
