#<ipxact:addressBlock>
# But currently only can concist of register


from ipmm_core_pkg.primitive import Primitive
from ipmm_core_pkg.register import Register

class AddressBlock(Primitive):

	def __init__(self, name, description, range, width):
		pass
		Primitive.__init__(self, name, description)
		self.range = range
		self.width = width
		self.registers = []
		
	def get_range(self):
		return self.range

	def get_width(self):
		return self.width

	def add_register(self, register):
		if type(register) is not Register:
			print("Type of add_register parameter is not Register")
		else:
			self.registers.append(register)
		return 0


	def printer(self):
		Primitive.printer(self)
		print("range: ", self.get_range())
		print("width: ", self.get_width())
		for r in self.registers:
			r.printer()


# Stand-a-lone testing
if __name__ == "__main__":	
	addressBlock_1 = AddressBlock("PP_Address_Block", "Peripheral address block", 2, 32)
	addressBlock_1.printer()

	

