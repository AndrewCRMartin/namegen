#define DEBUG 2
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
#define MAXBUFF 160
#define TYPE_BOOMER 1
#define TYPE_SPELLING 2


/************************************************************************/
/* Globals
*/

/************************************************************************/
/* Prototypes
*/
int main(int argc, char **argv);
BOOL ParseCmdLine(int argc, char **argv, char *inFile, char *outFile);
void Usage(void);
BOOL ProcessAllNames(FILE *in, char *nameFile, FILE *out);
BOOL CheckNameForConflicts(char *newName, FILE *namesFp,
                           char *conflictName, int maxConflictName,
                           unsigned int *conflictType);
BOOL CheckBoomer(char *newName, char *oldName);
BOOL CheckSpelling(char *newName, char *oldName);
REAL RunAligment(char *newName, char *oldName,
                 int gapOpenPenalty, int gapExtensionPenalty);


/************************************************************************/
int main(int argc, char **argv)
{
   FILE *in = stdin,
      *out = stdout;
   char inFile[MAXBUFF],
      outFile[MAXBUFF];
   
   
   if(ParseCmdLine(argc, argv, inFile, outFile))
   {
      if(blOpenStdFiles(inFile, outFile, &in, &out))
      {
         ProcessAllNames(in, inFile, out);
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

BOOL ParseCmdLine(int argc, char **argv, char *inFile, char *outFile)
{
   inFile[0] = outFile[0] = '\0';
   argc--; argv++;

   /* Require at least one filename */
   if(argc < 1)
      return(FALSE);

   strncpy(inFile, argv[0], MAXBUFF);

   argc--; argv++;

   if(argc)
   {
      argc--; argv++;
      strncpy(outFile, argv[0], MAXBUFF);
   }
         
   return(TRUE);
}

void Usage(void)
{
   fprintf(stderr,"checknames V0.1 (c) 2018, Dr Andrew C.R. Martin\n");
}

BOOL ProcessAllNames(FILE *in, char *nameFile, FILE *out)
{
   char buffer[MAXBUFF];
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
      if(!CheckNameForConflicts(buffer, namesFp, conflictName, MAXBUFF,
                                &conflictType))
      {
         fprintf(out, "Conflict of new name '%s' with '%s':",
                 buffer, conflictName);
         if((conflictType & TYPE_BOOMER))
         {
            fprintf(out, " boomer");
         }
         if((conflictType & TYPE_SPELLING))
         {
            fprintf(out, " spelling");
         }
         fprintf(out, "\n");
      }
      
   }

   fclose(namesFp);
   return(TRUE);
}

BOOL CheckNameForConflicts(char *newName, FILE *namesFp,
                           char *conflictName, int maxConflictName,
                           unsigned int *conflictType)
{
   char buffer[MAXBUFF];
   *conflictType = 0;
   
   while(fgets(buffer, MAXBUFF, namesFp))
   {
      BOOL BoomerOK = TRUE,
         SpellingOK = TRUE;
      
      TERMINATE(buffer);

      if(!strcmp(newName, buffer))
         return(TRUE);
      
      if(!blReadMDM("data/boomer.mat"))
      {
         fprintf(stderr,"Can't read boomer matrix\n");
         exit(1);
      }
      BoomerOK = CheckBoomer(newName, buffer);
      blFreeMDM();

#ifdef NEW
      if(!blReadMDM("data/spelling.mat"))
      {
         fprintf(stderr,"Can't read spelling matrix\n");
         exit(1);
      }
      SpellingOK   = CheckSpelling(newName, buffer);
      blFreeMDM();
#endif
      if(!BoomerOK)   *conflictType |= TYPE_BOOMER;
      if(!SpellingOK) *conflictType |= TYPE_SPELLING;
      
      if((!BoomerOK) || (!SpellingOK))
      {
         strncpy(conflictName, buffer, maxConflictName);
         return(FALSE);
      }
   }

   return(TRUE);
}

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
   
#ifdef DEBUG
   fprintf(stderr, "%s %s %.2f\n", newName, oldName, finalScore);
#if DEBUG > 1
   newNameAligned[alignmentLength] = '\0';
   oldNameAligned[alignmentLength] = '\0';
   
   fprintf(stderr, "%s\n",   newNameAligned);
   fprintf(stderr, "%s\n\n", oldNameAligned);
#endif
#endif

   free(newNameAligned);
   free(oldNameAligned);

   return(finalScore);
}

BOOL CheckBoomer(char *newName, char *oldName)
{
   REAL score;
   int  gapOpenPenalty      = 2,
        gapExtensionPenalty = 1;
   
   score = RunAlignment(newName, oldName, gapOpenPenalty,
                        gapExtensionPenalty);
   if(score > 50.0)
      return(FALSE);

   return(TRUE);
}

BOOL CheckSpelling(char *newName, char *oldName)
{
   return(TRUE);
}
