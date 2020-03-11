
# Getting port and related bitwidth from IP-XACT is way too complex to serve simple templates.
# Thus IR model has much simples port definition

from ipmm_core_pkg.primitive import Primitive

class Port(Primitive):

	def __init__(self, name, description, dataType, bitWidth, direction):
		pass
		Primitive.__init__(self, name, description)
		self.dataType = dataType
		self.bitWidth = bitWidth
		self.direction = direction
				
	def get_dataType(self):
		return self.dataType
		
	def get_bitWidth(self):
		return self.bitWidth
		
	def get_direction(self):
		return self.direction

	def printer(self):
		Primitive.printer(self)
		print("dataType: ", self.get_dataType())
		print("bitWidth: ", self.get_bitWidth())
		print("direction: ", self.get_direction())
		

# Stand-a-lone testing
if __name__ == "__main__":	
	port_1 = Port("RX_FIFO_output", "RX-FIFO Not Empty output.", "logic", 1, "output")
	port_1.printer()

