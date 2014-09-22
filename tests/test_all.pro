#-----------------------------------------------------------------------------
# File: test_all.pro
#-----------------------------------------------------------------------------
# Project: Kactus 2
# Author: Esko Pekkarinen
# Date: 24.27.2014
#
# Description:
# Qt project file for running all unit tests.
#-----------------------------------------------------------------------------

TEMPLATE = subdirs

CONFIG += testcase

SUBDIRS += Plugins/plugins.pro 
           
