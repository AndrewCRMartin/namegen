/************************************************************************/
/**

   Program:    abcheckname
   \file       abcheckname.c
   
   \version    V1.5.1
   \date       08.12.20
   \brief      Checks an antibody name for possible conflicts
   
   \copyright  (c) UCL / Prof. Andrew C. R. Martin 2018-20
   \author     Prof. Andrew C. R. Martin
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
   V1.0   18.05.18 Initial release
   V1.1   04.06.18 Removed output file parameter and added ability to
                   compare two names
   V1.2   23.10.18 Fixed bug when specifying names file with -n
   V1.3   03.04.19 Allows blank lines in names file
   V1.4   23.10.19 Allows comments in names file
   V1.5   23.10.19 Added -e but only works against database of names with
                   -n
   V1.5.1 08.12.20 Comments now work for analysis of the data file as 
                   well as individual names

*************************************************************************/
/* Includes
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bioplib/SysDefs.h"
#include "bioplib/macros.h"
#include "bioplib/seq.h"

/************************************************************************/
/* Defines and macros
*/
#define MAXBUFF                       160
#define MAXNAMELENGTH                 64
#define TYPE_BOUMA                    1
#define TYPE_PHONETICS                2
#define TYPE_LETTERSHAPE              3
#define TYPE_STEMMATCH                4
#define DEFAULT_TYPE                  TYPE_PHONETICS
#define DEFAULT_PHONETICS_MATRIX      "abnamedata/kondrak.mat"
#define DEFAULT_BOUMA_MATRIX          "abnamedata/bouma.mat"
#define DEFAULT_LETTER_MATRIX         "abnamedata/letters.mat"
#define GAPPEN_DUMMY                  -10000
#define DEFAULT_NAMESFILE             "abnamedata/abnames.dat"
#define DEFAULT_PHONETICS_THRESHOLD   85.5
#define DEFAULT_BOUMA_THRESHOLD       95.0
#define DEFAULT_LETTERSHAPE_THRESHOLD 80.7
#define DATAENV                       "DATADIR"

/************************************************************************/
/* Globals
*/
int gUserGappen    = GAPPEN_DUMMY;
int gUserGapExtpen = GAPPEN_DUMMY;
int gNMatches[MAXNAMELENGTH];


/************************************************************************/
/* Prototypes
*/
int main(int argc, char **argv);
BOOL ParseCmdLine(int argc, char **argv, char *inParam, char *abName2,
                  int *type, BOOL *doAll, BOOL *verbose, char *namesFile,
                  REAL *printThreshold);
void Usage(void);
BOOL ProcessAllNames(char *namesFile, int type, BOOL verbose,
                     char *scoreMatrix, FILE *out);
BOOL ProcessOneName(char *name, char *namesFile, int type, BOOL verbose,
                    char *scoreMatrix, REAL printThreshold, FILE *out);
BOOL CheckNameForConflicts(char *newName, FILE *namesFp,
                           char *conflictName, int maxConflictName,
                           unsigned int *conflictType, FILE *out);
BOOL CheckBouma(char *newName, char *oldName, BOOL verbose,
                 REAL printThreshold, FILE *out);
BOOL CheckPhonetics(char *newName, char *oldName, BOOL verbose,
                    REAL printThreshold, FILE *out);
BOOL CheckLetterShape(char *newName, char *oldName, BOOL verbose,
                      REAL printThreshold, FILE *out);
REAL RunAlignment(char *newName, char *oldName,
                  int gapOpenPenalty, int gapExtensionPenalty,
                  BOOL verbose, FILE *out);
BOOL OpenStdFile(char *file, FILE **fp, char *mode);
char *FindFile(char *filename, char *envvar, BOOL *noenv);
char *FindFileAndCheck(char *filename, char *message);
BOOL CompareTwoNames(char *abName1, char *abName2, int type, BOOL verbose,
                     char *scoreMatrix, FILE *out);
void PrintStemMatches(char *name, FILE *out);
void CheckStemMatch(char *name, char *oldName, BOOL verbose);
int ScoreStemMatch(char *name, char *oldName);


