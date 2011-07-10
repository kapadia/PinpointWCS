
echo "Revision Script Running ..."
echo "#define VERSION \"0.9.1\"" > version.h
echo "#define REVISION "\"`/Library/Frameworks/Python.framework/Versions/Current/bin/hg id -n`\" >> version.h
