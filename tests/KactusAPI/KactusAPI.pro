#-----------------------------------------------------------------------------
# File: KactusAPI.pro
#-----------------------------------------------------------------------------
# Project: Kactus2
# Author: Esko Pekkarinen
# Date: 16.10.2023
#
# Description:
# Qt project file template for running unit tests for Kactus2 API.
#-----------------------------------------------------------------------------

TEMPLATE = subdirs

CONFIG += testcase
SUBDIRS += \
            tst_ModeConditionParser.pro 
