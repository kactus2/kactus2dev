#-----------------------------------------------------------------------------
# File: verilogGenerator.pro
#-----------------------------------------------------------------------------
# Project: Kactus 2
# Author: Esko Pekkarinen
# Date: 24.27.2014
#
# Description:
# Qt project file for running unit tests in subdirectories.
#-----------------------------------------------------------------------------

TEMPLATE = subdirs

CONFIG += testcase

SUBDIRS += \
           verilogParameter/tst_verilogParameter.pro \
           verilogPort/tst_verilogPort.pro \
           verilogComponentInstance/tst_verilogComponentInstance.pro
