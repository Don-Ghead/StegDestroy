#!/usr/bin/perl

#Script to insert bits in to a file at a starting position and then at every offset.
#This should allow for damaged steg payloads to be recovered by shifting the bits in to their correct alignment.

use strict;
use warnings;
use Data::Dumper;

my $InFile = shift(@ARGV); #File to insert bit in to
my $OutFile = shift(@ARGV); #File that will contain the inserted bits
my $Start = shift(@ARGV); #First byte to insert a bit at the start of
my $Offset = shift(@ARGV); #Every $Offset bytes after $Start will also have bits inserted
my $NumToInsert = shift(@ARGV); #Number of bits to insert
my $BitValue = shift(@ARGV); #Value of the bits that will be inserted

if (not $InFile)
{
	die "No file defined.\n";
}

if (not -f $InFile)
{
	die "$InFile not a file.\n";
}

if (not $OutFile)
{
	die "Outfile not defined.\n";
}

if (not defined $Start)
{
	die "Start not defined.\n";
}

$Start = $Start * 8; #Bit level start position

if (not defined $Offset)
{
	die "Offset not defined.\n";
}

if (not $NumToInsert)
{
	$NumToInsert = 1;
}

if (not $BitValue)
{
	$BitValue = 0;
}

my $Insert = ""; #String of bits that will be inserted
$Insert = $BitValue x $NumToInsert;

my $NumInserts = 0; #Keeps track of how many bits have been inserted so that the correct amount can be appended to the end.

my $InFileSize = (-s $InFile) * 8;

#Read in original file and unpack in to a binary string
open (my $inFH, "<", $InFile) or die "Failed to open $InFile. $!\n";
binmode $inFH;

my $File = ""; #Binary string of $InFile

while (read($inFH, my $Buffer, 1024))
{
	for (split //, $Buffer)
	{
		$File .= unpack("B8", $_);
	}
}

close $inFH;

#Perform first insert at start position;

substr ($File, $Start, 0) = $Insert;
$NumInserts++;

#Perform insert at each $Offset

if ($Offset != 0)
{
	$Offset = $Offset * 8; #Bit level offset
	
	my $InsertPoint = $Start + $Offset + 1;

	while ($InsertPoint < $InFileSize)
	{
		substr ($File, $InsertPoint, 0) = $Insert;
		
		$InsertPoint = $InsertPoint + $Offset + 1;
		
		$NumInserts ++;
	}
}

#All bits have been inserted. Append enough bits to keep the number of bits a multiple of 8

my $NumBitsToAppend = ($NumInserts * $NumToInsert) % 8;

$File .= $BitValue x $NumBitsToAppend;

#Write out new file

my @Array = ($File =~ m/.{8}/g);

open (my $outFH, ">", $OutFile) or die "Failed to open $OutFile. $!\n";
binmode $outFH;

for (@Array)
{
	print $outFH pack("B8", $_);
}