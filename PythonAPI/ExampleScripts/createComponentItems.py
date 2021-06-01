import pythonAPI

def createRegister(registerInterface, registerIndex, registerName, description, offset, size):
	registerDataIndex = registerInterface.indexInRegisterData(registerIndex)
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

def createPort(portInterface, portName, direction, leftBound, rightBound):
	portInterface.addWirePort(portName)
	portInterface.setDirection(portName, direction)
	portInterface.setLeftBound(portName, leftBound)
	portInterface.setRightBound(portName, rightBound)
