/************************************************************************/
/**

   Program:    MakeKondrakMatrix
   \file       MakeKondrakMatrix.c
   
   \version    V1.0
   \date       15.05.18   
   \brief      Create a letter-sound similarity matrix
   
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

   Define #TEST to produce the sample data from the thesis
   Define #AS_THESIS to use parameters in the thesis that do not match
      what is in the code or the results shown in the matrix sample
      in the thesis shown by defining #TEST

**************************************************************************

   Revision History:
   =================

*************************************************************************/
/* Includes
*/
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "bioplib/macros.h"
#include "bioplib/SysDefs.h"
#include "bioplib/MathType.h"

/************************************************************************/
/* Defines and macros
*/
#include "KondrakConstants.h"
#ifdef TEST
#define MAXLETTERS 7
#else
#define MAXLETTERS 26
#endif
#define LETTER(x) (int)(islower(x)?(char)toupper(x):x)-65
#ifdef FOO
#define ISVOWEL(a) ((a == 'a') || (a == 'e') || (a == 'i') || (a == 'o') || (a == 'u') || (a == 'y'))
#endif
#define ISVOWEL(a) ((a == 'a') || (a == 'e') || (a == 'i') || (a == 'o') || (a == 'u'))

/* Thesis Tables 4.30 and 4.31                                          */
typedef struct features
{
   REAL place,
        manner;
   int  syllabic,
        voice,
        nasal,
        retroflex,
        lateral;
   REAL high,
        back;
   int  round;
}  FEATURES;


/************************************************************************/
/* Globals
*/

/************************************************************************/
/* Prototypes
*/
int main(int argc, char **argv);
FEATURES *InitLetterFeatures(void);
REAL CalcDelta(char p, char q, FEATURES *features);
REAL CalcV(char p, REAL Cvwl);

/************************************************************************/
int main(int argc, char **argv)
{
   FEATURES *letterFeatures = NULL;
#ifdef TEST
   char     *letters = "aiynprs";
#else
   char     *letters = "abcdefghijklmnopqrstuvwxyz";
#endif
   int      p, q;
   REAL     Cvwl = DEFAULT_CVWL;
   REAL     Csub = DEFAULT_CSUB;
   
   letterFeatures = InitLetterFeatures();

   printf("# Kondrak letter sound similarity matrix\n");
   printf("# Kondrak (2003) Computers and the Humanities 37:273-291\n");
   printf("# Kondrak PhD Thesis, Chapter 4\n");
   printf("# Parameters: Cvwl=%.1f Csub=%.1f\n", Cvwl, Csub);
   for(p=0; p<MAXLETTERS; p++)
   {
      for(q=0; q<MAXLETTERS; q++)
      {
         REAL delta = CalcDelta(letters[p], letters[q], letterFeatures);
         REAL Vp    = CalcV(letters[p], Cvwl);
         REAL Vq    = CalcV(letters[q], Cvwl);
         REAL qsub  = Csub - delta - Vp - Vq;
         
         if(qsub < 0.0)
         {
            printf("%4d", (int)(qsub - 0.5));
         }
         else
         {
            printf("%4d", (int)(qsub + 0.5));
         }
      }
      printf("\n");
   }

   for(p=0; p<MAXLETTERS; p++)
      printf("   %c", letters[p]);
   printf("\n");
   
   return(0);
}


/************************************************************************/
REAL CalcV(char p, REAL Cvwl)
{
   if(ISVOWEL(p))
      return(Cvwl);

   return(0.0);
}


