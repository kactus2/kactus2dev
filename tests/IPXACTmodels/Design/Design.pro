#-----------------------------------------------------------------------------
# File: BusDefinition.pro
#-----------------------------------------------------------------------------
# Project: Kactus2
# Author: Esko Pekkarinen
# Date: 04.05.2018
#
# Description:
# Qt project file template for running unit tests for Design.
#-----------------------------------------------------------------------------

TEMPLATE = subdirs

CONFIG += testcase
SUBDIRS += tst_DesignReader.pro \
	       tst_DesignWriter.pro \
		   tst_DesignValidator.pro
