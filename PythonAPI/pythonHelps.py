exitWord = "-exit"
listWord = "-list"
listComponentsWord = "-listComponents"
helpWord = "-help"
openWord = "-open"
closeWord = "-close"
portsWord = "-ports"
portWord = "-port"
saveWord = "-save"
migrateWord = "-migrate"

setNameCommandWord = "setName"

def helpText():
	print (exitWord, "= exit")
	print (listWord, "= list the VLNVs contained within the library")
	print (listComponentsWord, "= list the VLNVs of components within the library")
	print (openWord, "= open a VLNV document")
	print (helpWord, "= show this help")

def componentHelpText():
	print (exitWord, "= exit program")
	print (closeWord, "= close component")
	print (portsWord, "= list all the ports of the component")
	print (portWord, "x", setNameCommandWord, "y = change the name of port x to y")
	print (saveWord, "= save this component")
	print (migrateWord, " = transfer data from the active component to a python component")
	print (helpWord, "= show this help")