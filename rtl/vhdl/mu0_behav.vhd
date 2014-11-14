library IEEE;
use STD.textio.all;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;
use WORK.std_logic_textio.all;


entity mu0 is
  generic (
    MAXWIDTH : integer := 16;
    MAXDEPTH : integer := 12
  );
  port (
    clk      : in  std_logic;
    reset    : in  std_logic;
    pc       : out std_logic_vector(MAXWIDTH-1 downto 0);
    ir       : out std_logic_vector(MAXWIDTH-1 downto 0);
    acc      : out std_logic_vector(MAXWIDTH-1 downto 0)
  );
end entity mu0;

architecture behavioral of mu0 is 
  -- Type declarations.
  type mem_type is array(0 to 2**MAXDEPTH-1) of 
    std_logic_vector(MAXWIDTH-1 downto 0);
  -- According to XST User Manual, v14.1, p. 239.
  impure function InitRamFromFile(RamFileName : in string) 
    return mem_type is
    FILE RamFile : text is in RamFileName;
    variable RamFileLine : line;
    variable ram : mem_type;
  begin
    for i in mem_type'RANGE loop
      readline(RamFile, RamFileLine);
      hread(RamFileLine, ram(i));
    end loop;
    return ram;
  end function InitRamFromFile;
  -- Constant declarations.
  constant LDA : std_logic_vector(3 downto 0) := "0000"; 
  constant STO : std_logic_vector(3 downto 0) := "0001"; 
  constant ADD : std_logic_vector(3 downto 0) := "0010"; 
  constant SUB : std_logic_vector(3 downto 0) := "0011"; 
  constant JMP : std_logic_vector(3 downto 0) := "0100"; 
  constant JGE : std_logic_vector(3 downto 0) := "0101"; 
  constant JNE : std_logic_vector(3 downto 0) := "0110"; 
  constant STP : std_logic_vector(3 downto 0) := "0111"; 
  -- Signal declarations.
  signal opcode   : std_logic_vector(MAXWIDTH-1 downto MAXWIDTH-4);
  signal address  : std_logic_vector(MAXWIDTH-5 downto 0);
  -- Initialize memory using impure function.
  signal mem      : mem_type := InitRamFromFile("prog.lst");
  signal pc_reg   : std_logic_vector(MAXWIDTH-1 downto 0);
  signal pc_next  : std_logic_vector(MAXWIDTH-1 downto 0);
  signal ir_reg   : std_logic_vector(MAXWIDTH-1 downto 0);
  signal ir_next  : std_logic_vector(MAXWIDTH-1 downto 0);
  signal acc_reg  : std_logic_vector(MAXWIDTH-1 downto 0);
  signal acc_next : std_logic_vector(MAXWIDTH-1 downto 0);
begin

  -- Synchronous logic.
  process (clk, reset)
  begin
    if (reset = '1') then
      pc_reg  <= (others => '0');
      ir_reg  <= (others => '0');
      acc_reg <= (others => '0');
    elsif (rising_edge(clk)) then
      pc_reg  <= pc_next;
      ir_reg  <= ir_next;
      acc_reg <= acc_next;
    end if;
  end process;

  -- Combinational logic.
  process (
    pc_reg, pc_next,
    ir_reg, ir_next,
    acc_reg, acc_next
  )
    variable opcode_v : std_logic_vector(3 downto 0);
  begin
    pc_next  <= pc_reg;
    ir_next  <= ir_reg;
    acc_next <= acc_reg;
    --    
    opcode  <= ir_reg(MAXWIDTH-1 downto MAXWIDTH-4);
    address <= ir_reg(MAXWIDTH-5 downto 0);
    opcode_v := opcode;
    case (opcode_v) is
      when LDA => 
        acc_next <= mem(to_integer(unsigned(address)));
      when STO =>
        mem(to_integer(unsigned(address))) <= acc_reg;
      when ADD =>
        acc_next <= std_logic_vector(unsigned(acc_reg) + 
                    unsigned(mem(to_integer(unsigned(address)))));
      when SUB =>
        acc_next <= std_logic_vector(unsigned(acc_reg) - 
                    unsigned(mem(to_integer(unsigned(address)))));
      when JMP =>
        pc_next  <= (MAXWIDTH-1 downto MAXWIDTH-4 => '0') & address;
      when JNE =>
        if (acc_reg /= std_logic_vector(to_unsigned(0,MAXWIDTH))) then
          pc_next <= (MAXWIDTH-1 downto MAXWIDTH-4 => '0') & address;
        end if;
      when JGE =>
        if (acc_reg(MAXWIDTH-1) = '0') then
          pc_next <= (MAXWIDTH-1 downto MAXWIDTH-4 => '0') & address;
        end if;
      when STP =>
        pc_next <= pc_reg;
      when others =>
        pc_next <= pc_reg;
    end case;
    --
    ir_next  <= mem(to_integer(unsigned(pc_reg)));
    if (ir_reg(MAXWIDTH-1 downto MAXWIDTH-4) /= STP) then
      pc_next <= std_logic_vector(unsigned(pc_reg) + 1);
    end if;    
  end process;
    
  -- Assign outputs.
  pc  <= pc_reg;
  ir  <= ir_reg;
  acc <= acc_reg;

end architecture behavioral;
