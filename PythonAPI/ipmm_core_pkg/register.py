#<ipxact:register>
#          <ipxact:name>STAT</ipxact:name>
#          <ipxact:description>Status register. Collection of Status flags including interrupt status before enabling</ipxact:description>
#          <ipxact:addressOffset>'h0</ipxact:addressOffset>
#          <ipxact:size>32</ipxact:size>

from ipmm_core_pkg.primitive import Primitive
from ipmm_core_pkg.field import Field

class Register(Primitive):

	def __init__(self, name, description, addressOffset, size):
		pass
		Primitive.__init__(self, name, description)
		self.addressOffset = addressOffset
		self.size = size
		self.fields = []
		
	def get_addressOffset(self):
		return self.addressOffset

	def get_size(self):
		return self.size

	def add_field(self, field):
		if type(field) is not Field:
			print("Type of add_field parameter is not Field")
		else:
			self.fields.append(field)
		return 0


	def printer(self):
		Primitive.printer(self)
		print("addressOffset: ", self.get_addressOffset())
		print("size: ", self.get_size())
		for f in self.fields:
			f.printer()


# Stand-a-lone testing
if __name__ == "__main__":
	from ipmm_core_pkg.reset import Reset
	register_1 = Register("STAT", "Status register.", 0, 32)
	field_1 = Field("RX-FIFO", "RX-FIFO Not Empty.", 0, 1, "read-only")
	field_2 = Field("RXFIFO_OVFL", "X-FIFO Overflow.", 1, 1, "read-write")
	reset_1 = Reset("rstm", "Reset", "HW", 1, 1)
	field_1.add_reset(reset_1)
	register_1.add_field(field_1)
	register_1.add_field(field_2)

	register_1.printer()


	
	
