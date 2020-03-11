from ipmm_core_pkg.component import Component
from ipmm_core_pkg.port import Port

def createComponent(api):
	componentName = api.getComponentName().toStdString()
	componentDescription = api.getComponentDescription().toStdString()
	
	newComponent = Component(componentName, componentDescription)
	
	componentPorts = api.createSimplePortsForComponent()
	for port in componentPorts:
		portName = port.getName().toStdString()
		portDescription = port.getDescription().toStdString()
		portDataType = port.getDataType().toStdString()
		portBitWidth = port.getBitWidth()
		portDirection = port.getDirection().toStdString()
		
		newComponent.add_port(Port(portName, portDescription, portDataType, portBitWidth, portDirection))
	
	return newComponent
