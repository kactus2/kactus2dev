from ipmm_core_pkg.component import Component
from ipmm_core_pkg.port import Port
from ipmm_core_pkg.parameter import Parameter
from ipmm_core_pkg.register import Register
from ipmm_core_pkg.field import Field
from ipmm_core_pkg.reset import Reset

def createComponent(api, portsInterface, parameterInterface):
	componentName = api.getComponentName().toStdString()
	componentDescription = api.getComponentDescription().toStdString()
	
	newComponent = Component(componentName, componentDescription)
	
	for portName in portsInterface.getItemNames():
		if portsInterface.portIsWire(portName):
			portDescription = portsInterface.getDescription(portName)
			portDataType = ""
			portBitWidth = portsInterface.getWidth(portName)
			portDirection = portsInterface.getDirection(portName)
			
			newComponent.add_port(Port(portName, portDescription, portDataType, portBitWidth, portDirection))
	
	for parameterName in parameterInterface.getItemNames():
		description = parameterInterface.getDescription(parameterName)
		dataType = parameterInterface.getType(parameterName)
		
		bitWidthLeft = parameterInterface.getBitWidthLeftValue(parameterName, 10)
		if not bitWidthLeft:
			bitWidthLeft = "0"
		bitWidthRight = parameterInterface.getBitWidthRightValue(parameterName, 10)
		if not bitWidthRight:
			bitWidthRight = "0"
		
		bitWidth = abs(int(bitWidthLeft) - int(bitWidthRight)) + 1
		value = parameterInterface.getValueFormattedExpression(parameterName)
		
		newComponent.add_parameter(Parameter(parameterName, description, dataType, bitWidth, value))
	
	for registerInterface in api.getRegisterInterfaces():
		for registerName in registerInterface.getItemNames():
			registerDescription = registerInterface.getDescription(registerName)
			registerOffset = registerInterface.getOffsetValue(registerName, 10)
			registerSize = registerInterface.getSizeValue(registerName, 10)
			
			newRegister = Register(registerName, registerDescription, registerOffset, registerSize)
			
			fieldInterface = registerInterface.getSelectedSubInterface(registerName)
			for fieldName in fieldInterface.getItemNames():
			
				fieldDescription = fieldInterface.getDescription(fieldName)
				fieldBitOffset = fieldInterface.getOffsetValue(fieldName, 10)
				fieldBitWidth = fieldInterface.getWidthValue(fieldName, 10)
				fieldAccess = fieldInterface.getAccess(fieldName)
			
				newField = Field(fieldName, fieldDescription, fieldBitOffset, fieldBitWidth, fieldAccess)
				newRegister.add_field(newField)
				
				resetInterface = fieldInterface.getResetInterface(fieldName)
				if resetInterface is not None:
					for resetType in resetInterface.getItemNames():
						newResetType = resetType
						if not newResetType:
							newResetType = "HARD"
						resetName = ""
						resetDescription = ""
						resetValue = resetInterface.getResetValue(resetType, 10)
						resetMask = resetInterface.getResetMaskValue(resetType, 10)
					
						newField.add_reset(Reset(resetName, resetDescription, newResetType, resetValue, resetMask))
	
			newRegister.printer()
			print("\n")
		
	return newComponent
