#-----------------------------------------------------------------------------
# File: subdirs.pro
#-----------------------------------------------------------------------------
# Project: Kactus 2
# Author: Esko Pekkarinen
# Date: 24.27.2014
#
# Description:
# Qt project file template for running unit tests for all plugins.
#-----------------------------------------------------------------------------

TEMPLATE = subdirs

CONFIG += c++17 testcase

SUBDIRS += VerilogGenerator/VerilogGenerator.pro \
			MakefileGenerator/tst_MakefileGenerator.pro \
			VHDLimport/VHDLimport.pro \
			VerilogImport/VerilogImport.pro \
		    VerilogIncludeImport/tst_VerilogIncludeImport.pro \
			VerilogSourceAnalyzer/tst_VerilogSourceAnalyzer.pro \
			MemoryMapHeaderGenerator/tst_MemoryMapHeaderGenerator.pro \
#			MemoryViewGenerator/tst_MemoryViewGenerator.pro \
			CppSourceAnalyzer/tst_CppSourceAnalyzer.pro \
			QuartusProjectGenerator/tst_QuartusProjectGenerator.pro \
			LinuxDeviceTreeGenerator/tst_LinuxDeviceTreeGenerator.pro
