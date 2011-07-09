

echo "#define VERSION \"0.9.0\"" > version.h
echo "#define REVISION "\"`/Library/Frameworks/Python.framework/Versions/Current/bin/hg id -n`\" >> version.h
