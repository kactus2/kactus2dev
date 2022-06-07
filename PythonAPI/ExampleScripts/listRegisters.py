import pythonAPI

indentationIncrease = "  "

def printRegisterData(registerInterface, indentation, registerName):
	print (indentation + registerName)
	indentation = indentation + indentationIncrease
	print (indentation + "Offset: " + registerInterface.getOffsetValue(registerName))
	print (indentation + "Size: " + registerInterface.getSizeValue(registerName))
	print (indentation + "Access: " + registerInterface.getAccessString(registerName))

def printRegisters(registerInterface, indentation):
	indentation = indentation + indentationIncrease
	
	for registerIndex in range(registerInterface.itemCount()):
		registerName = registerInterface.getIndexedItemName(registerIndex)
		printRegisterData(registerInterface, indentation, registerName)
	
def printAddressBlocks(api, mapName, blockInterface, indentation):
		indentation = indentation + indentationIncrease
		
		for blockName in blockInterface.getItemNames():
			print (indentation + blockName)
			
			api.setRegistersForInterface(mapName, blockName)
			registerInterface = blockInterface.getSubInterface()
			if (registerInterface.itemCount() > 0):
				printRegisters(registerInterface, indentation)
			
def printMemoryMaps(api, mapInterface, vlnv):
	print ("Memory map structure:")
	for mapName in mapInterface.getItemNames():
		indentation = indentationIncrease
		print (indentation + mapName)
		
		api.setBlocksForInterface(mapName)
		blockInterface = mapInterface.getAddressBlockInterface()
		if (blockInterface.itemCount() > 0):
			printAddressBlocks(api, mapName, blockInterface, indentation)

def listComponentRegisters(api, vlnv):
	vlnvArray = vlnv.split(":")
	if (len(vlnvArray) != 4):
		print ("VLNV should be given in format vendor:library:name:version")
		return
	
	success = api.openComponent(vlnv)
	if (success == False):
		print ("Cannot open component " + vlnv)
		return

	print (vlnv)
	
	mapInterface = api.getMapInterface()
	if (mapInterface.itemCount() > 0):
		printMemoryMaps(api, mapInterface, vlnv)
		
	api.closeOpenComponent()
	
print ("\n", end='')
print ("Example for displaying registers of the selected component")
api = pythonAPI.PythonAPI()

command = input("VLNV: ")
listComponentRegisters(api, command)