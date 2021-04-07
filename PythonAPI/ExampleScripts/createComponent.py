import pythonAPI

def createRegister(registerInterface, registerIndex, registerName, description, offset, size):
	registerDataIndex = registerInterface.indexInRegisterData(registerIndex);
	registerInterface.addRegister(registerIndex, registerDataIndex, registerName)
	registerInterface.setDescription(registerName, description)
	registerInterface.setOffset(registerName, offset)
	registerInterface.setSize(registerName, size)

def createField(fieldInterface, fieldIndex, fieldName, description, offset, width, access):
	fieldInterface.addField(fieldIndex, fieldName)
	fieldInterface.setDescription(fieldName, description)
	fieldInterface.setOffset(fieldName, offset)
	fieldInterface.setWidth(fieldName, width)
	fieldInterface.setAccess(fieldName, access)

def createReset(resetInterface, resetIndex, resetType, resetValue, resetMask):
	resetInterface.addReset(resetIndex)
	resetInterface.setResetTypeReference(resetIndex, resetType)
	resetInterface.setResetValue(resetIndex, resetValue)
	resetInterface.setResetMask(resetIndex, resetMask)

def constructComponent(api, vendor, library, name, version):
	
	if (api.vlnvExistsInLibrary(vendor, library, name, version)):
		print ("Component exists already")
		return False

	# Create a component.
	success = api.createComponent(vendor, library, name, version)
	if (not success):
		print ("Could not create the example component")
		return False
	
	api
	mapInterface = api.getMapInterface()
	mapName = "memoryMap"
	mapInterface.addMemoryMap(0, mapName)
	mapInterface.setAddressUnitBits(mapName, "16")

	# Set available address blocks for the address block interface.
	api.setBlocksForInterface(mapName)
	blockInterface = mapInterface.getSubInterface()
	blockName = "PP_Address_Block"
	blockInterface.addBlock(0, blockName)
	blockInterface.setDescription(blockName, "Peripheral address block")
	blockInterface.setBaseAddress(blockName, "'h0")
	blockInterface.setRange(blockName, "32")
	blockInterface.setWidth(blockName, "32")

	# Set available register for the register interface.
	api.setRegistersForInterface(mapName, blockName)
	registerInterface = blockInterface.getSubInterface()
	registerName1 = "STAT"
	registerIndex = 0
	createRegister(registerInterface, registerIndex, registerName1, "Status register", "2", "32")

	# Set available fields for the field interface.
	api.setFieldsForInterface(mapName, blockName, registerName1)
	fieldInterface = registerInterface.getSubInterface()
	fieldName1 = "RX_FIFO"
	fieldIndex = 0
	createField(fieldInterface, fieldIndex, fieldName1, "RX_FIFO Not Empty", "0", "1", "read-only")
	
	# Set available resets for the reset interface.
	api.setResetsForInterface(mapName, blockName, registerName1, fieldName1)
	resetInterface = fieldInterface.getSubInterface()
	createReset(resetInterface, 0, "", "1'b1", "1'b1")
	
	fieldName2 = "RX_FIFO_OVFL"
	fieldIndex = fieldIndex + 1
	createField(fieldInterface, fieldIndex, fieldName2, "RX_FIFO Overflow", "1", "1", "read-write")

	# Set available resets for the reset interface.
	api.setResetsForInterface(mapName, blockName, registerName1, fieldName2)
	createReset(resetInterface, 0, "", "1'b1", "1'b1")

	registerName2 = "CMD"
	registerIndex = registerIndex + 1
	createRegister(registerInterface, registerIndex, registerName2, "Command register", "4", "32")

	# Set available fields for the field interface.
	api.setFieldsForInterface(mapName, blockName, registerName2)
	fieldName3 = "COMMAND"
	fieldIndex = 0
	createField(fieldInterface, fieldIndex, fieldName3, "Peripheral Command", "0", "4", "read-write")
	
	# Set available resets for the reset interface.
	api.setResetsForInterface(mapName, blockName, registerName2, fieldName3)
	createReset(resetInterface, 0, "", "1'b1", "1'b1")
	
	return True

print ("\n", end='')
print ("Example for constructing a Kactus2 component")
api = pythonAPI.PythonAPI()

vendor = "tut.fi"
library = "example"
name = "register_if"
version = "1.0"

success = constructComponent(api, vendor, library, name, version)
if (success):
	api.saveComponent()

api.closeOpenComponent()