/************************************************************************/
/*>int main(int argc, char **argv)
   -------------------------------
*//*
   Main program

-  18.05.18 Original   By: ACRM
-  23.10.18 Fixed bug when specifying names file with -n
*/
int main(int argc, char **argv)
{
   char inParam[MAXBUFF],
        abName2[MAXBUFF];
   int  type    = DEFAULT_TYPE,
        retval  = 0,
        i;
   BOOL doAll   = FALSE,
        verbose = FALSE;
   char *namesFile = NULL,
        *scoreMatrix = NULL,
        defNamesFile[MAXBUFF];
   REAL printThreshold = (REAL)(-1000.0); /* If negative, default will 
                                             be used 
                                          */

   strncpy(defNamesFile, DEFAULT_NAMESFILE, MAXBUFF);

   if(ParseCmdLine(argc, argv, inParam, abName2, &type, &doAll, &verbose,
                   defNamesFile, &printThreshold))
   {
      if(!inParam[0])
      {
         Usage();
      }

      if((namesFile=FindFileAndCheck(defNamesFile, "names file"))
         ==NULL)
      return(1);

      switch(type)
      {
      case TYPE_PHONETICS:
         if((scoreMatrix=FindFileAndCheck(DEFAULT_PHONETICS_MATRIX,
                                          "phonetics matrix file"))
            ==NULL)
            return(1);
         break;
      case TYPE_BOUMA:
         if((scoreMatrix=FindFileAndCheck(DEFAULT_BOUMA_MATRIX,
                                          "Bouma matrix file"))
            ==NULL)
            return(1);
         break;
      case TYPE_LETTERSHAPE:
         if((scoreMatrix=FindFileAndCheck(DEFAULT_LETTER_MATRIX,
                                          "Simpson letter matrix file"))
            ==NULL)
            return(1);
         break;
      case TYPE_STEMMATCH:
         for(i=0; i<MAXNAMELENGTH; i++)
         {
            gNMatches[i] = 0;
         }
         break;
      default:
         fprintf(stderr,"Error: Internal error - unknown type (%d)\n",
                 type);
      }

      if(doAll)
      {
         ProcessAllNames(inParam, type, verbose, scoreMatrix, stdout);
      }
      else
      {
         if(abName2[0])
         {
            /* Second antibody name specified                          */
            CompareTwoNames(inParam, abName2, type, verbose, scoreMatrix,
                            stdout);
         }
         else
         {
            ProcessOneName(inParam, namesFile, type, verbose, scoreMatrix,
                           printThreshold, stdout);
         }
      }
      
   }
   else
   {
      Usage();
   }

   FREE(namesFile);
   return(retval);
}


