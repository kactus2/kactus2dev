import datetime
import pythonAPI
from ExampleScripts.createComponentItems import createPort
#from ExampleScripts.createComponent import createPort

def constructFile(api, fileInterface, fileSetName, filePath):
	api.setFilesForInterface(fileSetName)

	if (not fileInterface.itemHasValidName(filePath)):
		fileRow = fileInterface.itemCount()
		fileInterface.addFile(fileRow, filePath)
		
		fileName = fileInterface.getIndexedItemName(fileRow)
	
		fileInterface.addFileType(fileName, "verilogSource")
	
		currentDateTime = datetime.datetime.now()
		date = currentDateTime.strftime("%d.%m.%Y")
		time = currentDateTime.strftime("%H:%M:%S")
		
		fileDescription = "Generated at " + time + " on " + date + " by Kactus2."
		fileInterface.setDescription(fileName, fileDescription)
	
def constructFileSet(fileSetInterface):
	fileSetName = "verilogFiles"
	if (not fileSetInterface.itemHasValidName(fileSetName)):
		newRow = fileSetInterface.itemCount()
		fileSetInterface.addFileSet(newRow, fileSetName)
	return fileSetName
	
def runVerilogGenerator(api, vendor, library, name, version, vlnvString, viewName):
	filePath = api.getVLNVDirectory(vendor, library, name, version)
	if (not filePath):
		print ("Could not find file " + vlnvString)
		return ""
	
	api.generate("verilog", vlnvString, viewName, filePath)
	
	verilogFileName = name + ".v"
	return verilogFileName
	
def getViewName(api):
	return api.getFirstViewName()
	
def portExists(portInterface, comparisonName):
	for portName in portInterface.getItemNames():
		if (portName == comparisonName):
			return True
	return False
	
def createOneBitPorts(api):
	portInterface = api.getPortsInterface()
	clkName = "clk"
	if (not portExists(portInterface, clkName)):
		createPort(portInterface, clkName, "in", "0", "0")
	rstName = "rst_n"
	if (not portExists(portInterface, rstName)):
		createPort(portInterface, rstName, "out", "0", "0")
	ackName = "ack"
	if (not portExists(portInterface, ackName)):
		createPort(portInterface, ackName, "inout", "0", "0")

print ("\n", end='')
print ("Example for generating a verilog file from a Kactus2 component")
api = pythonAPI.PythonAPI()

vendor = "tut.fi"
library = "example"
name = "register_if"
version = "1.0"

vlnvString = vendor + ":" + library + ":" + name + ":" + version
success = api.openComponent(vlnvString)
if (success):
	
	createOneBitPorts(api)
	viewName = getViewName(api)
	
	# Apply the changes to the component file before running the generator.
	api.saveComponent()

	verilogFileName = runVerilogGenerator(api, vendor, library, name, version, vlnvString, viewName)
	if (len(verilogFileName) != 0):
		fileSetInterface = api.getFileSetInterface()
		fileSetName = constructFileSet(fileSetInterface)
		constructFile(api, fileSetInterface.getFileInterface(), fileSetName, verilogFileName)
		
	api.saveComponent()
	api.closeOpenComponent()
	
	print ("The generated verilog file can be found in the file sets of component " + vlnvString)
else:
	print ("Component " + vlnvString + " was not found.")
	print ("Use createComponent.py to create the example component")