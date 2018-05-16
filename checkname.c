#undef BOOMER
#define PHONETICS
#define ALIGNMENTS
/************************************************************************/
/**

   Program:    checkname
   \file       checkname.c
   
   \version    V0.1
   \date       02.05.18   
   \brief      Checks an antibody name for possible conflicts
   
   \copyright  (c) UCL / Dr. Andrew C. R. Martin 2018
   \author     Dr. Andrew C. R. Martin
   \par
               Institute of Structural & Molecular Biology,
               University College,
               Gower Street,
               London.
               WC1E 6BT.
   \par
               andrew@bioinf.org.uk
               andrew.martin@ucl.ac.uk
               
**************************************************************************

   This program is not in the public domain, but it may be copied
   according to the conditions laid out in the accompanying file
   COPYING.DOC

   The code may be modified as required, but any modifications must be
   documented so that the person responsible can be identified.

   The code may not be sold commercially or included as part of a 
   commercial product except as described in the file COPYING.DOC.

**************************************************************************

   Description:
   ============

**************************************************************************

   Usage:
   ======

**************************************************************************

   Revision History:
   =================

*************************************************************************/
/* Includes
*/
#include <stdio.h>
#include <stdlib.h>
#include "bioplib/SysDefs.h"
#include "bioplib/macros.h"
#include "bioplib/seq.h"

/************************************************************************/
/* Defines and macros
*/
#define MAXBUFF        160
#define TYPE_BOOMER    1
#define TYPE_PHONETICS 2
#define DEFAULT_TYPE   TYPE_PHONETICS

/************************************************************************/
/* Globals
*/

/************************************************************************/
/* Prototypes
*/
int main(int argc, char **argv);
BOOL ParseCmdLine(int argc, char **argv, char *inFile, char *outFile,
                  int *type);
void Usage(void);
BOOL ProcessAllNames(FILE *in, char *nameFile, int type, FILE *out);
BOOL CheckNameForConflicts(char *newName, FILE *namesFp,
                           char *conflictName, int maxConflictName,
                           unsigned int *conflictType, FILE *out);
BOOL CheckBoomer(char *newName, char *oldName, FILE *out);
BOOL CheckPhonetics(char *newName, char *oldName, FILE *out);
REAL RunAligment(char *newName, char *oldName,
                 int gapOpenPenalty, int gapExtensionPenalty);


/************************************************************************/
int main(int argc, char **argv)
{
   FILE *in  = stdin,
        *out = stdout;
   char inFile[MAXBUFF],
        outFile[MAXBUFF];
   int  type = DEFAULT_TYPE;
   
   
   if(ParseCmdLine(argc, argv, inFile, outFile, &type))
   {
      if(blOpenStdFiles(inFile, outFile, &in, &out))
      {
         ProcessAllNames(in, inFile, type, out);
      }
      else
      {
         fprintf(stderr,"Error: Unable to open input or output file\n");
         return(1);
      }
   }
   else
   {
      Usage();
   }

   return(0);
}

/************************************************************************/
BOOL ParseCmdLine(int argc, char **argv, char *infile, char *outfile,
                  int *type)
{
   BOOL setType = FALSE;
   
   argc--; 
   argv++;

   infile[0] = outfile[0] = '\0';


   while(argc)
   {
      if(argv[0][0] == '-')
      {
         switch(argv[0][1])
         {
         case 'b':
            if(setType)
               return(FALSE);
            *type   = TYPE_BOOMER;
            setType = TRUE;
            break;
         case 'p':
            if(setType)
               return(FALSE);
            *type   = TYPE_PHONETICS;
            setType = TRUE;
            break;
         default:
            return(FALSE);
            break;
         }
      }
      else
      {
         /* Check that there are only 1 or 2 arguments left             */
         if(argc > 2)
            return(FALSE);
         
         /* Copy the first to infile                                    */
         strcpy(infile, argv[0]);
         
         /* If there's another, copy it to outfile                      */
         argc--;
         argv++;
         if(argc)
            strcpy(outfile, argv[0]);
            
         return(TRUE);
      }
      argc--;
      argv++;
   }
   
   return(TRUE);
}

/************************************************************************/
void Usage(void)
{
   fprintf(stderr,"checknames V0.1 (c) 2018, Dr Andrew C.R. Martin\n");
}

/************************************************************************/
BOOL ProcessAllNames(FILE *in, char *nameFile, int type, FILE *out)
{
   char name1buffer[MAXBUFF];
   FILE *namesFp;
   unsigned int conflictType;
   
   if((namesFp=fopen(nameFile, "r"))==NULL)
   {
      fprintf(stderr,"Error: Can't open names file for reading (%s)\n",
              nameFile);
      return(FALSE);
   }
   
   while(fgets(buffer, MAXBUFF, in))
   {
      char conflictName[MAXBUFF];
      
      TERMINATE(buffer);
      
      rewind(namesFp);

      switch(type)
      {
      case TYPE_PHONETICS:
         if(!blReadMDM("data/kondrak.mat"))
         {
            fprintf(stderr,"Error: Can't read phonetics matrix\n");
            exit(1);
         }
         CheckPhonetics(newName, buffer, out);
         blFreeMDM();
         break;
      case TYPE_BOOMER:
         if(!blReadMDM("data/boomer.mat"))
         {
            fprintf(stderr,"Error: Can't read boomer matrix\n");
            exit(1);
         }
         CheckBoomer(newName, buffer, out);
         blFreeMDM();
         break;
      default:
         fprint(stderr,"Internal Error: Unrecognized type (%d)\n", type);
         break;
      }
   }

   fclose(namesFp);
   return(TRUE);
}

