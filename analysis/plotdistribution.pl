#!/usr/bin/perl
use strict;

my $xlab = "Score";
my $ylab = "Frequency";
my $date = shift(@ARGV);

my @fileNames = ('phonetics', 'letter',            'bouma');
my @labels    = ('Phonetics', 'Letter similarity', 'Bouma');
my $count     = 0;
my @data      = ();

print <<__EOF;
epsf
boxed
xlabel "$xlab"
ylabel "$ylab"
marksize 10
__EOF

my $xmin = 0;
my $xmax = 100;
my $ymin = 10000000000;
my $ymax = 0;
    
my $pen = 0;
for(my $fileCount=0; $fileCount<scalar(@fileNames); $fileCount++)
{
    my $filename = $fileNames[$fileCount] . "_" . $date . ".bars";

    if(open(my $fp, '<', $filename))
    {
        # Create a new plot
        $pen++;
        push @data, "% $filename";
        push @data, "newset";
        push @data, "pen $pen";
        push @data, "datapoint $pen $pen 1";
        push @data, "style 1";

        while(<$fp>)
        {
            chomp;
            # Store the data
            push @data, $_;

            # Update min and max values
            my ($x, $y) = split;
            $ymin = $y if($y < $ymin);
            $ymax = $y if($y > $ymax);
        }
        close($fp);
    }
    else
    {
        print STDERR "Error: Can't open file ($filename)\n";
        exit 1;
    }
}


printf "keypos %.2f %.2f\n", $xmin + 0.1*($xmax-$xmin), $ymin + 0.9*($ymax-$ymin);

for(my $i=0; $i<scalar(@labels); $i++)
{
    my $pen = $i+2;
    print "keytext $pen \"$labels[$i]\"\n";
}


foreach my $datum (@data)
{
    print "$datum\n";
}
