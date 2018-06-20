#!/usr/bin/perl
use strict;

main();

sub main
{
    my $inRow   = 0;
    my $inData  = 0;
    my $tdCount = 0;
    my @data    = ();
    
    my $date=`date +%d.%m.%y`;
    chomp $date;

    print "#Collected from mednet $date\n";
    print "#Entry,name,PL,RL,PLDate\n";

    my %plToYear = (
        '66' => '1991b',
        '74' => '1995b',
        '77' => '1997a',
        '78' => '1997b',
        '79' => '1998a',
        '80' => '1998b',
        '81' => '1999a',
        '82' => '1999b',
        '83' => '2000a',
        '84' => '2000b',
        '85' => '2001',
        '86' => '2002a',
        '87' => '2002b',
        '88' => '2003a',
        '89' => '2003b',
        '90' => '2004a',
        '91' => '2004b',
        '92' => '2004c',
        '93' => '2005a',
        '94' => '2005b',
        '95' => '2006a',
        '96' => '2006b',
        '97' => '2007a',
        '98' => '2007b',
        '99' => '2008a',
        '100' => '2008b',
        '101' => '2009a',
        '102' => '2009b',
        '103' => '2010a',
        '104' => '2010b',
        '105' => '2011a',
        '106' => '2011b',
        '107' => '2012a',
        '108' => '2012b',
        '109' => '2013a',
        '110' => '2013b',
        '111' => '2014a',
        '112' => '2014b',
        '113' => '2015a',
        '114' => '2015b',
        '115' => '2016a',
        '116' => '2016b',
        '117' => '2017a',
        '118' => '2017b',
        '119' => '2018a',
        '120' => '2018b');
    
    while(<>)
    {
        chomp;
        s/^\s+//;
        s/\s+$//;
        if(/\<tr\>/ || /\<tr /)
        {
            $inRow = 1;
            $tdCount = 0;
            @data    = ();
        }
        elsif(/\<\/tr\>/)
        {
            $inRow   = 0;
            $tdCount = 0;
            if(scalar(@data))
            {
                foreach my $datum (@data)
                {
                    $datum = StripHTML($datum);
                }
                if(isAntibody($data[3]))
                {
                    $data[5] = '00' if($data[5] eq '');
                    print "$data[2],$data[3],$data[4],$data[5],$plToYear{$data[4]}\n";
#                    print "$data[2],$data[3],$data[4],$data[5]\n";
                }
            }
            @data    = ();
        }
        elsif($inRow)
        {
            if(/\<td\>(.*)\<\/td\>/)
            {
                $data[$tdCount++] = $1;
            }
            elsif(/\<td\>/)
            {
                $inData = 1;
            }
            elsif(/\<\/td\>/)
            {
                $inData = 0;
                $tdCount++;
            }
            elsif($inData)
            {
                $data[$tdCount] .= $_;
            }
        }
    }
}

sub StripHTML
{
    my($in) = @_;
    my $out = '';
    my @intext = split(//, $in);
    my $inMarkup = 0;
    foreach my $char (@intext)
    {
        if($char eq '<')
        {
            $inMarkup = 1;
        }
        elsif($char eq '>')
        {
            $inMarkup = 0;
        }
        elsif(!$inMarkup)
        {
            $out .= $char;
        }
    }

    return($out);
}

sub isAntibody
{
    my($name) = @_;
    my @fields = split(/\s+/, $name);
    foreach my $field (@fields)
    {
        return(1) if($field =~ /mab$/);
    }

    return(0);
}
