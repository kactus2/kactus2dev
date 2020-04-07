#from ipmm_core_pkg.component import Component
import pythonHelps
from constructPythonComponent import createComponent
import pythonAPI

print ("\n", end='')
print ("Starting python program")
api = pythonAPI.PythonAPI()
portsInterface = api.getPortsInterface()
parameterInterface = api.getComponentParameterInterface()

def handleCommandsForOpenComponent(componentName):
	pythonHelps.componentHelpText()
	print ("\n", end='')
	componentCommand = input("Component command:")
	commandArray = []
	while (componentCommand != pythonHelps.closeWord and componentCommand != pythonHelps.exitWord):

		if (componentCommand != ""):
			commandArray = componentCommand.split()
			componentCommand = commandArray.pop(0)
			
		if (componentCommand == pythonHelps.portsWord):
			for portName in portsInterface.getItemNames():
				print (portName)
		elif (componentCommand == pythonHelps.portWord and len(commandArray) == 3 and commandArray[1] == pythonHelps.setNameCommandWord):
			currentPortName = commandArray[0]
			newPortName = commandArray[2]
			portsInterface.setName(currentPortName, newPortName);

			print ("Changed port %s name to %s" % (currentPortName, newPortName))
			
		elif (componentCommand == pythonHelps.saveWord):
			api.saveComponent()
		elif (componentCommand == pythonHelps.migrateWord):
			pythonComponent = createComponent(api, portsInterface, parameterInterface)
			print ()
			pythonComponent.printer()
			for parameter in pythonComponent.parameters:
				parameter.printer()
		elif (componentCommand == pythonHelps.helpWord):
			pythonHelps.componentHelpText()
		else:
			print("Error: unknown command")

		print ("\n", end='')
		componentCommand = input("Component command: ")
	api.closeOpenComponent()
	print ("Closing component", componentName)
	return componentCommand

print (api)
print ("\n", end='')

api.setupLibrary(pythonAPI.QString.fromStdString(r"C:\Users\teuhom\AppData\Roaming\TUT\Kactus2.ini"))

print ("\n", end='')

pythonHelps.helpText()
print("\n", end='')

command = input("Command: ")
commandTarget = ""
while (command != pythonHelps.exitWord):
	
	if (command != ""):
		commandArray = command.split()
		command = commandArray[0]
		if (len(commandArray) > 1):
			commandTarget = commandArray[1]
	
		if (command == pythonHelps.listWord):
			# List all VLNVs.
			api.listVLNVs()
		elif (command == pythonHelps.listComponentsWord):
			# List component VLNVs.
			api.listComponentVLNVs()
		elif (command == pythonHelps.openWord):
			# Open component for editing
			if (api.openComponent(pythonAPI.QString.fromStdString(commandTarget)) == True):
				print("\n", end='')
				componentCommand = handleCommandsForOpenComponent(commandTarget)
				if (componentCommand == pythonHelps.exitWord):
					break
		elif (command == pythonHelps.helpWord):
			pythonHelps.helpText()
		else:
			print("Error: unknown command")
	
	print("\n", end='')
	command = input("Command: ")

print ("Exiting...")
del api