/************************************************************************/
/* Unused */
BOOL CheckNameForConflicts(char *newName, FILE *namesFp,
                           char *conflictName, int maxConflictName,
                           unsigned int *conflictType, FILE *out)
{
   char buffer[MAXBUFF];
   *conflictType = 0;
   
   while(fgets(buffer, MAXBUFF, namesFp))
   {
      BOOL BoomerOK = TRUE,
         PhoneticsOK = TRUE;
      
      TERMINATE(buffer);

      if(!strcmp(newName, buffer))
         return(TRUE);

#ifdef BOOMER
      if(!blReadMDM("data/boomer.mat"))
      {
         fprintf(stderr,"Error: Can't read boomer matrix\n");
         exit(1);
      }
      BoomerOK = CheckBoomer(newName, buffer, out);
      blFreeMDM();
#endif

#ifdef PHONETICS
      if(!blReadMDM("data/kondrak.mat"))
      {
         fprintf(stderr,"Error: Can't read phonetics matrix\n");
         exit(1);
      }
      PhoneticsOK = CheckPhonetics(newName, buffer, out);
      blFreeMDM();
#endif

      if(!BoomerOK)   *conflictType |= TYPE_BOOMER;
      if(!PhoneticsOK) *conflictType |= TYPE_PHONETICS;
      
      if((!BoomerOK) || (!PhoneticsOK))
      {
         strncpy(conflictName, buffer, maxConflictName);
         return(FALSE);
      }
   }

   return(TRUE);
}

/************************************************************************/
REAL RunAlignment(char *newName, char *oldName,
                  int gapOpenPenalty, int gapExtensionPenalty)
{
   
   int score, newScore, oldScore, maxAlnLen;
   char *newNameAligned = NULL,
        *oldNameAligned = NULL;
   int alignmentLength;
   REAL finalScore = 0.0;
   
   maxAlnLen = strlen(newName) + strlen(oldName) + 1;
   if((newNameAligned = (char *)malloc(maxAlnLen * sizeof(char)))==NULL)
   {
      fprintf(stderr, "Error: no memory for storing alignment\n");
      exit(1);
   }
   if((oldNameAligned = (char *)malloc(maxAlnLen * sizeof(char)))==NULL)
   {
      fprintf(stderr, "Error: no memory for storing alignment\n");
      exit(1);
   }
   
   
   oldScore = blAffinealign(oldName, strlen(oldName),
                            oldName, strlen(oldName),
                            FALSE, /* verbose */
                            FALSE, /* identity */
                            gapOpenPenalty,
                            gapExtensionPenalty,
                            newNameAligned,
                            oldNameAligned,
                            &alignmentLength);
   
   newScore = blAffinealign(newName, strlen(newName),
                            newName, strlen(newName),
                            FALSE, /* verbose */
                            FALSE, /* identity */
                            gapOpenPenalty,
                            gapExtensionPenalty,
                            newNameAligned,
                            oldNameAligned,
                            &alignmentLength);

   score = blAffinealign(newName, strlen(newName),
                         oldName, strlen(oldName),
                         FALSE, /* verbose */
                         FALSE, /* identity */
                         gapOpenPenalty,
                         gapExtensionPenalty,
                         newNameAligned,
                         oldNameAligned,
                         &alignmentLength);

   finalScore = 100.0 * score / MIN(oldScore, newScore);
   
#ifdef ALIGNMENTS
   newNameAligned[alignmentLength] = '\0';
   oldNameAligned[alignmentLength] = '\0';
   
   fprintf(stderr, "\n%s\n",   newNameAligned);
   fprintf(stderr, "%s\n", oldNameAligned);
#endif

   free(newNameAligned);
   free(oldNameAligned);

   return(finalScore);
}

/************************************************************************/
BOOL CheckBoomer(char *newName, char *oldName, FILE *out)
{
   REAL score;
   int  gapOpenPenalty      = 1,
        gapExtensionPenalty = 1;
   
   score = RunAlignment(newName, oldName, gapOpenPenalty,
                        gapExtensionPenalty);

   fprintf(out, "Boomer: %s %s %.2f\n", newName, oldName, score);

   if(score > 60.0)
      return(FALSE);

   return(TRUE);
}

/************************************************************************/
BOOL CheckPhonetics(char *newName, char *oldName, FILE *out)
{
   REAL score;
   int  gapOpenPenalty      = 10,
        gapExtensionPenalty = 10;
   
   score = RunAlignment(newName, oldName, gapOpenPenalty,
                        gapExtensionPenalty);

   fprintf(out, "Phonetics: %s %s %.2f\n", newName, oldName, score);
   
   if(score > 93.0)
      return(FALSE);

   return(TRUE);
}
