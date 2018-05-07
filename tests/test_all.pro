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

SUBDIRS += IPXACTmodels/IPXACTmodels.pro \
		editors/editors.pro \
		wizards/wizards.pro \
		Plugins/plugins.pro \
        kactusGenerators/kactusGenerators.pro \
		mainwindow/mainwindow.pro
