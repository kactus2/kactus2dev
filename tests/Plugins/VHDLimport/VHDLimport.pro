#-----------------------------------------------------------------------------
# File: VHDLimport.pro
#-----------------------------------------------------------------------------
# Project: Kactus 2
# Author: Esko Pekkarinen
# Date: 19.09.2014
#
# Description:
# Qt project file template for running unit tests for VHDLimport.
#-----------------------------------------------------------------------------

TEMPLATE = subdirs

CONFIG += testcase
SUBDIRS += VHDLPortParser/tst_VHDLPortParser.pro \
		   VHDLGenericParser/tst_VHDLGenericParser.pro \
		   VHDLParser/tst_VHDLParser.pro 