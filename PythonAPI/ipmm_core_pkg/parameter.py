# Parameter can represent configuration values and constants


from ipmm_core_pkg.primitive import Primitive

class Parameter(Primitive):

	def __init__(self, name, description, dataType, bitWidth, value):
		pass
		Primitive.__init__(self, name, description)
		self.dataType = dataType
		self.bitWidth = bitWidth
		self.value = value
				
	def get_dataType(self):
		return self.dataType
		
	def get_bitWidth(self):
		return self.bitWidth
		
	def get_value(self):
		return self.value

	def printer(self):
		Primitive.printer(self)
		print("dataType: ", self.get_dataType())
		print("bitWidth: ", self.get_bitWidth())
		print("value: ", self.get_value())

# Stand-a-lone testing
if __name__ == "__main__":	
	parameter_1 = Parameter("RX_FIFO_const", "RX-FIFO threshold value parameter.", "logic" , 4, 1)
	parameter_1.printer()
