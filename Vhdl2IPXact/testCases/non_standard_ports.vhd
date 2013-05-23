-- Not to be used in any design, only for testing purposes!
-- This test case presents non-standard port types and different port directions

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.std_logic_unsigned."+";
use IEEE.std_logic_arith.conv_unsigned;

entity foo is
	port (
	
		-- perus skalaariportit ja kommentointi
		clk_in: in std_logic := '1';			-- kuuluu portille clk_in
		-- ei tule mihinkään porttiin
		clk_out	:out std_logic;
		-- ei tule mihinkään porttiin
		clk_inout: 		inout std_logic;
		
		-- perus vektoroidut portit
		data_in: in std_logic_vector(31 downto 0) :="1010000";
		data_out		: out std_logic_vector(15 downto 0);
		data_inout : inout std_logic_vector(7 downto 0);
		
		-- erikoisemmat tyypit porteille
		addr_in			:	in	std_ulogic_vector(7 downto 0);
		addr_out:out std_logic_vector(0 downto 0); -- ei toimi mutta ei saa kaataa
		addr_inout	:inout	std_ulogic_vector(3 downto 2);
		id_in	:	in STRING (4 downto 0) := "START";
		
		-- omat tyypit skalaariporteille
		clock_in : in clk_type;	
		clock_out	: out clk_type;
		clock_inout :  inout clk_type;
		
		-- omat tyypit vektoroiduille porteille
		foo_in : in vayla(4 downto 1);
		foo_out : out vayla(3 downto 0);
		foo_inout : inout vayla(2 downto 1);
	);
end foo;

architecture struct of foo is

   for all: ahbmst
       use entity leon2dma_lib.ahbmst(rtl);

type dma_state_type is (readc, writec);
type reg_type is record
  srcaddr : std_logic_vector(31 downto 0);
  status  : std_logic_vector(1 downto 0);
  dstate  : dma_state_type;
  data    : std_logic_vector(31 downto 0);
end record;

signal r, rin : reg_type;
signal dmai : ahb_dma_in_type;
signal dmao : ahb_dma_out_type;

begin

  comb : process(apbi, dmao, rst, r)
  variable v       : reg_type;
  variable regd    : std_logic_vector(31 downto 0);   -- data from registers
  variable start   : std_logic;
  variable burst   : std_logic;
  variable write   : std_logic;
  variable ready   : std_logic;
  variable retry   : std_logic;
  variable mexc    : std_logic;
  variable irq     : std_logic;
  variable address : std_logic_vector(31 downto 0);   -- DMA address
  variable size    : std_logic_vector( 1 downto 0);   -- DMA transfer size
  variable newlen  : std_logic_vector(7 downto 0);
  variable oldaddr : std_logic_vector(9 downto 0);
  variable newaddr : std_logic_vector(9 downto 0);
  variable oldsize : std_logic_vector( 1 downto 0);
  variable ainc    : std_logic_vector( 3 downto 0);

  begin

    v := r; regd := (others => '0'); burst := '0'; start := '0';
    write := '0'; ready := '0'; mexc := '0'; address := r.srcaddr;
    size := r.srcinc; irq := '0';

-- pragma translate_off
    if not is_x(r.len) then
-- pragma translate_on
      newlen := r.len - 1;
-- pragma translate_off
   end if;
-- pragma translate_on

  regs : process(clk)
  begin if rising_edge(clk) then r <= rin; end if; end process;

end;

