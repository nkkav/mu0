/* COMPILE_MU0.C
 *
 * Compiles code for the ficticious "MU0" processor.
 * To be executed on the EXECUTE_MU0 emulator program.
 *
 * Version 1, 10 October 1994
 * Version 1.01, 3 March 1995 - Tidied up for Project
 * Version 2.00, 14 November 2014 - Modifications by 
 *   Nikolaos Kavvadias <nikos@nkavvadias.com>
 *
 * (C) 1994, 1995 Benjy (Soft Eng A2)
 */

/* Include Files */

#include <stdio.h>
#include <string.h>

/* Number of bits for a memory address. */
#ifndef MAXDEPTH
#define MAXDEPTH    12
#endif

/* On the UNIX machines, int is long enough. Doing it like this
   allows us to upgrade all addresses to long if needed on other
   machines */

typedef int address;

/* To store symbol table from first pass for forward referencing */

struct {
         char id[32];
         address address;
       } labels[128];

/* Function prototypes */

int compile(int pass,char *source_filename,char *dest_filename);
int readline(FILE *file,char *string);
int getvalue (char *operand);
void fillzeroes(FILE *file, int addr);
int main();

int main()
{
  char source_filename[50],
       dest_filename[50];

  printf ("COMPILE_MU0 - companion program to EXECUTE_MU0\n");
  printf ("(C) 1994 Benjy\n\n");
  printf ("Please enter source filename > ");  /* Get filenames */
  scanf ("%s",source_filename);
  printf ("Please enter destination filename > ");
  scanf ("%s",dest_filename);
  
  printf ("\nCommencing compilation...\n\n");

  if (compile(1,source_filename,dest_filename))   /* Pass 1 */
      compile(2,source_filename,dest_filename) ;  /* No errors - Pass 2 */
  
  return(1);
}


/* Main compile function. Parameters are source and destination
   filename, and the pass we're doing - 1 is process labels only
   and 2 is generate code */
