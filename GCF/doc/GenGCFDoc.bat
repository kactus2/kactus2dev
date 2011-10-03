:: This batch file generates the documentation for GCF and generates Qt Documentation files.
:: These files are used by GCFAssistant. Optionally you can also include them in Qt Assistant.

@ECHO OFF

:: Lets copy logo files
copy /Y images\gcflogo.png html
copy /Y images\vcllogo.png html
copy /Y images\vtkd2logo.png html
copy /Y images\vcl_logo_small.jpg html
copy /Y images\gcf.jpg html
copy /Y images\gcf_*.png html

:: Lets first create documentation of GCF
doxygen Doxyfile
ECHO Generated HTML Documentation for GCF

:: Now create qch and qhcp files
cd html
doxytag -t gcf.xml
ECHO Generated TAG file for GCF from doxytag

..\..\bin\DoxytagToQHP.exe gcf.xml GCFHelp.qhp GCFHelp.qhcp com.vcreatelogic.gcf.2.0 GCF 2.0 Generic_Component_Framework
ECHO Generated GCFHelp.qhp and GCFHelp.qhcp

..\..\bin\MergeFiles.exe ..\images\additionalkeywords.xml GCFHelp.qhp 68
..\..\bin\MergeFiles.exe ..\images\tableofcontents.xml GCFHelp.qhp 8
ECHO Table of contents merged

qcollectiongenerator GCFHelp.qhcp -o GCFHelp.qhc
ECHO Generated QtHelp files

cd ..
move html\GCFHelp.qch help
move html\GCFHelp.qhc help

ECHO GCF Documentation generated