/************************************************************************/
/*>BOOL ParseCmdLine(int argc, char **argv, char *inParam, char *abName2,
                     int *type, BOOL *doAll, BOOL *verbose, 
                     char *namesFile, REAL *printThreshold)
   ----------------------------------------------------------------------
*//*
   \param[in]  argc           Argument count
   \param[in]  argv           Arguments
   \param[out] inParam        Compulsory argument passed on command line
                              antibody name or file of names if -a 
                              specified
   \param[out] abName2        Optional second antibody name
   \param[out] type           Type of comparison (Phonetics, Bouma or 
                              Letters)
   \param[out] doAll          Analyze the distribution in the existing 
                              words file
   \param[out] verbose        Show the alignments
   \param[out] namesFile      User-specified names file
   \param[out] printThreshold Threshold for printing matches

   Parse the command line

-  18.05.18 Original   By: ACRM
-  23.10.19 Added -e TYPE_STEMMATCH
*/
BOOL ParseCmdLine(int argc, char **argv, char *inParam, char *abName2,
                  int *type, BOOL *doAll, BOOL *verbose, char *namesFile,
                  REAL *printThreshold)
{
   BOOL setType = FALSE;
   
   inParam[0] = abName2[0] = '\0';

   argc--; argv++;

   while(argc)
   {
      if(argv[0][0] == '-')
      {
         switch(argv[0][1])
         {
         case 'b':
            if(setType)
               return(FALSE);
            *type   = TYPE_BOUMA;
            setType = TRUE;
            break;
         case 'p':
            if(setType)
               return(FALSE);
            *type   = TYPE_PHONETICS;
            setType = TRUE;
            break;
         case 's':
            if(setType)
               return(FALSE);
            *type   = TYPE_LETTERSHAPE;
            setType = TRUE;
            break;
         case 'e':
            if(setType)
               return(FALSE);
            *type   = TYPE_STEMMATCH;
            setType = TRUE;
            break;
         case 'a':
            *doAll = TRUE;
            break;
         case 'v':
            *verbose = TRUE;
            break;
         case 'g':
            argc--; argv++;
            if(!argc) return(FALSE);
            if(!sscanf(argv[0], "%d", &gUserGappen)) return(FALSE);
            break;
         case 'x':
            argc--; argv++;
            if(!argc) return(FALSE);
            if(!sscanf(argv[0], "%d", &gUserGapExtpen)) return(FALSE);
            break;
         case 'n':
            argc--; argv++;
            if(!argc) return(FALSE);
            strncpy(namesFile, argv[0], MAXBUFF);
            break;
         case 't':
            argc--; argv++;
            if(!argc) return(FALSE);
            if(!sscanf(argv[0], "%lf", printThreshold)) return(FALSE);
            break;
         default:
            return(FALSE);
            break;
         }
      }
      else
      {
         /* Check there are 1 or 2 arguments left                       */
         if((argc < 1) || (argc > 2))
         {
            return(FALSE);
         }
         

         /* Copy the first (compulsory) argument                        */
         strncpy(inParam, argv[0], MAXBUFF);
         
         /* See if there is another argument left                       */
         argc--; argv++;
         if(argc)
         {
            strcpy(abName2, argv[0]);
            argc--; argv++;
         }
         
         return(TRUE);
      }
      argc--;
      argv++;
   }
   
   return(FALSE);
}


/************************************************************************/
/*>void Usage(void)
   ----------------
*//*
   Print usage message

-  18.05.18 Original   By: ACRM
-  04.06.18 V1.1
-  23.10.18 V1.2
-  03.04.19 V1.3
-  23.10.19 V1.4, V1.5
-  08.12.20 V1.5.1
*/
void Usage(void)
{
   fprintf(stderr,"\nabcheckname V1.5.1 (c) 2018-20, Prof Andrew C.R. \
Martin\n");
   fprintf(stderr,"\nUsage:\n");
   fprintf(stderr,"    Compare name against library...\n");
   fprintf(stderr,"       abcheckname [-b|-p|-s|-e][-v][-g n][-x n]\
[-t n][-n file.names] name [name2]]\n");
   fprintf(stderr,"    -or- Compare two names...\n");
   fprintf(stderr,"       abcheckname [-b|-p|-s|-e][-v][-g n][-x n] \
name1 name2\n");
   fprintf(stderr,"    -or- Analyze library...\n");
   fprintf(stderr,"       abcheckname -a [-b|-p|-s|-e][-v][-g n]\
[-x n] file.names\n");

   fprintf(stderr,"\n                  -a Check all used names against \
each other\n");
   fprintf(stderr,"                  -b Check the Bouma\n");
   fprintf(stderr,"                  -p Check the Kondrak phonetics \
[Default]\n");
   fprintf(stderr,"                  -s Check the Simpson letter \
similarity\n");
   fprintf(stderr,"                  -e Check the end similarity\n");
   fprintf(stderr,"                     Currently only wih -n\n");
   fprintf(stderr,"                  -v Verbose - show alignments\n");
   fprintf(stderr,"                  -g Specify gap opening penalty\n");
   fprintf(stderr,"                  -x Specify gap extension penalty\n");
   fprintf(stderr,"                  -n Specify the file containing \
antibody names\n");
   fprintf(stderr,"                  -t Specify a threshold for printing \
as similar\n");

   fprintf(stderr,"\nAbcheckname is a program for comparing potential \
antibody names against\n");
   fprintf(stderr,"those already used by the WHO-INN.\n");

   fprintf(stderr,"\nThe -a option is used to compare existing names to \
look at the \n");
   fprintf(stderr,"distribution of similarity scores.\n\n");

   fprintf(stderr,"Default thesholds for printing names as too similar \
are:\n");
   fprintf(stderr,"   Phonetics:        %.1f\n",
           DEFAULT_PHONETICS_THRESHOLD);
   fprintf(stderr,"   LetterSimilarity: %.1f\n",
           DEFAULT_LETTERSHAPE_THRESHOLD);
   fprintf(stderr,"   Bouma:            %.1f\n\n",
           DEFAULT_BOUMA_THRESHOLD);
}


