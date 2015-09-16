#-----------------------------------------------------------------------------
# File: ComponentModel.pro
#-----------------------------------------------------------------------------
# Project: Kactus 2
# Author: Mikko Teuho
# Date: 16.09.2015
#
# Description:
# Qt project file template for running unit tests in Component Model.
#-----------------------------------------------------------------------------

TEMPLATE = subdirs

CONFIG += testcase
SUBDIRS += Model\tst_ViewWriter.pro \
		   Model\tst_ViewReader.pro \
		   Model\tst_InstantiationsWriter.pro \
		   Model\tst_InstantiationsReader.pro \
		   Model\tst_ComponentPortWriter.pro \
		   Model\tst_ComponentPortReader.pro