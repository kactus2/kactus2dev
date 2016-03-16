#-----------------------------------------------------------------------------
# File: MemoryMaps.pro
#-----------------------------------------------------------------------------
# Project: Kactus 2
# Author: Mikko Teuho
# Date: 17.11.2015
#
# Description:
# Qt project file template for running unit tests in Memory Maps.
#-----------------------------------------------------------------------------

TEMPLATE = subdirs

CONFIG += testcase
SUBDIRS += validators\tst_EnumeratedValueValidator.pro \
		   validators\tst_FieldValidator.pro \
		   validators\tst_RegisterValidator.pro \
		   validators\tst_AddressBlockValidator.pro \
		   validators\tst_MemoryMapBaseValidator.pro \
		   validators\tst_MemoryMapValidator.pro \
		   tst_EnumeratedValueWriter.pro \
		   tst_EnumeratedValueReader.pro \
		   tst_FieldWriter.pro \
		   tst_FieldReader.pro \
		   tst_RegisterWriter.pro \
		   tst_RegisterReader.pro \
		   tst_AddressBlockWriter.pro \
		   tst_AddressBlockReader.pro \
		   tst_MemoryMapWriter.pro \
		   tst_MemoryMapReader.pro \
		   tst_MemoryMapBaseReader.pro \
		   tst_MemoryMapBaseWriter.pro