/************************************************************************/
/*>BOOL ProcessAllNames(char *namesFile, int type, BOOL verbose,
                     char *scoreMatrix, FILE *out)
   -------------------------------------------------------------
*//*

   \param[in]  namesFile   Filename of file containing existing names
   \param[in]  type        Type of comparison to perform
   \param[in]  verbose     Print the alignments
   \param[in]  scoreMatrix Filename of the score matrix
   \param[in]  out         Pointer to output file

   Process the file of names comparing them all against each other to
   obtain a score distribution.

-  18.05.18 Original   By: ACRM
-  08.12.20 Terminates at # in data file
*/
BOOL ProcessAllNames(char *namesFile, int type, BOOL verbose,
                     char *scoreMatrix, FILE *out)
{
   char name1[MAXBUFF],
        name2[MAXBUFF];
   FILE *fp1 = NULL,
        *fp2 = NULL;
   
   if((fp1=fopen(namesFile, "r"))==NULL)
   {
      fprintf(stderr,"Error: Can't open names file for first reading \
(%s)\n",
              namesFile);
      return(FALSE);
   }
   
   if((fp2=fopen(namesFile, "r"))==NULL)
   {
      fprintf(stderr,"Error: Can't open names file for second reading \
(%s)\n",
              namesFile);
      return(FALSE);
   }

   if(!blReadMDM(scoreMatrix))
   {
      fprintf(stderr,"Error: Can't read scoring matrix (%s)\n",
              scoreMatrix);
      exit(1);
   }
   
   while(fgets(name1, MAXBUFF, fp1))
   {
      TERMINATE(name1);
      TERMAT(name1, '#');
      KILLTRAILSPACES(name1);
      if(strlen(name1))
      {
         rewind(fp2);
         while(fgets(name2, MAXBUFF, fp2))
         {
            TERMINATE(name2);
            TERMAT(name2, '#');
            KILLTRAILSPACES(name2);
            if(strlen(name2))
            {
               if(strcmp(name1, name2))
               {
                  switch(type)
                  {
                  case TYPE_PHONETICS:
                     CheckPhonetics(name1, name2, verbose,
                                    (REAL)0.0, out);
                     break;
                  case TYPE_BOUMA:
                     CheckBouma(name1, name2, verbose,
                                (REAL)0.0, out);
                     break;
                  case TYPE_LETTERSHAPE:
                     CheckLetterShape(name1, name2, verbose,
                                      (REAL)0.0, out);
                     break;
                  default:
                     fprintf(stderr,"Internal Error: Unrecognized \
type (%d)\n", type);
                     break;
                  }
               }
            }
         }
      }
   }
   
   blFreeMDM();
   fclose(fp1);
   fclose(fp2);
   return(TRUE);
}


