#!/usr/bin/perl -s

$::s = (defined($::s)?1:0);
$::m = (defined($::m)?1:0);
$::c = (defined($::c)?1:0);

$::s = 1 if((!$::s) && (!$::m));  # If neither set, then set $::s

UsageDie() if(defined($::h));


use strict;

my $dataset = 0;
my @names   = ();

print "#set Phonetics Letter-similarity Bouma\n";

while(<>)
{
    chomp;
    s/^\s+//;
    s/\s+$//;
    if(/^#\s*(.*)/)      # Start of new dataset
    {
        $dataset = $1;
        @names = () if(!$::c);
    }
    elsif(!length())     # End of dataset
    {
        Analyze($dataset, $::m, $::s, @names);
    }
    else                 # Store a name
    {
        push @names, $_;
    }
}

sub Analyze
{
    my($dataset, $mean, $sd, @names) = @_;
    my $file   = "$dataset.txt";
    my $result = "$dataset.dat";
    
    WriteFile($file, @names);

    my $p = doAnalyze($file, $result, '-p', $mean, $sd);
    my $s = doAnalyze($file, $result, '-s', $mean, $sd);
    my $b = doAnalyze($file, $result, '-b', $mean, $sd);

    if(($p > 0.0) && ($s > 0.0) && ($b > 0.0))
    {
        print "$dataset $p $s $b\n";
    }

    unlink($file);
    unlink($result);
}

sub doAnalyze
{
    my($file, $result, $flag, $mean, $sd) = @_;

    `abcheckname -a $flag $file | awk '{print \$4}' > $result`;
    my $meansd = `calcsd $result`;
    $meansd =~ s/^\s+//;
    $meansd =~ s/\s+$//;
    $meansd =~ s/,//g;
    
    my @fields = split(/\s+/, $meansd);
    my $value = 0;
    if($mean)
    {
        $value  = $fields[1];
        $value += 3.0 * $fields[3] if($sd);
    }
    elsif($sd)
    {
        $value = $fields[3];
        $value = 0.0 if($value =~ /nan/);
    }
    else
    {
        die("Internal error: $::s or $::m must be set");
    }
        
    return(sprintf("%.2f ", $value))
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

sub UsageDie
{
    print <<__EOF;

diversityOverTime V1.1 (c) 2019-19, UCL, Dr Andrew C.R Martin

Usage: diversityOverTime [-s][-m][-c] namesbyparam.dat
       -s Output standard deviation  [Default]
       -m Output mean
       -c Calculate values cummulatively
       Output the sum of mean and standard deviation if both set

Takes the output of namesOvertime - i.e. a set of names that were used
in each year (or previous years) with each set introduced by a comment
giving the year and ended with a blank line. Calculates the variation
in the names and displays this as the mean, standard deviation or sum
of the two.

Note that depending on the input, the groups can be by proposed list
or recommended list instead of by year.

The input looks like
   # 1989
   abname
   abname
   abname

   # 1990
   abname
   abname

   ...etc...
    

__EOF

    exit 0;
}
