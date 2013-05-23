-- Not to be used in any design, only for testing purposes!
-- This test case presents strange whitespaces

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.std_logic_unsigned."+";
use IEEE.std_logic_arith.conv_unsigned;

entity foo is
	-- monta genericiä yhdellä rivillä
	generic (local_memory_start_addr : integer := 16#1000#;local_memory_addr_bits  : integer := 12;code_file :string:="master.tbl";  width_g	: integer := 8);
	port (clk: in std_logic;addr: in std_logic_vector(11 downto 0);	data: out std_logic_vector(31 downto 0);intr: out std_logic);
end foo;
