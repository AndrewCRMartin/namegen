#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "bioplib/macros.h"
#include "bioplib/SysDefs.h"
#include "bioplib/MathType.h"

#define MAXLETTERS 26
#define LETTER(x) (int)(islower(x)?(char)toupper(x):x)-65
#define ISVOWEL(a) ((a == 'a') || (a == 'e') || (a == 'i') || (a == 'o') || (a == 'u') || (a == 'y'))

/* Table 4.28: Place */
#define BILABIAL       1.00
#define LABIODENTAL    0.95
#define DENTAL         0.90
#define ALVEOLAR       0.85
#define RETROFLEX      0.80
#define PALATOALVEOLAR 0.75
#define PALATAL        0.70
#define VELAR          0.60
#define UVULAR         0.50
#define PHARANGEAL     0.30
#define GLOTTAL        0.10
#define VELARBILABIAL  0.80 /* Mean of BILABIAL and VELAR */

/* Table 4.28: Manner */
#define STOP        1.0
#define AFFRICATIVE 0.9
#define FRICATIVE   0.8
#define APPROXIMANT 0.6
#define HIGHVOWEL   0.4
#define MIDVOWEL    0.2
#define LOWVOWEL    0.0

/* Table 4.28: High */
#define HIGH        1.0 
#define MID         0.5
#define LOW         0.0

/* Table 4.28: Back */
#define FRONT       1.0
#define CENTRAL     0.5
#define BACK        0.0

#define SALIENCE_SYLLABIC   5
#define SALIENCE_VOICE     10
#define SALIENCE_LATERAL   10
#define SALIENCE_HIGH       5
#define SALIENCE_MANNER    50
#define SALIENCE_PLACE     40
#define SALIENCE_NASAL     10
#define SALIENCE_BACK       5
#define SALIENCE_RETROFLEX 10
#define SALIENCE_ROUND      5
#define SALIENCE_LONG       1 /* Unused */
#define SALIENCE_ASPIRATED  5 /* Unused */

/* Tables 4.30 and 4.31 */
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



int main(int argc, char **argv);
REAL CalcDelta(char p, char q, FEATURES *features);
FEATURES *InitLetterFeatures(void);
REAL CalcV(char p, REAL Cvwl);


   

int main(int argc, char **argv)
{
   FEATURES *letterFeatures = NULL;
   char     *letters = "abcdefghijklmnopqrstuvwxyz";
   int      p, q;
   REAL     Cvwl = 10;
   REAL     Csub = 35;
/*   REAL     Cexp = 45; */
   
   letterFeatures = InitLetterFeatures();

   for(p=0; p<MAXLETTERS; p++)
   {
      for(q=0; q<MAXLETTERS; q++)
      {
         REAL delta = CalcDelta(letters[p], letters[q], letterFeatures);
         REAL Vp    = CalcV(letters[p], Cvwl);
         REAL Vq    = CalcV(letters[q], Cvwl);
         REAL qsub  = Csub - delta - Vp - Vq;
         
         printf("%c %c: %f\n", letters[p], letters[q], qsub);
      }
   }
   
   return(0);
}


REAL CalcV(char p, REAL Cvwl)
{
   if(ISVOWEL(p))
      return(Cvwl);

   return(0.0);
}


REAL CalcDelta(char p, char q, FEATURES *features)
{
   BOOL gotConsonant = (!ISVOWEL(p) || !ISVOWEL(q))?TRUE:FALSE;
   REAL delta = 0.0;

   if(gotConsonant)
   {
      delta += ABS(features[LETTER(p)].syllabic  - features[LETTER(q)].syllabic ) * SALIENCE_SYLLABIC;
      delta += ABS(features[LETTER(p)].voice     - features[LETTER(q)].voice    ) * SALIENCE_VOICE;
      delta += ABS(features[LETTER(p)].lateral   - features[LETTER(q)].lateral  ) * SALIENCE_LATERAL;
      delta += ABS(features[LETTER(p)].manner    - features[LETTER(q)].manner   ) * SALIENCE_MANNER;
      delta += ABS(features[LETTER(p)].place     - features[LETTER(q)].place    ) * SALIENCE_PLACE;
      delta += ABS(features[LETTER(p)].nasal     - features[LETTER(q)].nasal    ) * SALIENCE_NASAL;
      delta += ABS(features[LETTER(p)].retroflex - features[LETTER(q)].retroflex) * SALIENCE_RETROFLEX;
      /* Plus Aspirated */
   }
   else
   {
      delta += ABS(features[LETTER(p)].syllabic  - features[LETTER(q)].syllabic ) * SALIENCE_SYLLABIC;
      delta += ABS(features[LETTER(p)].high      - features[LETTER(q)].high     ) * SALIENCE_HIGH;
      delta += ABS(features[LETTER(p)].nasal     - features[LETTER(q)].nasal    ) * SALIENCE_NASAL;
      delta += ABS(features[LETTER(p)].back      - features[LETTER(q)].back     ) * SALIENCE_BACK;
      delta += ABS(features[LETTER(p)].retroflex - features[LETTER(q)].retroflex) * SALIENCE_RETROFLEX;
      delta += ABS(features[LETTER(p)].round     - features[LETTER(q)].round    ) * SALIENCE_ROUND;
      /* Plus Long */
   }
   

   return(delta);
}


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
   /* y */ {VELAR,         HIGHVOWEL,   1,1,0,0,0, HIGH, FRONT,   0},
   /* z */ {ALVEOLAR,      FRICATIVE,   0,1,0,0,0, 0,    0,       0}
   };

   return(features);
}


