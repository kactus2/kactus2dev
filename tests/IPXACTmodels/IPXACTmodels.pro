#-----------------------------------------------------------------------------
# File: IPXACTmodels.pro
#-----------------------------------------------------------------------------
# Project: Kactus 2
# Author: Esko Pekkarinen
# Date: 24.27.2014
#
# Description:
# Qt project file template for running unit tests in IPXACTmodels.
#-----------------------------------------------------------------------------

TEMPLATE = subdirs

CONFIG += testcase
SUBDIRS += validators\tst_ParameterValidator.pro \
		   validators\tst_ParameterValidator2014.pro
