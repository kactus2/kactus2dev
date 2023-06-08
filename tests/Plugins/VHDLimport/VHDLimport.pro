#-----------------------------------------------------------------------------
# File: VHDLimport.pro
#-----------------------------------------------------------------------------
# Project: Kactus2
# Author: Esko Pekkarinen
# Date: 19.09.2014
#
# Description:
# Qt project file template for running unit tests for VHDLimport.
#-----------------------------------------------------------------------------

TEMPLATE = subdirs

CONFIG += testcase
SUBDIRS += tst_VHDLPortParser.pro \
		   tst_VHDLGenericParser.pro \
		   tst_VHDLimport.pro 