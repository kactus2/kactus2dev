#-----------------------------------------------------------------------------
# File: subdirs.pro
#-----------------------------------------------------------------------------
# Project: Kactus 2
# Author: Esko Pekkarinen
# Date: 24.27.2014
#
# Description:
# Qt project file template for running unit tests for generators.
#-----------------------------------------------------------------------------

TEMPLATE = subdirs

CONFIG += testcase
SUBDIRS += documentGenerator/tst_documentGenerator.pro
