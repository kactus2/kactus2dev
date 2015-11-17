#-----------------------------------------------------------------------------
# File: Common.pro
#-----------------------------------------------------------------------------
# Project: Kactus 2
# Author: Mikko Teuho
# Date: 16.11.2015
#
# Description:
# Qt project file template for running common IP-XACT unit tests.
#-----------------------------------------------------------------------------

TEMPLATE = subdirs

CONFIG += testcase
SUBDIRS += Validators\tst_ParameterValidator2014.pro \
		   tst_ParameterWriter.pro \
		   tst_ParameterReader.pro \