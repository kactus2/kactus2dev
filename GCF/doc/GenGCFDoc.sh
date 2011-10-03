# This batch file generates the documentation for GCF and generates Qt Documentation files
# These files are used by GCFAssistant. Optionally you can also include them in Qt Assistant

# Copy logo files
cp -f images/gcflogo.png html
cp -f images/vcllogo.png html
cp -f images/vtkd2logo.png html
cp -f images/vcl_logo_small.jpg html
cp -f images/gcf.jpg html
cp -f images/gcf_*.png html

# Lets first create documentation of GCF
doxygen

# Now ask doxytag to generate the tag file for us
cd html
doxytag -t gcf.xml

# Now generate input files for qcollectiongenerator
../../bin/DoxytagToQHP gcf.xml GCFHelp.qhp GCFHelp.qhcp com.vcreatelogic.gcf.2.0 GCF 2.0 Generic_Component_Framework

../../bin/MergeFiles ../images/additionalkeywords.xml GCFHelp.qhp 68
../../bin/MergeFiles ../images/tableofcontents.xml GCFHelp.qhp 8

# Now generate QCH and QHC files for use with QtHelp module
/usr/local/Trolltech/Qt-4.4.1/bin/qcollectiongenerator GCFHelp.qhcp -o GCFHelp.qhc

# Move them to the correct location
cd ..
mv html/GCFHelp.qch help
mv html/GCFHelp.qhc help
