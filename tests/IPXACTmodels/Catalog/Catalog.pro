#-----------------------------------------------------------------------------
# File: BusDefinition.pro
#-----------------------------------------------------------------------------
# Project: Kactus2
# Author: Esko Pekkarinen
# Date: 04.05.2018
#
# Description:
# Qt project file template for running unit tests for BusDefinition.
#-----------------------------------------------------------------------------

TEMPLATE = subdirs

CONFIG += testcase
SUBDIRS += tst_CatalogReader.pro \
	       tst_CatalogWriter.pro \
		   tst_CatalogValidator.pro
