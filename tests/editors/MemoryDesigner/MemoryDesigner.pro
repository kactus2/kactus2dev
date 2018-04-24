#-----------------------------------------------------------------------------
# File: MemoryDesigner.pro
#-----------------------------------------------------------------------------
# Project: Kactus 2
# Author: Mikko Teuho
# Date: 26.03.2018
#
# Description:
# Qt project file for running unit tests in memory designer.
#-----------------------------------------------------------------------------

TEMPLATE = subdirs

CONFIG += c++11 testcase
SUBDIRS += MasterSlavePathSearch/tst_MasterSlavePathSearch.pro
