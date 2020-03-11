

from ipmm_core_pkg.primitive import Primitive
from ipmm_core_pkg.addressBlock import AddressBlock
from ipmm_core_pkg.register import Register
from ipmm_core_pkg.field import Field
from ipmm_core_pkg.port import Port
from ipmm_core_pkg.parameter import Parameter

        
class Component(Primitive):

        def __init__(self, name, description):
            pass
            Primitive.__init__(self, name, description)
            self.parameters = []
            self.ports = []
            self.buses = []
            self.constants = []
            self.signals = []
            self.memoryMaps = []
            self.behavior = []
            self.renderers = []
            
        #TODO: type checkings for add_
        def add_parameter(self, parameter):
            self.parameters.append(parameter)

        def add_port(self, port):
            self.ports.append(port)

        def add_constant(self, constant):
            self.constants.append(constant)
        

        def add_memoryMap(self, memoryMap):
            self.memoryMaps.append(memoryMap)

        def add_renderer(self, renderer):
            self.renderers.append(renderer)

        ## render can manipulate (remove, add, modify) parameters, ports, memorymaps    
        def render(self):
            for r in self.renderers:
                r()

        def printer(self):
            Primitive.printer(self)

            for p in self.ports:
                p.printer()

            for c in self.constants:
                c.printer()


            for mm in self.memoryMaps:
                mm.printer()


# Stand-a-lone testing
if __name__ == "__main__":	


        register_1 = Register("STAT", "Status register.", 2, 32)
        field_1 = Field("RX-FIFO", "RX-FIFO Not Empty.", 0, 1, "read-only")
        field_2 = Field("RXFIFO_OVFL", "X-FIFO Overflow.", 1, 1, "read-write")
        
        ## move this to rgister_if class
        port_1 = Port(field_1.get_name()+"_input", "RX-FIFO Not Empty inout", "logic", field_1.get_bitWidth(), "input")
        port_2 = Port(field_2.get_name()+"_output", "RX-FIFO Overflow output", "logic", field_2.get_bitWidth(), "output")


        register_2 = Register("CMD", "Command register.", 4, 32)
        field_3 = Field("Command", "Peripheral Command.", 0, 4, "read-write")
    
     
        register_1.add_field(field_1)
        register_1.add_field(field_2)
        
        register_2.add_field(field_3)

        addressBlock_1 = AddressBlock("PP_Address_Block", "Peripheral address block", 32, 32)
        addressBlock_1.add_register(register_1)
        addressBlock_1.add_register(register_2)


        component_1 = Component("Helloder", "This components is Helloder")
        component_1.add_port(port_1)
        component_1.add_port(port_2)

        component_1.add_memoryMap(addressBlock_1)
        

        component_1.printer()
