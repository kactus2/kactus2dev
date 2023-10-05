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
SUBDIRS += tst_AddressSpaceReader.pro \
		   tst_AddressSpaceWriter.pro \
		   tst_AddressSpaceValidator.pro \
		   tst_BusInterfacePortMapValidator.pro \
		   tst_BusInterfaceReader.pro \
		   tst_BusInterfaceWriter.pro \
		   tst_BusInterfaceValidator.pro \
		   tst_ChannelReader.pro \
		   tst_ChannelWriter.pro \
		   tst_ChannelValidator.pro \
		   tst_ChoiceReader.pro \
		   tst_ChoiceWriter.pro \
		   tst_ChoiceValidator.pro \
		   tst_ComponentGeneratorReader.pro \
		   tst_ComponentGeneratorWriter.pro \
		   tst_ComponentGeneratorValidator.pro \
		   tst_ComponentPortWriter.pro \
		   tst_ComponentPortReader.pro \
		   tst_ComponentPortValidator.pro \
		   tst_CPUReader.pro \
		   tst_CPUWriter.pro \
		   tst_CPUValidator.pro \
		   tst_FileWriter.pro \
		   tst_FileReader.pro \
		   tst_FileValidator.pro \
		   tst_FileSetWriter.pro \
		   tst_FileSetReader.pro \
		   tst_FileSetValidator.pro \
		   tst_IndirectInterfaceReader.pro \
		   tst_IndirectInterfaceWriter.pro \
		   tst_IndirectInterfaceValidator.pro \
		   tst_InstantiationsWriter.pro \
		   tst_InstantiationsReader.pro \
		   tst_InstantiationsValidator.pro \
		   tst_OtherClockDriverReader.pro \
		   tst_OtherClockDriverWriter.pro \
		   tst_OtherClockDriverValidator.pro \		   
		   tst_ParameterReader.pro \
		   tst_ParameterWriter.pro \
		   tst_ParameterValidator.pro \
		   tst_RemapStateWriter.pro \
		   tst_RemapStateReader.pro \
		   tst_RemapStateValidator.pro \
		   tst_ViewWriter.pro \
		   tst_ViewReader.pro \
		   tst_ViewValidator.pro \
		   tst_EnumeratedValueWriter.pro \
		   tst_EnumeratedValueReader.pro \
		   tst_EnumeratedValueValidator.pro \
		   tst_FieldWriter.pro \
		   tst_FieldReader.pro \
		   tst_FieldValidator.pro \
		   tst_RegisterWriter.pro \
		   tst_RegisterReader.pro \		
		   tst_RegisterValidator.pro  \		   		  
		   tst_AddressBlockWriter.pro \
		   tst_AddressBlockReader.pro \
		   tst_AddressBlockValidator.pro \
		   tst_MemoryArrayReader.pro \
		   tst_MemoryArrayValidator.pro \
		   tst_MemoryArrayWriter.pro \
		   tst_MemoryMapBaseReader.pro \
		   tst_MemoryMapBaseWriter.pro \
		   tst_MemoryMapBaseValidator.pro \
		   tst_MemoryMapWriter.pro \
		   tst_MemoryMapReader.pro \
		   tst_MemoryMapValidator.pro \  
		   tst_ModeReader.pro \  
		   tst_ModeWriter.pro \  
		   tst_ModeValidator.pro \
		   tst_PowerDomainWriter.pro \
		   tst_PowerDomainReader.pro \
#		   tst_PowerDomainValidator.pro \  
		   tst_ComponentWriter.pro \
		   tst_ComponentReader.pro \
		   tst_ComponentValidator.pro \
		   tst_FieldAccessPolicyWriter.pro \
		   tst_FieldAccessPolicyReader.pro \
		   tst_FieldAccessPolicyValidator.pro \
		   tst_FieldReferenceWriter.pro \
		   tst_FieldReferenceReader.pro \
		   tst_FieldReferenceValidator.pro
		   
