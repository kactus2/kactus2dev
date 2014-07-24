#-----------------------------------------------------------------------------
# File: subdirs.pro
#-----------------------------------------------------------------------------
# Project: Kactus 2
# Author: Esko Pekkarinen
# Date: 24.27.2014
#
# Description:
# Qt project file template for running unit tests in subdirectories.
#-----------------------------------------------------------------------------

win32-msvc {
    TEMPLATE = vcsubdirs
}
else {
    TEMPLATE = subdirs
}

CONFIG += testcase
SUBDIRS += <subdir/subproject.pro>
