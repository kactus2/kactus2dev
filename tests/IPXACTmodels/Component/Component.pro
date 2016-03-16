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
SUBDIRS += validators\tst_ChannelValidator.pro \
		   validators\tst_CPUValidator.pro \
		   validators\tst_FileValidator.pro \
		   validators\tst_FileSetValidator.pro \
		   validators\tst_RemapStateValidator.pro \
		   validators\tst_AddressSpaceValidator.pro \
		   validators\tst_BusInterfaceValidator.pro \
		   validators\tst_ComponentGeneratorValidator.pro \
		   validators\tst_ChoiceValidator.pro \
		   validators\tst_OtherClockDriverValidator.pro \
		   validators\tst_ComponentValidator.pro \
		   Model\tst_ViewWriter.pro \
		   Model\tst_ViewReader.pro \
		   Model\tst_InstantiationsWriter.pro \
		   Model\tst_InstantiationsReader.pro \
		   Model\validators\tst_InstantiationsValidator.pro \
		   Model\validators\tst_ViewValidator.pro \
		   Model\validators\tst_ComponentPortValidator.pro \
		   Model\tst_ComponentPortWriter.pro \
		   Model\tst_ComponentPortReader.pro \
		   MemoryMaps\MemoryMaps.pro \
		   tst_ComponentWriter.pro \
		   tst_ComponentReader.pro \
		   tst_CPUReader.pro \
		   tst_CPUWriter.pro \
		   tst_FileWriter.pro \
		   tst_FileReader.pro \
		   tst_FileSetWriter.pro \
		   tst_FileSetReader.pro \
		   tst_ChannelReader.pro \
		   tst_ChannelWriter.pro \
		   tst_ChoiceReader.pro \
		   tst_ChoiceWriter.pro \
		   tst_businterfaceReader.pro \
		   tst_businterfaceWriter.pro \
		   tst_RemapStateWriter.pro \
		   tst_RemapStateReader.pro \
		   tst_AddressSpaceReader.pro \
		   tst_AddressSpaceWriter.pro \
		   tst_OtherClockDriverReader.pro \
		   tst_OtherClockDriverWriter.pro \
		   tst_ComponentGeneratorReader.pro \
		   tst_ComponentGeneratorWriter.pro