/************************************************************************/
/*>REAL RunAlignment(char *newName, char *oldName,
                     int gapOpenPenalty, int gapExtensionPenalty, 
                     BOOL verbose, FILE *out)
   -----------------------------------------------
*//*
   \param[in]  newName             Name to be checked
   \param[in]  oldName             Name against which to compare
   \param[in]  gapOpenPenalty      Gap opening ppenalty
   \param[in]  gapExtensionPenalty Gap extension penalty
   \param[in]  verbose             Show alignments
   \param[in]  out                 Output file pointer

   \return                         Percentage score (0.0 ... 100.0)

   Runs an alignment and calculate the alignment score. 
   Prints the alignment if required.

-  18.05.18 Original   By: ACRM
*/
REAL RunAlignment(char *newName, char *oldName,
                  int gapOpenPenalty, int gapExtensionPenalty, 
                  BOOL verbose, FILE *out)
{
   
   int  score,
        newScore,
        oldScore,
        maxAlnLen;
   char *newNameAligned = NULL,
        *oldNameAligned = NULL;
   int  alignmentLength;
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

   finalScore = 100.0 * score / MAX(oldScore, newScore);
   
   if(verbose)
   {
      newNameAligned[alignmentLength] = '\0';
      oldNameAligned[alignmentLength] = '\0';
   
      fprintf(out, "\n%s\n", newNameAligned);
      fprintf(out, "%s\n",   oldNameAligned);
   }

   free(newNameAligned);
   free(oldNameAligned);

   return(finalScore);
}


/************************************************************************/
/*>BOOL CheckBouma(char *newName, char *oldName, BOOL verbose, 
                    REAL printThreshold, FILE *out)
   ------------------------------------------------------------
*//*
   \param[in]  newName         New name to check
   \param[in]  oldName         Old name against which to check
   \param[in]  verbose         Print the alignment
   \param[in]  printThreshold  Threshold at which to print a match
   \param[in]  out             Output file pointer

   Run the alignment and print results for Bouma comparison

-  18.05.18 Original   By: ACRM
*/
BOOL CheckBouma(char *newName, char *oldName, BOOL verbose,
                 REAL printThreshold, FILE *out)
{
   REAL score;
   int  gapOpenPenalty      = 5,
        gapExtensionPenalty = 5;

   if(gUserGappen != GAPPEN_DUMMY)
      gapOpenPenalty = gUserGappen;
   if(gUserGapExtpen != GAPPEN_DUMMY)
      gapExtensionPenalty = gUserGapExtpen;
   
   score = RunAlignment(newName, oldName, gapOpenPenalty,
                        gapExtensionPenalty, verbose, out);

   if(score > printThreshold)
      fprintf(out, "Bouma: %s %s %.2f\n", newName, oldName, score);
   
   if(score > printThreshold)
      return(FALSE);

   return(TRUE);
}


/************************************************************************/
/*>BOOL CheckPhonetics(char *newName, char *oldName, BOOL verbose, 
                       REAL printThreshold, FILE *out)
   ---------------------------------------------------------------
*//*
   \param[in]  newName         New name to check
   \param[in]  oldName         Old name against which to check
   \param[in]  verbose         Print the alignment
   \param[in]  printThreshold  Threshold at which to print a match
   \param[in]  out             Output file pointer

   Run the alignment and print results for Phonetics comparison

-  18.05.18 Original   By: ACRM
*/
BOOL CheckPhonetics(char *newName, char *oldName, BOOL verbose,
                    REAL printThreshold, FILE *out)
{
   REAL score;
   int  gapOpenPenalty      = 10,
        gapExtensionPenalty = 10;

   if(gUserGappen != GAPPEN_DUMMY)
      gapOpenPenalty = gUserGappen;
   if(gUserGapExtpen != GAPPEN_DUMMY)
      gapExtensionPenalty = gUserGapExtpen;
   
   score = RunAlignment(newName, oldName, gapOpenPenalty,
                        gapExtensionPenalty, verbose, out);

   if(score > printThreshold)
      fprintf(out, "Phonetics: %s %s %.2f\n", newName, oldName, score);
   
   if(score > printThreshold)
      return(FALSE);

   return(TRUE);
}


