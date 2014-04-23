-- Not to be used in any design, only for testing purposes!
-- This test case presents case where there are several generic and port declarations on one entity

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.std_logic_unsigned."+";
use IEEE.std_logic_arith.conv_unsigned;

entity foo is
	generic (
		  local_memory_start_addr : integer := 16#1000#;    -- upper 16 bits of address
		  local_memory_addr_bits  : integer := 12;          -- number of address bits
		  code_file               : string  := "master.tbl" -- file to read commands from
	);
	port (
		clk: in std_logic;
		addr: in std_logic_vector(11 downto 0);
		data: out std_logic_vector(31 downto 0)
	);
	generic (
		  local_memory_start_addr2 : integer := 16#1000#;    -- upper 16 bits of address
		  local_memory_addr_bits2  : integer := 12;          -- number of address bits
		  code_file2               : string  := "master.tbl" -- file to read commands from
	);
	port (
		clk2: in std_logic;
		addr2: in std_logic_vector(11 downto 0);
		data2: out std_logic_vector(31 downto 0)
	);
end foo;
