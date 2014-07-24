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

win32-msvc {
    TEMPLATE = vcsubdirs
}
else {
    TEMPLATE = subdirs
}

CONFIG += testcase

SUBDIRS += kactusGenerators/kactusGenerators.pro