/************************************************************************/
/*>BOOL CheckLetterShape(char *newName, char *oldName, BOOL verbose, 
                         REAL printThreshold, FILE *out)
   -----------------------------------------------------------------
*//*
   \param[in]  newName         New name to check
   \param[in]  oldName         Old name against which to check
   \param[in]  verbose         Print the alignment
   \param[in]  printThreshold  Threshold at which to print a match
   \param[in]  out             Output file pointer

   Run the alignment and print results for letter shape comparison

-  18.05.18 Original   By: ACRM
*/
BOOL CheckLetterShape(char *newName, char *oldName, BOOL verbose,
                      REAL printThreshold, FILE *out)
{
   REAL score;
   int  gapOpenPenalty      = 200,
        gapExtensionPenalty = 200;

   if(gUserGappen != GAPPEN_DUMMY)
      gapOpenPenalty = gUserGappen;
   if(gUserGapExtpen != GAPPEN_DUMMY)
      gapExtensionPenalty = gUserGapExtpen;
   
   score = RunAlignment(newName, oldName, gapOpenPenalty,
                        gapExtensionPenalty, verbose, out);

   if(score > printThreshold)
      fprintf(out, "LetterSimilarity: %s %s %.2f\n",
              newName, oldName, score);
   
   if(score > printThreshold)
      return(FALSE);

   return(TRUE);
}


/************************************************************************/
/*>BOOL OpenStdFile(char *file, FILE **fp, char *mode)
   ---------------------------------------------------
*//**

   \param[in]     *file       Filename
   \param[out]    **fp        Pointer to file pointer
   \param[in]     *mode       File read/write mode
   \return                    Success?

   Open the file if specified. fp is not modified if file is not
   specified.

-  18.05.18 Original    By: ACRM
*/
BOOL OpenStdFile(char *file, FILE **fp, char *mode)
{
   if(file!=NULL && file[0] && strcmp(file,"-"))
   {
      if((*fp = fopen(file,mode))==NULL)
      {
         fprintf(stderr,"Unable to open file: %s\n", file);
         return(FALSE);
      }
   }
      
   return(TRUE);
}


/************************************************************************/
/*>BOOL ProcessOneName(char *name, char *namesFile, int type, 
                       BOOL verbose, char *scoreMatrix, 
                       REAL printThreshold, FILE *out)
   ----------------------------------------------------------
*//*
   \param[in]  name            A name to be tested
   \param[in]  namesFile       Filename of the file containing existing
                               names
   \param[in]  type            Type of comparison
   \param[in]  verbose         Print alignments
   \param[in]  scoreMatrix     Filename of matrix file
   \param[in]  printThreshold  Threshold to print a match
   \param[in]  out             Output file handle
   \return                     Success

   Compares a single name against the existing names

-  18.05.18 Original   By: ACRM
-  03.05.19 Added check for blank name
-  23.10.19 Allow commented lines and comments after names
-           Added TYPE_STEMMATCH
-  08.12.20 Modified to use TERMAT()
*/
BOOL ProcessOneName(char *name, char *namesFile, int type, BOOL verbose,
                    char *scoreMatrix, REAL printThreshold, FILE *out)
{
   FILE *fp = NULL;
   char oldName[MAXBUFF];
   
   if((fp=fopen(namesFile, "r"))==NULL)
   {
      fprintf(stderr,"Error: Can't open names file (%s)\n",
              namesFile);
      return(FALSE);
   }

   if(type != TYPE_STEMMATCH)
   {
      if(!blReadMDM(scoreMatrix))
      {
         fprintf(stderr,"Error: Can't read scoring matrix (%s)\n",
                 scoreMatrix);
         return(FALSE);
      }
   }
   

   if(printThreshold < 0.0)
   {
      switch(type)
      {
      case TYPE_PHONETICS:
         printThreshold = DEFAULT_PHONETICS_THRESHOLD;
         break;
      case TYPE_BOUMA:
         printThreshold = DEFAULT_BOUMA_THRESHOLD;
         break;
      case TYPE_LETTERSHAPE:
         printThreshold = DEFAULT_LETTERSHAPE_THRESHOLD;
         break;
      case TYPE_STEMMATCH:
         break;
      default:
         fprintf(stderr,"Internal Error: Unrecognized type (%d)\n", type);
         break;
      }
   }
   
   while(fgets(oldName, MAXBUFF, fp))
   {
      TERMINATE(oldName);
      TERMAT(oldName, '#');
      KILLTRAILSPACES(oldName);

      if(strlen(oldName))
      {
         switch(type)
         {
         case TYPE_PHONETICS:
            CheckPhonetics(name, oldName, verbose, printThreshold, out);
            break;
         case TYPE_BOUMA:
            CheckBouma(name, oldName, verbose, printThreshold, out);
            break;
         case TYPE_LETTERSHAPE:
            CheckLetterShape(name, oldName, verbose, printThreshold, out);
            break;
         case TYPE_STEMMATCH:
            CheckStemMatch(name, oldName, verbose);
            break;
         default:
            fprintf(stderr,"Internal Error: Unrecognized type (%d)\n",
                    type);
            break;
         }
      }
   }
   
   blFreeMDM();
   fclose(fp);

   if(type == TYPE_STEMMATCH)
      PrintStemMatches(name, out);

   return(TRUE);
}


