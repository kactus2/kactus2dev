#!/bin/sh
ln -f -s Kactus2 executable/libKactus2.so

qhelpgenerator Help/kactus2help.qhp -o Help/Kactus2Help.qch
qcollectiongenerator Help/kactus2help.qhcp -o Help/Kactus2Help.qhc
rm -R -f executable/Help
mkdir executable/Help
cp -f Help/Kactus2Help.qhc executable/Help/Kactus2Help.qhc
cp -f Help/Kactus2Help.qch executable/Help/Kactus2Help.qch
