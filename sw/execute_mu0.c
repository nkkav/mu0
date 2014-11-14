/* EXECUTE_MU0.C
 * Executes code for the ficticious "MU0" processor
 * Code has been compiled with the COMPILE_MU0 program
 *
 * Version 1, 10 October 1994
 * Version 1.01, 6 March 1995 - Tidied up for Project
 *
 * (C) 1994, 1995 Benjy (Soft Eng A2)
 */

/* Include files */

#include <stdio.h>
#include <ctype.h>
#include <string.h>

/* Type for memory address, in same way as for COMPILE_MU0 */

typedef int address;

/* We'll use capital letters to indicate the "memory" and "accumulator"
   within the virtual MU0 processor. MU0 specs limit us to 4096 words
   memory (each 16 bits) and the accumulator. */

address RAM[4096];
signed int ACC=0;

/* Function prototypes */

int main();
void help ();
void execute(int mode);

int main()
{
  char command[50];
  int commandok,
      loop;

  printf ("EXECUTE_MU0 - companion program to COMPILE_MU0\n");
  printf ("(C) 1994 Benjy\n\n");

  for (loop=0;loop<4096;loop++)  /* Clear memory */
    RAM[loop]=0;

  printf ("Enter \"help\" for command list\n\n");

  while (1) {     /* Eternal loop to get commands */
    printf (">");
    scanf ("%s",command);
    commandok=0;

    if (!strcmp(command,"help")) {  /* Command help */
      help();
      commandok=1;
    }
  
    if (!strcmp(command,"exit")) {  /* Exit program */
      break;
    }

    if (!strcmp(command,"view")) {  /* View one address */
      address address;
      scanf ("%x",&address);
      printf ("[%03X] : %04X\n",address,RAM[address]);
      commandok=1;
    }

    if (!strcmp(command,"set")) {  /* Set an address to a value */
      address address;
      int value;
      scanf ("%x%x",&address,&value);  /* Get address and value */
      RAM[address]=value;
      printf ("[%03X] : %04X\n",address,RAM[address]);
      commandok=1;
    }
    
    if (!strcmp(command,"viewacc")) { /* View the accumulator */
      printf ("Accumulator : %04X\n",ACC);
      commandok=1;
    }

    if (!strcmp(command,"setacc")) {  /* Set the accumulator */
      int value;
      scanf ("%x",&value);
      ACC=value;
      printf ("Accumulator : %04X\n",ACC);
      commandok=1;
    }

    if (!strcmp(command,"go")) {  /* Execute code */
      execute(1);  /* Parameter indicates show execution */
      commandok=1;
    }

    if (!strcmp(command,"gostep")) {  /* Execute code in step mode */
      execute(2);  /* Parameter indicates step mode */
      commandok=1;
    } 

    if (!strcmp(command,"dump")) {  /* Dump a large chunk of memory */
      int low,high,loop;
      scanf ("%x%x",&low,&high);
      for (loop=low;loop<=high;loop++) {  /* Loop through memory */
        if (!((loop-low)%8))
          printf ("\n[%03X]  ",loop);  /* All this for nice layout */

        if (((loop-low)%8)==4) 
          printf (" ");

        printf ("%04X ",RAM[loop]);
      }
      printf ("\n");
      commandok=1;
    }

    if (!strcmp(command,"goquiet")) {  /* Execute code */
      execute(0);  /* This time without any display */
      commandok=1;
    }

    if (!strcmp(command,"load")) {  /* Load "executable" code into "RAM" */
      address address=0;
      unsigned char highb,
                    lowb;
      char filename[50]; 
      FILE *infile;
      scanf ("%s",filename);
      
      if ((infile=fopen (filename,"rb"))==0) {  /* Open file */
        printf ("Can't find file %s\n",filename);  /* Not found */
      } else {
        while (!feof(infile)) {  /* Loop until end of file */
          highb=fgetc(infile);   /* Get bytes */
          lowb=fgetc(infile);
          if (!feof(infile)) {
            RAM[address]=(highb<<8) + lowb;  /* If not EOF, load them */
            printf ("[%03X] : %04X\n",address,RAM[address]);  /* Show it */
            address++;
    }
        }
        fclose (infile);  /* Close */
      }
      commandok=1;
    }

    if (!commandok) {  /* Bad command entered */
      printf ("Unknown command\n");
    }
  }



  printf ("Done\n\n");

  return(0);
}

void help()  /* Function simply lists commands with a bit of info */
{
  printf ("EXECUTE_MU0 command list\n\n");
  printf ("load <filename>          Load MU0 executable code at 000\n");
  printf ("go                       Execute code from 000 with display\n");
  printf ("goquiet                  Execute code from 000 without display\n");
  printf ("gostep                   Execute code from 000 in step mode\n");
  printf ("set <address> <value>    Set address to value\n");
  printf ("view <address>           View value at address\n");
  printf ("viewacc                  View the accumulator\n");
  printf ("setacc <value>           Set the accumulator\n");
  printf ("dump <lowadd> <hiadd>    View memory from lowadd to hiadd\n");
  printf ("help                     View command list\n");
  printf ("exit                     Exit program\n\n");
  printf ("All addresses are 3 digit hex, values are 4 digit hex\n");
  printf ("Program load and execution 000\n\n");
} 

/* Main execution function. Returns nothing. Takes one parameter, which
   can be zero to two. Zero indicates do a quiet execution, with no display.
   One indicates trace execution, showing each address, instruction and
   accumulator contents. Two indicates step mode (wait for key after each
   instruction has been executed.
*/

void execute(int mode)
{
  address PC=0;  /* Program counter */
  int PI,  /* Current instruction being executed */
      oppcode,
      operand;

  while(1) {
    PI=RAM[PC];  /* Fetch instruction from RAM */
    if (mode) printf ("PC : %03X  PI : %04X  ACC : %04X",PC,PI,ACC);
         /* Display in mode 1 or mode 2 */

    oppcode=PI/4096;  /* Separate opcode and operand */
    operand=PI%4096;

    switch (oppcode) {  /* Check for opcode */
    case 0:
      ACC=RAM[operand];  /* ACC<= */
      break;

    case 1:
      RAM[operand]=ACC;  /* ACC=> */
      break;

    case 2:
      ACC+=RAM[operand];  /* ACC+ */
      break;

    case 3:
      ACC-=RAM[operand];  /* ACC- */
      break;

    case 4:  
      PC=operand-1;   /* PC<=  (-1 because we increment later) */
      break;

    case 5:
      if (ACC>0) PC=operand-1;  /* IF+VE */
      break;

    case 6:
      if (ACC!=0) PC=operand-1;  /* IF!=0 */
      break;

    case 7:  /* STOP (halt processor */
      printf ("\n");
      return;

    default:  /* Bad opcode found */
      printf ("Unknown opcode - %X\n",oppcode);
    }

    PC++;  /* Increment program counter */
  
    if (mode==1)   /* We're in trace mode, so need a blank line */
      printf ("\n");

    if (mode==2) {  /* Single step mode */
      char ln[10];  /* To hold the inputed line */
      printf ("   <Q=stop, T=trace, G=execute, S=continue >\n");

      scanf ("%s",ln);

      switch (toupper(ln[0])) {
      case 'Q':  /* Stop */
        printf ("\n");
        return;

      case 'T':  /* Trace mode */
        mode=1;
        break;


      case 'G':  /* Execute to end quietly */
        mode=0;
        break;
 
      case 'S':
        break;  /* Continue, do nothing special */
      }
    }
  }
}
