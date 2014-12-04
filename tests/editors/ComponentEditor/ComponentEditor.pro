#-----------------------------------------------------------------------------
# File: ComponentEditor.pro
#-----------------------------------------------------------------------------
# Project: Kactus 2
# Author: Esko Pekkarinen
# Date: 12.11.2014
#
# Description:
# Qt project file template for running unit tests in ComponentEditor.
#-----------------------------------------------------------------------------

TEMPLATE = subdirs

CONFIG += testcase
SUBDIRS += modelParameters/modelParameters.pro \
		common/tst_SystemVerilogExpressionParser.pro \
		common/tst_IPXactSystemVerilogParser.pro 