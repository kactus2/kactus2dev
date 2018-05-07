#-----------------------------------------------------------------------------
# File: BusDefinition.pro
#-----------------------------------------------------------------------------
# Project: Kactus2
# Author: Esko Pekkarinen
# Date: 04.05.2018
#
# Description:
# Qt project file template for running unit tests for Design Configuration.
#-----------------------------------------------------------------------------

TEMPLATE = subdirs

CONFIG += testcase
SUBDIRS += tst_DesignConfigurationReader.pro \
	       tst_DesignConfigurationWriter.pro \
		   tst_DesignConfigurationValidator.pro
