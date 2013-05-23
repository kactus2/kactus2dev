-- Not to be used in any design, only for testing purposes!
-- This test case presents non-standard generic types

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.std_logic_unsigned."+";
use IEEE.std_logic_arith.conv_unsigned;

entity foo is
	generic (
		  local_memory_start_addr : integer := 16#1000#;    -- upper 16 bits of address
		  local_memory_addr_bits  : integer := 12;          -- number of address bits
		  code_file               : string  := "master.tbl"; -- file to read commands from
		  width_g	: integer := 8; -- leveys
		  foo_g : omituinen_tyyppi := "jotain" -- kommentti oudolle
	);
	port (
		clk: in std_logic
		);
end foo;
