from ipmm_core_pkg.component import Component
from ipmm_core_pkg.port import Port
from ipmm_core_pkg.parameter import Parameter

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
	
	fieldInterfaceList = api.getFieldInterfaces()
	for fieldInterface in fieldInterfaceList:
		print (fieldInterface.getItemNames())
	
	return newComponent
