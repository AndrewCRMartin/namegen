#!/usr/bin/perl
use strict;

my $titleLine = <>;
chomp $titleLine;
$titleLine =~ s/^#//;
my @titles = split(/\s+/, $titleLine);
my $nFields = int(@titles);

my %data;

while(<>)
{
    chomp;
    my @fields = split;
    for(my $i=0; $i<$nFields; $i++)
    {
        push @{$data{$titles[$i]}}, $fields[$i];
    }
}

for(my $i=1; $i<$nFields; $i++)
{
    my $title = $titles[$i];
    print "newset\n";
    print "% $title\n";

    my $count = 0;
    foreach my $set (@{$data{$titles[0]}})
    {
        if($set ne $titles[0])
        {
            printf "$set %.2f\n", $data{$title}[$count++];
        }
    }
}
