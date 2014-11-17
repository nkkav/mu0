/***************************************************************/
/*  File       : mu0_isa.cpp
 *  Description: Instruction behaviors.
 *  Author     : Nikolaos Kavvadias <nikos@nkavvadias.com>
 *
 *  Copyright (C) 2014 Nikolaos Kavvadias
 *
 *  Author affiliation:
 *    Dr. Nikolaos Kavvadias
 *    Independent Consultant -- Research Scientist
 *    Kornarou 12 Rd., Nea Ampliani,
 *    35100 Lamia, Greece
 *                                                             
 *  Contact information for the ArchC team:                     
 *    The ArchC Team                                              
 *    Computer Systems Laboratory (LSC)                           
 *    IC-UNICAMP                                                  
 *    http://www.lsc.ic.unicamp.br                                
 */
/***************************************************************/

#include  "mu0_isa.H"
#include  "mu0_isa_init.cpp"
#include  "mu0_bhv_macros.H"
#include  <stdio.h>
#include  <ctype.h>

//If you want debug information for this model, uncomment next line.
#define DEBUG_MODEL
#include "ac_debug_model.H"

using namespace mu0_parms;

inline void update_cycle(ac_reg<unsigned short int>& ac_cycle, 
  unsigned short int increment)
{
  ac_cycle += increment;
}

//!Generic instruction behavior method.
void ac_behavior( instruction )
{
  dbg_printf("----- Cycle %#d -----\n", (int)CYC);
  dbg_printf("----- PC=%#x ----- %lld\n", (int)ac_pc, ac_instr_counter);
#ifndef NO_NEED_PC_UPDATE
  ac_pc = ac_pc + 2;
#endif  
};

//! Instruction Format behavior methods.
void ac_behavior( Type_I )
{
  PC = ac_pc;
}

//!Behavior called before starting simulation.
void ac_behavior(begin)
{
  dbg_printf("@@@ begin behavior @@@\n");
  ACC   = 0;
  IR    = 0;
  PC    = 0;
  ac_pc = 0x00;
  CYC   = 0x00000000;
}

//!Behavior called after finishing simulation.
void ac_behavior(end)
{
  int i;

  dbg_printf("@@@ end behavior @@@\n");
  printf("----- Cycle %#d -----\n", (int)CYC);
  // Report register contents.
  dbg_printf("@@@ Reporting register data @@@\n");
  //
  dbg_printf("ACC = %#x\n", ACC.read());
  dbg_printf("IR  = %#x\n", IR.read());
  dbg_printf("PC  = %#x\n", PC.read());
}
        
//!Instruction lda behavior method.
void ac_behavior( lda )
{
  update_cycle(CYC, 2);
  dbg_printf("ACC = mem[%d]\n", imm);
  ACC = MEM.read(imm);
  dbg_printf("Result = %#x\n", ACC.read());
};

//!Instruction sto behavior method.
void ac_behavior( sto )
{
  update_cycle(CYC, 2);
  dbg_printf("mem[%d] = ACC\n", imm);
  MEM.write(imm, ACC);
  dbg_printf("Result = %#x\n", MEM.read(imm));
};

//!Instruction add behavior method.
void ac_behavior( add )
{
  update_cycle(CYC, 2);
  dbg_printf("ACC = ACC + mem[%d]\n", imm);
  ACC = ACC + MEM.read(imm);
  dbg_printf("Result = %#x\n", ACC.read());
};

//!Instruction sub behavior method.
void ac_behavior( sub )
{
  update_cycle(CYC, 2);
  dbg_printf("ACC = ACC - mem[%d]\n", imm);
  ACC = ACC - MEM.read(imm);
  dbg_printf("Result = %#x\n", ACC.read());
};

//!Instruction jmp behavior method.
void ac_behavior( jmp )
{
  update_cycle(CYC, 1);
  dbg_printf("PC = %d\n", imm);
  ac_pc = imm;
  dbg_printf("Target = %#x\n", ac_pc.read());
};

//!Instruction jge behavior method.
void ac_behavior( jge )
{
  update_cycle(CYC, 1);
#ifndef JGE_IS_JGT
  dbg_printf("if ACC>=0 pc = %d\n", imm);
#else
  dbg_printf("if ACC>0 pc = %d\n", imm);
#endif
#ifndef JGE_IS_JGT
  if (ACC >= 0) {
#else
  if (ACC > 0) {
#endif
    ac_pc = imm;
  }
  dbg_printf("Target = %#x\n", ac_pc.read());
};

//!Instruction jne behavior method.
void ac_behavior( jne )
{
  update_cycle(CYC, 1);
  dbg_printf("if ACC!=0 pc = %d\n", imm);
  if (ACC != 0) {
    ac_pc = imm;
  }
  dbg_printf("Target = %#x\n", ac_pc.read());
};

//!Instruction stp behavior method.
void ac_behavior( stp )
{
  update_cycle(CYC, 1);
  dbg_printf("stp\n");
  stop();
}
