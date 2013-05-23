-- Not to be used in any design, only for testing purposes!
-- This test case presents strange whitespaces

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.std_logic_unsigned."+";
use IEEE.std_logic_arith.conv_unsigned;

entity foo is
	-- generic declaration yhdellä rivillä
	generic (local_memory_start_addr : integer := 16#1000#);
	
	-- sama portille
	port (clk: in std_logic);
end foo;
