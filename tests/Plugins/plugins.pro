#-----------------------------------------------------------------------------
# File: subdirs.pro
#-----------------------------------------------------------------------------
# Project: Kactus 2
# Author: Esko Pekkarinen
# Date: 24.27.2014
#
# Description:
# Qt project file template for running unit tests for all plugins.
#-----------------------------------------------------------------------------

TEMPLATE = subdirs

CONFIG += testcase
<<<<<<< .mine
SUBDIRS += VerilogGenerator/VerilogGenerator.pro \
			MakefileGenerator/tst_MakefileGenerator.pro
=======
SUBDIRS += VerilogGenerator/VerilogGenerator.pro \
		   VHDLimport/VHDLimport.pro
>>>>>>> .r1557
