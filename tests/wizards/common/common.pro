#-----------------------------------------------------------------------------
# File: common.pro
#-----------------------------------------------------------------------------
# Project: Kactus 2
# Author: Esko Pekkarinen
# Date: 16.10.2014
#
# Description:
# Qt project file template for running unit tests for common elements in wizards.
#-----------------------------------------------------------------------------

TEMPLATE = subdirs

CONFIG += testcase
SUBDIRS += VLNVComparator/tst_VLNVComparator.pro \
		ComponentComparator/tst_ComponentComparator.pro \
		ModelParameterComparator/tst_ModelParameterComparator.pro \
		ComponentDiffWidget/tst_ComponentDiffWidget.pro