/************************************************************************/
void PrintStemMatches(char *name, FILE *out)
{
   int i,
       maxMatches = 0,
       bestMatchLength;
   char stem[MAXNAMELENGTH];

   for(i=4; i<=strlen(name); i++)
   {
      if(gNMatches[i] > 0)
      {
         strncpy(stem, name + (strlen(name)-i), i);
         stem[i] = '\0';

         if(i==strlen(name))
         {
            fprintf(out, "Stem match was %2d characters in %3d names \
(%s) whole name!\n",
                    i, gNMatches[i], stem);
         }
         else
         {
            fprintf(out, "Stem match was %2d characters in %3d names \
(-%s)\n",
                    i, gNMatches[i], stem);
         }
         
      }
   }
}

      
/************************************************************************/
void CheckStemMatch(char *name, char *oldName, BOOL verbose)
{
   int nMatch;

   if((nMatch = ScoreStemMatch(name, oldName)) >= MAXNAMELENGTH)
   {
      fprintf(stderr, "Name is too long (%d characters). Increase \
MAXNAMELENGTH\n", nMatch);
      exit(1);
   }
   
   gNMatches[nMatch]++;
}


/************************************************************************/
int ScoreStemMatch(char *name, char *oldName)
{
   int nameLast,
       oldNameLast,
       nMatch  = 0;

   nameLast    = strlen(name)    - 1;
   oldNameLast = strlen(oldName) - 1;

   while(((nameLast    - nMatch) >= 0) &&
         ((oldNameLast - nMatch) >= 0) &&
         (name[nameLast-nMatch] == oldName[oldNameLast-nMatch]))
   {
      nMatch++;
   }

   return(nMatch);
}

      
/************************************************************************/
/*>BOOL CompareTwoNames(char *abName1, char *abName2, int type, 
                        BOOL verbose, char *scoreMatrix, FILE *out)
   -------------------------------------------------------------------
*//**
   \param[in]  abName1         A name to be tested
   \param[in]  abName2         A name against which to test
   \param[in]  type            Type of comparison
   \param[in]  verbose         Print alignments
   \param[in]  scoreMatrix     Filename of matrix file
   \param[in]  out             Output file handle
   \return                     Success

   Compares two single names against each other

-  04.06.18 Original   By: ACRM
*/
BOOL CompareTwoNames(char *abName1, char *abName2, int type, BOOL verbose,
                     char *scoreMatrix, FILE *out)
{
   REAL printThreshold = (REAL)0.0;
   
   if(!blReadMDM(scoreMatrix))
   {
      fprintf(stderr,"Error: Can't read scoring matrix (%s)\n",
              scoreMatrix);
      return(FALSE);
   }

   switch(type)
   {
   case TYPE_PHONETICS:
      CheckPhonetics(abName1, abName2, verbose, printThreshold, out);
      break;
   case TYPE_BOUMA:
      CheckBouma(abName1, abName2, verbose, printThreshold, out);
      break;
   case TYPE_LETTERSHAPE:
      CheckLetterShape(abName1, abName2, verbose, printThreshold, out);
      break;
   default:
      fprintf(stderr,"Internal Error: Unrecognized type (%d)\n", type);
      break;
   }

   blFreeMDM();
   return(TRUE);
}


