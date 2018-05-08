#-----------------------------------------------------------------------------
# File: Core.pro
#-----------------------------------------------------------------------------
# Project: Kactus2
# Author: Esko Pekkarinen
# Date: 08.05.2018
#
# Description:
# Qt project file template for running unit tests for Kactus2 core.
#-----------------------------------------------------------------------------

TEMPLATE = subdirs

CONFIG += testcase
SUBDIRS += tst_CommandLineParser.pro \
		   tst_HierarchicalSaveBuildStrategy.pro \
		   tst_DocumentTreeBuilder.pro \
		   tst_documentGenerator.pro
