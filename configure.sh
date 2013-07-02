#!/bin/sh

# Generate compressed help files.
qhelpgenerator Help/kactus2help.qhp -o Help/Kactus2Help.qch
qcollectiongenerator Help/kactus2help.qhcp -o Help/Kactus2Help.qhc
rm -R -f executable/Help
mkdir executable/Help
cp -f Help/Kactus2Help.qhc executable/Help/Kactus2Help.qhc
cp -f Help/Kactus2Help.qch executable/Help/Kactus2Help.qch

#Run qmake using qtchooser.
qtchooser -run-tool=qmake -qt=qt5 Kactus2_Solution.pro