int compile(int pass,char *source_filename,char *dest_filename)
{
  FILE *infile,
       *outfile;
  int i;
  int loop,
      nextlabel=0,
      address=0;
      char line[100],
           command[50],
           operand[50];

  printf ("Pass %d\n",pass);
  printf ("Opening source file %s\n",source_filename);

  if ((infile=fopen (source_filename,"r"))==0) {  /* Open source */
    printf ("ERROR: Can't open %s\n",source_filename);
    return(0);
  }

  if (pass==1) {   /* On first pass need to clear symbol table */
    printf ("Clearing label index\n");

    for (loop=0;loop<128;loop++) {
      labels[loop].id[0]=0;
      labels[loop].address=0;
    }
  } else {   /* Or on second pass, open dest. file */
    printf ("Opening destination file %s\n",dest_filename);

    if ((outfile=fopen (dest_filename,"wb"))==0) {

      printf ("ERROR: Can't open %s\n",dest_filename);
      fclose(infile);
      return(0);
    }
  }

  while(1) { /* Loop forever (EOF checking done below */
    if (!readline(infile,line)) {   /* Read in line */
      printf ("ERROR: Unexpected EOF\n");  /* EOF without end marker? */
      fclose(infile); 
      if (pass==2) fclose (outfile);  /* Close outfile only if needed */
      return(0);
    }
   
    if (pass==2) printf ("\n[%03X] %-30s",address,line);
              /* Print out instruction on second pass only */

    command[0]=operand[0]=0;
    sscanf (line,"%s%s",command,operand);  /* Get command & operand */

    if (!strcmp(command,".word")) {   /* .word pseudo-operation */
      if (pass==2) {
        int byte;
        sscanf (operand,"%x",&byte);
        printf ("%04X",byte);   /* Display the word */
        /* Generate hexadecimal listing instead. */
#ifdef HEX
        fprintf(outfile, "%04x\n", byte);
#else        
        fputc(byte/256,outfile);  /* Output the word to file */
        fputc(byte%256,outfile);
#endif
      }
      address++;
    }
 
    if (!strcmp(command,".end")) {    /* .end, end of file marker */
      printf ("\nEnd of file marker, pass %d complete\n\n",pass);
      fclose(infile);
      if (pass==2) { 
#ifdef HEX      
        fillzeroes(outfile, address);
#endif
        fclose (outfile);
      }
      return(1);
    }
    
    if (!strcmp(command,".label")) {   /* .label, this is a label!! */
      if (pass==1) {
        printf ("Label %d at address [%03X]: %s\n",nextlabel+1,address,operand);
        strcpy (labels[nextlabel].id,operand);  /* Put it in symbol table */
        labels[nextlabel].address=address;
        nextlabel++;
      }
    }

    if (pass==1) {    /* On pass 1 don't assemble, just check validity */

      if (!strcmp(command,"ACC<=") ||
          !strcmp(command,"ACC=>") ||
          !strcmp(command,"ACC+") ||
          !strcmp(command,"ACC-") ||
          !strcmp(command,"PC<=") ||
          !strcmp(command,"IF+VE") ||
          !strcmp(command,"IF!=0") ||
          !strcmp(command,"STOP"))
      {
        address++;
      }
    } else {     /* Pass 2, actually assemble. */
      if (strcmp(command,".word") &&   /* Already done */
          strcmp(command,".end") &&
          strcmp(command,".label") &&
          strcmp(command,";") &&
          command[0]) {
      int opcode=-1,value=-1,byte;

      if (!strcmp(command,"ACC<=")) {  /* Load accumulator */
        opcode=0;
        value=getvalue(operand+1);
      }

      if (!strcmp(command,"ACC=>")) {  /* Store accumulator */
        opcode=1;
        value=getvalue(operand+1);
      }

      if (!strcmp(command,"ACC+")) {  /* Add accumulator */
        opcode=2;
        value=getvalue(operand+1);
      }

      if (!strcmp(command,"ACC-")) {  /* Subtract accumulator */
        opcode=3;
        value=getvalue(operand+1);
      }
 
      if (!strcmp(command,"PC<=")) {  /* Jump to new address */
        opcode=4;
        value=getvalue(operand);
      }
 
      if (!strcmp(command,"IF+VE")) {  /* If positive */
        opcode=5;
        sscanf (line,"%s%s%s",command,operand,operand);
        value=getvalue(operand);
      }

      if (!strcmp(command,"IF!=0")) {  /* If not equal to 0 */
        opcode=6;
        sscanf (line,"%s%s%s",command,operand,operand);
        value=getvalue(operand);
      }

      if (!strcmp(command,"STOP")) {  /* Halt processor */
        opcode=7;
        value=0;
      }

      if (opcode==-1) {  /* Unknown command, but keep going */
        printf ("ERROR: Unknown command - ");
        opcode=15;
        value=0;
      }

      printf ("%X%03X",opcode,value);     /* Display it on screen */

      byte=(opcode << 4) + (value >> 8);  /* Calculate first byte */
      /* Generate hexadecimal listing instead. */
#ifdef HEX
      fprintf(outfile, "%02x", byte);
#else              
      fputc(byte,outfile);  /* And output it */
#endif
      byte=value & 0xFF;    /* Calculate second byte */
      /* Generate hexadecimal listing instead. */
#ifdef HEX
      fprintf(outfile, "%02x\n", byte);
#else              
      fputc(byte,outfile);  /* And output it */
#endif
      address++;  /* Increment address */
    }
  }
  }
}

int getvalue (char *operand)  /* Gets the operand */
{
  if (operand[strlen(operand)-1]==']')  /* Ignore trailing ] */
    operand[strlen(operand)-1]=0;

  if (operand[0]=='$') {  /* Actual value */
    int value;

    sscanf (&(operand[1]),"%x",&value);
    return(value);
  } else {  /* If no $, must be a label */
    int loop;
    
    for (loop=0;loop<128;loop++) {  /* Find label in table */
      if (!strcmp(labels[loop].id,operand))  
        return (labels[loop].address);  /* Get address */
    }

    printf ("ERROR: Unknown Label - %s - ",operand);  /* Label not found */
    return(0);
  }
}  

int readline(FILE *file,char *string)  /* Read line from file */
{
  int index=0,
      byte;
 
  string[0]=index;  /* Clear string */

  while (!feof(file)) {  /* While file left */
    byte=fgetc(file);  /* Get byte */
    if (byte=='\n' || byte=='\r') { /* End of Line */
      return (1);
    }
    string[index++]=byte;  /* Chuck it in string */
    string[index]=0;
  }
  return(0);
}

void fillzeroes(FILE *file, int addr)
{
  int i;
  /* Fill unused addresses with zeroes (up to word 4095). */
  for (i = addr; i < (1<<MAXDEPTH); i++) {
    fprintf(file, "%04x\n", 0x0000);
  }
}
