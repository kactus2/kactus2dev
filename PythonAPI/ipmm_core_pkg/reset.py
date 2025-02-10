# ipxact:reset


from ipmm_core_pkg.primitive import Primitive

class Reset(Primitive):

	def __init__(self, name, description, resetType, value, mask):
		pass
		Primitive.__init__(self, name, description)
		self.resetType = resetType
		self.value = value
		self.mask = mask
				
	def get_resetType(self):
		return self.resetType
		
	def get_value(self):
		return self.value
		
	def get_mask(self):
		return self.mask

	def printer(self):
		Primitive.printer(self)
		print("resetType: ", self.get_resetType())
		print("value: ", self.get_value())
		print("mask: ", self.get_mask())

# Stand-a-lone testing
if __name__ == "__main__":	
	reset_1 = Reset("rstm", "Reset", "HW", 1, 1)
	reset_1.printer()