/************************************************************************/
REAL CalcDelta(char p, char q, FEATURES *features)
{
   BOOL gotConsonant = (!ISVOWEL(p) || !ISVOWEL(q))?TRUE:FALSE;
   REAL delta = 0.0;

   if(gotConsonant)
   {
      delta += ABS(features[LETTER(p)].syllabic  -
                   features[LETTER(q)].syllabic ) * SALIENCE_SYLLABIC;
      delta += ABS(features[LETTER(p)].voice     -
                   features[LETTER(q)].voice    ) * SALIENCE_VOICE;
      delta += ABS(features[LETTER(p)].lateral   -
                   features[LETTER(q)].lateral  ) * SALIENCE_LATERAL;
      delta += ABS(features[LETTER(p)].manner    -
                   features[LETTER(q)].manner   ) * SALIENCE_MANNER;
      delta += ABS(features[LETTER(p)].place     -
                   features[LETTER(q)].place    ) * SALIENCE_PLACE;
      delta += ABS(features[LETTER(p)].nasal     -
                   features[LETTER(q)].nasal    ) * SALIENCE_NASAL;
      delta += ABS(features[LETTER(p)].retroflex -
                   features[LETTER(q)].retroflex) * SALIENCE_RETROFLEX;
      /* Plus Aspirated - not used                                      */
   }
   else
   {
      delta += ABS(features[LETTER(p)].syllabic  -
                   features[LETTER(q)].syllabic ) * SALIENCE_SYLLABIC;
      delta += ABS(features[LETTER(p)].high      -
                   features[LETTER(q)].high     ) * SALIENCE_HIGH;
      delta += ABS(features[LETTER(p)].nasal     -
                   features[LETTER(q)].nasal    ) * SALIENCE_NASAL;
      delta += ABS(features[LETTER(p)].back      -
                   features[LETTER(q)].back     ) * SALIENCE_BACK;
      delta += ABS(features[LETTER(p)].retroflex -
                   features[LETTER(q)].retroflex) * SALIENCE_RETROFLEX;
      delta += ABS(features[LETTER(p)].round     -
                   features[LETTER(q)].round    ) * SALIENCE_ROUND;
      /* Plus Long - not used                                           */
   }
   

   return(delta);
}


/************************************************************************/
FEATURES *InitLetterFeatures(void)
{
   static FEATURES features[] = {
   /* a */ {VELAR,         LOWVOWEL,    1,1,0,0,0, LOW,  CENTRAL, 0},
   /* b */ {BILABIAL,      STOP,        0,1,0,0,0, 0,    0,       0},
   /* c */ {ALVEOLAR,      AFFRICATIVE, 0,0,0,0,0, 0,    0,       0},
   /* d */ {ALVEOLAR,      STOP,        0,1,0,0,0, 0,    0,       0},
   /* e */ {PALATAL,       MIDVOWEL,    1,1,0,0,0, MID,  FRONT,   0},
   /* f */ {LABIODENTAL,   FRICATIVE,   0,0,0,0,0, 0,    0,       0},
   /* g */ {VELAR,         STOP,        0,1,0,0,0, 0,    0,       0},
   /* h */ {GLOTTAL,       FRICATIVE,   0,0,0,0,0, 0,    0,       0},
   /* i */ {PALATAL,       HIGHVOWEL,   1,1,0,0,0, HIGH, FRONT,   0},
   /* j */ {ALVEOLAR,      AFFRICATIVE, 0,1,0,0,0, 0,    0,       0},
   /* k */ {VELAR,         STOP,        0,0,0,0,0, 0,    0,       0},
   /* l */ {ALVEOLAR,      APPROXIMANT, 0,1,0,0,1, 0,    0,       0},
   /* m */ {BILABIAL,      STOP,        0,1,1,0,0, 0,    0,       0},
   /* n */ {ALVEOLAR,      STOP,        0,1,1,0,0, 0,    0,       0},
   /* o */ {VELAR,         MIDVOWEL,    1,1,0,0,0, MID,  BACK,    1},
   /* p */ {BILABIAL,      STOP,        0,0,0,0,0, 0,    0,       0},
   /* q */ {GLOTTAL,       STOP,        0,0,0,0,0, 0,    0,       0},
   /* r */ {RETROFLEX,     APPROXIMANT, 0,1,0,1,0, 0,    0,       0},
   /* s */ {ALVEOLAR,      FRICATIVE,   0,0,0,0,0, 0,    0,       0},
   /* t */ {ALVEOLAR,      STOP,        0,0,0,0,0, 0,    0,       0},
   /* u */ {VELAR,         HIGHVOWEL,   1,1,0,0,0, HIGH, BACK,    1},
   /* v */ {LABIODENTAL,   FRICATIVE,   0,1,0,0,0, 0,    0,       0},
   /* w */ {VELARBILABIAL, HIGHVOWEL,   1,1,0,0,0, HIGH, BACK,    1},
   /* x */ {VELAR,         FRICATIVE,   0,0,0,0,0, 0,    0,       0},
#ifdef AS_THESIS
   /* y */ {VELAR,         HIGHVOWEL,   1,1,0,0,0, HIGH, FRONT,   0},
#else
   /* y */ {PALATAL,       HIGHVOWEL,   0,1,0,0,0, HIGH, FRONT,   0},
#endif
   /* z */ {ALVEOLAR,      FRICATIVE,   0,1,0,0,0, 0,    0,       0}
   };

   return(features);
}