/************************************************************************/
/*>char *FindFile(char *filename, char *envvar, BOOL *noenv)
   ---------------------------------------------------------
*//*
   \param[in]  filename    Basic (or complete) filename
   \param[in]  envvar      Environment variable to search
   \param[out] noenv       Indicates if the environment variable was
                           not set (may be NULL)
   \return                 Malloc'd filename or NULL if not found

   This is equivalent to blOpenFile() but just returns the file name
   rather than opening the file.

   Attempts to find a filename as specified. Returns a malloc'd copy
   of the filename if found. If this fails:

   Under UNIX/MS-DOS/Mac:
   gets the contents of the envvar environment variable and prepends
   that to the filename and tries again. If envvar was not set, noenv
   is set to TRUE and the routine returns a NULL pointer.

   Under other OSs:
   prepends the envvar string onto the filename and sees if that exists.
   If it does, the malloc'd complete path/filename is returned. 
   If not returns a NULL pointer

   The return is a malloc'd string that must be freed when finished with.

-  22.05.18  Original   By: ACRM


*/
char *FindFile(char *filename, char *envvar, BOOL *noenv)
{
   char *buffer  = NULL;
   int  nameSize = 0;
#if (unix || __unix__ || MS_WINDOWS || __unix || __MACH__ || __APPLE__)
   char *datadir = NULL;
#endif

   if(noenv != NULL) *noenv = FALSE;
   if(filename == NULL || filename[0] == '\0')
      return(NULL);

   /* Calculate space needed for filename plus environment variable     */
   nameSize = strlen(filename) + 2;
#if (unix || __unix__ || MS_WINDOWS || __unix || __MACH__ || __APPLE__)
   if((datadir = getenv(envvar)) != NULL)
   {
      nameSize += strlen(datadir);
   }
#else
   nameSize += strlen(envvar);
#endif
   
   /* Allocate space for new filename                                   */
   if((buffer=(char *)malloc(nameSize * sizeof(char)))==NULL)
      return(NULL);

   /* If the file exists as given, simply copy it and return            */
   if(access(filename, R_OK) == 0)
   {
      strncpy(buffer, filename, nameSize);
   }
   else
   {
      /* Failed, so build alternative directory/filename                */
#if (unix || __unix__ || MS_WINDOWS || __unix || __MACH__ || __APPLE__)
      if(datadir != NULL)
      {
         if(datadir[strlen(datadir)-1] == '/')
            sprintf(buffer,"%s%s",datadir,filename);
         else
            sprintf(buffer,"%s/%s",datadir,filename);
      }
      else
      {
         if(noenv != NULL) *noenv = TRUE;
         FREE(buffer);
         return(NULL);
      }
#else
      sprintf(buffer,"%s:%s",envvar,filename);
#endif

      if(access(buffer, R_OK) != 0)
      {
         FREE(buffer);
         return(NULL);
      }
   }

   return(buffer);
}


/************************************************************************/
/*>char *FindFileAndCheck(char *filename, char *message)
   -----------------------------------------------------
*//*
   \param[in]  filename    Basic filename
   \param[in]  message     Text to be printed on error to describe
                           the file

   Wrapper to FindFile() that prints an error message if FindFile()
   failed.

-  22.05.18 Original   By: ACRM
*/
char *FindFileAndCheck(char *filename, char *message)
{
   char *newFilename = NULL;
   BOOL noEnv = FALSE;
   
   if((newFilename = FindFile(filename, DATAENV, &noEnv))==NULL)
   {
      fprintf(stderr, "Error: %s (%s) not found.\n", message,
              filename);
      if(noEnv)
      {
         fprintf(stderr, "       Environment variable (%s) not set.\n",
                 DATAENV);
      }
   }
   return(newFilename);
}
