#!/usr/bin/perl
use strict;

my @letters = qw/a b c d e f g h i j k l m n o p q r s t u v w x y z/;

for(my $i=0; $i<26; $i++)
{
    for(my $j=$i; $j<26; $j++)
    {
        print "$letters[$i] $letters[$j] ";
        print "10" if($letters[$i] eq $letters[$j]);
        print "\n";
    }
}
