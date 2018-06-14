#!/usr/bin/perl

use strict;

my $dataset = 0;
my @names   = ();

print "#set sound letter bouma\n"
while(<>)
{
    chomp;
    s/^\s+//;
    s/\s+$//;
    if(/^#\s*(.*)/)      # Start of new dataset
    {
        $dataset = $1;
        @names = ();
    }
    elsif(!length())     # End of dataset
    {
        Analyze($dataset, @names);
    }
    else                 # Store a name
    {
        push @names, $_;
    }
}

sub Analyze
{
    my($dataset, @names) = @_;
    my $file   = "$dataset.txt";
    my $result = "$dataset.dat";
    
    WriteFile($file, @names);

    print "$dataset ";

    doAnalyze($file, $result, '-p');
    doAnalyze($file, $result, '-s');
    doAnalyze($file, $result, '-b');
    print "\n";

    unlink($file);
}

sub doAnalyze
{
    my($file, $result, $flag) = @_;

    `checkname -a $flag $file | awk '{print \$4}' > $result`;
    my $meansd = `calcsd $result`;
    $meansd =~ s/^\s+//;
    $meansd =~ s/\s+$//;
    $meansd =~ s/,//g;
    
    my @fields = split(/\s+/, $meansd);
    printf("%.2f ", $fields[1]+$fields[3]);
}


sub WriteFile
{
    my($file, @names) = @_;

    if(open(my $fp, ">$file"))
    {
        foreach my $name (@names)
        {
            print $fp "$name\n";
        }
        close $fp;
    }
}

