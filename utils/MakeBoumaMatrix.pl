#!/usr/bin/perl
# See https://en.wikipedia.org/wiki/Bouma

use strict;

my @letters = qw/a b c d e f g h i j k l m n o p q r s t u v w x y z/;
my %up  = (a=>0, b=>2, c=>0, d=>2, e=>0, f=>2, g=>0, h=>2, i=>1,
           j=>1, k=>2, l=>2, m=>0, n=>0, o=>0, p=>0, q=>0, r=>0,
           s=>0, t=>1, u=>0, v=>0, w=>0, x=>0, y=>0, z=>0);
my %down= (a=>0, b=>0, c=>0, d=>0, e=>0, f=>1, g=>2, h=>0, i=>0,
           j=>2, k=>0, l=>0, m=>0, n=>0, o=>0, p=>2, q=>2, r=>0,
           s=>0, t=>0, u=>0, v=>0, w=>0, x=>0, y=>2, z=>0);

# Print a header
print <<__EOF;
# Bouma letter shape similarity matrix
# Bouma (1971). Visual Recognition of Isolated Lower-Case Letters. 
#    Vision Research 11:459-474.
# Haber & Haber (1981) The Shape of a Word Can Specify Its Meaning.
#    Reading Research Quarterly 16:334-345 
#    (http://www.jstor.org/stable/747406)
# https://en.wikipedia.org/wiki/Word_recognition#Bouma_shape
# http://www.jamesrobertwatson.com/bouma.html
__EOF

my $maxUpScore   = FindMax(%up);
my $maxDownScore = FindMax(%down);
my $maxScore     = ($maxUpScore > $maxDownScore)?$maxUpScore:$maxDownScore;
    
# Calculate the difference in ascending and descending Bouma scores
foreach my $letter1 (@letters)
{
    foreach my $letter2 (@letters)
    {
        my $boomer = 0;

        $boomer  = abs($up{$letter1}   - $up{$letter2});
        $boomer += abs($down{$letter1} - $down{$letter2});

        printf("%4d", 10*($maxScore - $boomer));
    }
    print "\n";
}

foreach my $letter (@letters)
{
    printf("%4s", $letter);
}
print "\n";


sub FindMax
{
    my(%scores) = @_;
    my $maxValue = 0;
    
    foreach my $value (values %scores)
    {
        $maxValue = $value if($value > $maxValue);
    }
    return($maxValue);
}
