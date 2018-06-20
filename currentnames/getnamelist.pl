#!/usr/bin/perl
use strict;

my %shortNames = ();
while(<>)
{
    chomp;
    my @fields = split(/\,/);
    my $name = $fields[1];
    @fields = split(/\s+/, $name);
    foreach my $field (@fields)
    {
        
        if($field =~ /mab$/)
        {
            $shortNames{$field} = 1;
            last;
        }
    }
}

foreach my $name (sort keys %shortNames)
{
    print "$name\n";
}
