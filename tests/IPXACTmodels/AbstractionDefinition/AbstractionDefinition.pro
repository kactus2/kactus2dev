#-----------------------------------------------------------------------------
# File: AbstractionDefinition.pro
#-----------------------------------------------------------------------------
# Project: Kactus2
# Author: Esko Pekkarinen
# Date: 04.05.2018
#
# Description:
# Qt project file template for running unit tests for AbstractionDefinition.
#-----------------------------------------------------------------------------

TEMPLATE = subdirs

CONFIG += testcase
SUBDIRS += tst_AbstractionDefinitionReader.pro \
	       tst_AbstractionDefinitionWriter.pro \
		   tst_AbstractionDefinitionValidator.pro
