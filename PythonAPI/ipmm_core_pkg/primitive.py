
class Primitive(object):

	def __init__(self, name, description):
		self.name = name
		self.description =  description
		self.elements = []
		
	def get_name(self):
		return self.name
        
	def get_description(self):
		return self.description
        
	def add_element(self, element):
		self.elements.append(element)

	def printer(self):
		print("Type: ", type(self))
		print("Name: ", self.get_name())
		print("Description: ", self.get_description())
		

# Stand-a-lone testing
if __name__ == "__main__":	
	primitive_1 = Primitive("RX_FIFO", "RX-FIFO Not Empty element")
	primitive_1.printer()
