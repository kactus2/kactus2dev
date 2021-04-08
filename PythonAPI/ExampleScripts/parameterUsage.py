import pythonAPI

parameterName = "DATA_WIDTH"

def setParameterToPorts(api, parameterID):
	leftBoundExpression = parameterID + "-1"
	portsInterface = api.getPortsInterface()
	for portName in portsInterface.getItemNames():
		leftValue = portsInterface.getLeftBoundExpression(portName)
		if (leftValue == "31"):
			portsInterface.setLeftBound(portName, leftBoundExpression)

def addParameter(api):
	parameterInterface = api.getComponentParameterInterface()
	newParameterIndex = parameterInterface.itemCount()
	parameterInterface.addParameter(newParameterIndex, parameterName)
	parameterInterface.setValue(parameterName, "32")
	return parameterInterface.getID(parameterName)

print ("\n", end='')
print ("Example for using parameters in a Kactus2 component")
api = pythonAPI.PythonAPI()

vendor = "tut.fi"
library = "example"
name = "register_if"
version = "1.0"

vlnvString = vendor + ":" + library + ":" + name + ":" + version
success = api.openComponent(vlnvString)
if (success):
	parameterID = addParameter(api)
	setParameterToPorts(api, parameterID)
	
	api.saveComponent()
	api.closeOpenComponent()
	
	print ("Changed the left bound values of the ports of component " + vlnvString +
	" to an expression using parameter " + parameterName)
else:
	print ("Component " + vlnvString + " was not found.")
	print ("Use createComponent.py to create the example component")