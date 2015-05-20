#-----------------------------------------------------------------------------
# File: mainwindow.pro
#-----------------------------------------------------------------------------
# Project: Kactus 2
# Author: Esko Pekkarinen
# Date: 18.05.2015
#
# Description:
# Qt project file template for running unit tests in directory mainwindow.
#-----------------------------------------------------------------------------

TEMPLATE = subdirs

CONFIG += testcase
SUBDIRS += DrawingBoard/tst_DocumentTreeBuilder.pro
