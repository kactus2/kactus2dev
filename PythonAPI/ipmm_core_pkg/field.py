
#<ipxact:field>
#            <ipxact:name>RXFIFO_NE</ipxact:name>
#            <ipxact:description>RX-FIFO Not Empty. This interrupt capable status flag indicates the RX-FIFO status and associated interrupt status before the enable stage. The flag can only be implicitly cleared by reading the RXFIFO_DAT register</ipxact:description>
#            <ipxact:bitOffset>0</ipxact:bitOffset>
#            <ipxact:bitWidth>1</ipxact:bitWidth>
#            <ipxact:access>read-only</ipxact:access>
#

from ipmm_core_pkg.primitive import Primitive
from ipmm_core_pkg.reset import Reset


class Field(Primitive):

	def __init__(self, name, description, bitOffset, bitWidth, access):
		pass
		Primitive.__init__(self, name, description)
		self.bitOffset = bitOffset
		self.bitWidth = bitWidth
		self.access = access
		self.resets = []
		
	def get_bitOffset(self):
		return self.bitOffset
		
	def get_bitWidth(self):
		return self.bitWidth
		
	def get_access(self):
		return self.access

	def add_reset(self, reset):
		if type(reset) is not Reset:
			print("Type of add_reset parameter is not Reset")
		else:
			self.resets.append(reset)
		return 0
	def get_reset_by_type(self, typeName):
		found = 0
		for rst in self.resets:
			if rst.get_resetType() is "HW":
				found = 1
				print("get_reset_by_type found", typeName)
				return rst
		return found

		
	def printer(self):
		Primitive.printer(self)
		print("bitOffset: ", self.get_bitOffset())
		print("bitWidth: ", self.get_bitWidth())
		print("access: ", self.get_access())
		for rst in self.resets:
			rst.printer()

# Stand-a-lone testing
if __name__ == "__main__":	
	field_1 = Field("RX-FIFO", "RX-FIFO Not Empty.", 0, 1, "read-only")
	reset_1 = Reset("rstm", "Reset", "HW", 1, 1)
	field_1.add_reset(reset_1)
	field_1.printer()

	reset_2 = field_1.get_reset_by_type("HW")
	reset_2.printer()

