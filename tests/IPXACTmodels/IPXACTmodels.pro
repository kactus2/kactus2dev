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
SUBDIRS += AbstractionDefinition\AbstractionDefinition.pro \
		   BusDefinition\BusDefinition.pro \
		   Catalog\Catalog.pro\
		   DesignConfiguration\DesignConfiguration.pro \
		   Design\Design.pro \
		   Component\Component.pro \
		   KactusExtensions\KactusExtensions.pro 