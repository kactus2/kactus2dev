#-----------------------------------------------------------------------------
# File: ComponentEditor.pro
#-----------------------------------------------------------------------------
# Project: Kactus 2
# Author: Esko Pekkarinen
# Date: 14.04.2014
#
# Description:
# Qt project file template for running unit tests in MemoryMaps.
#-----------------------------------------------------------------------------

TEMPLATE = subdirs

CONFIG += c++11 testcase
SUBDIRS += memoryMapsVisualizer/MemoryMapGraphItem/tst_MemoryMapGraphItem.pro \
		   memoryMapsVisualizer/AddressBlockGraphItem/tst_AddressBlockGraphItem.pro \
		   memoryMapsVisualizer/RegisterGraphItem/tst_RegisterGraphItem.pro \
		   memoryMapsVisualizer/FieldGraphItem/tst_FieldGraphItem.pro
		