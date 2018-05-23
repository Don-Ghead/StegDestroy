#!/usr/bin/perl

use strict;
use warnings;
use bytes;
use Data::Dumper;
use Getopt::Long qw(GetOptions);


#Tool to peform steanography on PPM and PGM digital images.

no warnings "experimental::smartmatch";

#Implemented modes of steganography
my %StegModes =
(
	"SEQ" => 
	{
		"Hide" => \&SeqHide, #Function ref to hide payload
		"Extract" => \&SeqExtract, #Function ref to extract payload
		"MaxPayload" => \&SeqMaxPayload, #Function ref to calculate maximum payload for cover
	},
	"MOSAIC" =>
	{
		"Hide" => \&MosaicHide,
		"Extract" => \&MosaicExtract,
		"MaxPayload" => \&MosaicMaxPayload,
		"ValidateConfig" => \&MosaicValidateConfig #Function ref to validate config specific to this mode
	}
);

# Supported NetPBM headers with number of bytes per pixel and format name
my %Types =
(
	"P5" =>
	{
		"BPP" => 1, #Bytes per pixel
		"Name" => "PGM",
	},
	"P6" =>
	{
		"BPP" => 3,
		"Name" => "PPM",
	},
);

#Info about an image that has been loaded, either cover or steg.
#This is purely here for documentation purposes, the HideMode and ExtractMode functions
#will duplicate this hash.
my %Info = 
(
	"Type" => "", #Type of image PGM or PPM
	"Width" => "", #Width of the image
	"Height" => "", #Height of the image
	"MaxVal" => "", #MaxVal of the image
	"DataStart" => "", #Start position of the image data
	"DataSize" => "", #Size of the image data in bytes
	"HeaderData" => "", #Raw header data of the image
	"ImageData" => "", #Image data in 2 dimensional array
	"MaxPayloadSize" => "", #Max payload the image can support in bytes
	"Size" => "", #File size
	"BPP" => "", #Number of bytes per pixel
);

#Modes of operation
my $CoverImage; #If defined tool is hiding a file. Value should be the cover image
my $StegImage; #If defined tool is extracting a hidden file. Value should be the steg image
my $HelpMode; #Print help

#General configuration
my $Output; #Output file. If HideMode its the steg image. If ExtractMode its the payload.

#Configuration for hiding a file
my $Payload; #File that will be hidden.
my $Mode = "SEQ"; #Embedding mode 

#Configuration needed for extracting a file
my $PayloadSize; #Size of the payload to extract;

#Configuration for mosaic embedding
my $MosaicX = 200; #Number of pixels (width) to use for embedding
my $MosaicY = 200; #Number of pixels (height) to use for embedding
my $MosaicPos = "TL"; #Position to extract the payload from. Top left, top right etc.

#Get command line arguments
GetOptions
(
	"h=s" => \$CoverImage,
	"e=s" => \$StegImage,
	"help" => \$HelpMode,
	"o=s" => \$Output,
	"p=s" => \$Payload,
	"z=i" => \$PayloadSize,
	"m=s" => \$Mode,
	"x=i" => \$MosaicX,
	"y=i" => \$MosaicY,
	"mp=s" => \$MosaicPos,
) or PrintUsage();

if ($HelpMode)
{
	PrintUsage();
}

#Validate arguments

#Must specify either a cover image or a steg image
if (not $CoverImage and not $StegImage)
{
	PrintUsage("Must specify a mode of operation");
}

#Cannot specify both a cover image and a steg image
if ($CoverImage and $StegImage)
{
	PrintUsage("Cannot specify HideMode and ExtractMode");
}

#Verify defined mode is valid
$Mode = uc $Mode;

if (not $Mode ~~ [keys %StegModes])
{
	PrintUsage("Unknown mode specified");
}

#If in hide mode validate relevant configuration then run
if ($CoverImage)
{
	if ($Payload)
	{
		if (not -f $Payload)
		{
			PrintUsage("Payload file does not exist");
		}
	}
	else
	{
		PrintUsage("Must define a payload to hide");
	}
	
	if (not -f $CoverImage)
	{
		PrintUsage("Cover image does not exist");
	}
	
	#Validate additional configuration.
	#Validation depends on embedding mode specified
	if (defined $StegModes{$Mode}{"ValidateConfig"})
	{
		$StegModes{$Mode}{"ValidateConfig"}();
	}

	Hide();
}
#If in extraction mode validate relevant configuration then run
elsif ($StegImage)
{
	if (not -f $StegImage)
	{
		PrintUsage("Steg image does not exist");
	}
	
	if ($PayloadSize)
	{
		if (not $PayloadSize =~ /^\d+$/)
		{
			PrintUsage("Payload size must be a number");
		}
	}
	else
	{
		PrintUsage("Must define payload size to extract");
	}
	
	#Validate additional configuration.
	#Validation depends on embedding mode specified
	if (defined $StegModes{$Mode}{"ValidateConfig"})
	{
		$StegModes{$Mode}{"ValidateConfig"}();
	}
	
	Extract();
}

#Validate additional configuration for mosaic embedding
sub MosaicValidateConfig
{
	$MosaicPos = uc $MosaicPos;
	
	if ($MosaicX)
	{
		if (not $MosaicX =~ /^\d*$/)
		{
			PrintUsage("Mosaic tile width must be a number");
		}
	}
	else
	{
		PrintUsage("If specifying Mosaic tile width (-x) a value must be provided. Default is 200");
	}
	

	
	if ($MosaicY)
	{
		if (not $MosaicY =~ /^\d*$/)
		{
			PrintUsage("Mosaic tile height must be a number");
		}
	}
	else
	{
		PrintUsage("If specifying Mosaic tile height (-y) a value must be provided. Default is 200");
	}
	
	if ($MosaicPos)
	{
		if (not $MosaicPos ~~ ["TL", "TR", "BL", "BR"])
		{
			PrintUsage("MosaicPos (-mp) must be one of the following: 'TL', 'TR', 'BL', 'BR'.")
		}
	}
}

sub Extract
{	
	#Default Output name
	if (not $Output)
	{
		$Output = "ExtractedPayload";
	}
	
	#Hash containing info about the steg image
	my %StegInfo = %Info;
	
	#Extracted payload file
	open(my $OutputFH, ">", $Output) or die "Failed to open output file. $!\n";
	binmode $OutputFH;
	
	LoadImage($StegImage,\%StegInfo);
	
	#Call appropriate extract mode
	$StegModes{$Mode}{"Extract"}(\%StegInfo, $OutputFH);
}

sub SeqExtract
{
	my $StegInfo = shift(@_);
	my $OutputFH = shift(@_);
	
	#Keep track of how many payload bytes have been extracted
	my $BytesExtracted = 0;
	
	my $y = 0;
	my $x = 0;
	my $PayloadByte = ""; #Build up a byte until it can be written to OutputFH
	
	EmbedLoop: while ($y < $StegInfo->{"Height"})
	{
		while ($x < ($StegInfo->{"Width"} * $StegInfo->{"BPP"}))
		{
			#Extract payload bit
			ExtractBit(\$StegInfo->{"ImageData"}[$y][$x], \$PayloadByte);
			
			#If PayloadByte is 8 characters long we have an entire payload byte.
			if (length $PayloadByte == 8)
			{
				#Write out PayloadByte and reset PayloadByte
				print $OutputFH pack("B8", $PayloadByte);
				$PayloadByte = "";
				$BytesExtracted++;
			}
			
			if ($BytesExtracted == $PayloadSize)
			{
				last EmbedLoop;
			}
			
			$x++;
		}
	
		$x = 0;
		$y++;
	}
}

sub ExtractBit
{
	my $StegByteRef = shift(@_);
	my $PayloadByteRef = shift(@_);
	
	if ($$StegByteRef % 2 == 0)
	{
		$$PayloadByteRef .= 0;
	}
	else
	{
		$$PayloadByteRef .= 1;
	}
}

sub Hide
{	
	#Hash containing information about the cover image
	my %CoverInfo = %Info;

	#Load cover image, validate and read image data
	LoadImage($CoverImage,\%CoverInfo);
	
	#Calculate max payload size of the cover image
	$StegModes{$Mode}{"MaxPayload"}(\%CoverInfo);
	
	#Verify that payload can fit in cover
	if (-s $Payload > $CoverInfo{"MaxPayloadSize"})
	{
		die "Payload is too large. Maximum payload size for this image is: $CoverInfo{'MaxPayloadSize'} bytes.\n";
	}
	
	#File to hide
	open (my $PayloadFH, "<", $Payload) or die "Failed to open payload. $!\n";
	binmode $PayloadFH;
	
	#Steg image
	
	#Default output name
	if (not $Output)
	{
		$Output = "StegImage." . $Types{$CoverInfo{"Type"}}{"Name"};
	}
	
	open (my $StegFH, ">", "$Output") or die "Failed to open outfile. $!\n";
	binmode $StegFH;
	
	#Write image header from cover image to steg image
	WriteHeader($CoverInfo{"HeaderData"}, $StegFH);
	
	#Call appropriate hide mode to hide payload in $CoverInfo->{"ImageData"}
	$StegModes{$Mode}{"Hide"}(\%CoverInfo, $PayloadFH, $StegFH);
	
	#All payload is hidden in $CoverInfo->{"ImageData"}, write to steg file
	WriteImageData(\%CoverInfo,$StegFH);
	
	#Print summary
	print "Payload embedded. Payload size: " . (-s $Payload) . ".\n";
}

#Calculate maximum payload for a standard sequential embedding
sub SeqMaxPayload
{
	my $CoverInfo = shift(@_);
	
	$CoverInfo->{"MaxPayloadSize"} = $CoverInfo->{"DataSize"} / 8;
}

#Calculate maximum payload for a mosaic embedding
sub MosaicMaxPayload
{
	my $CoverInfo = shift(@_);
	
	if ($MosaicX > $CoverInfo->{"Width"})
	{
		die "Mosaic width of $MosaicX is greater than the width of the image.\n"
	}
	
	if ($MosaicY > $CoverInfo->{"Height"})
	{
		die "Mosaic height of $MosaicY is greater than the heght of the image.\n";
	}
	
	$CoverInfo->{"MaxPayloadSize"} = ($MosaicX * $MosaicY * $CoverInfo->{"BPP"}) / 8;
}

#Hide data in a mosaic format
sub MosaicHide
{
	my $CoverInfo = shift(@_);
	my $PayloadFH = shift(@_);
	my $StegFH = shift(@_);
	
	#MosaicX is multiplied by the number of bytes per pixel.
	$MosaicX = $MosaicX * $CoverInfo->{"BPP"};
	
	#Based on the dimensions of the image we may be able to embed in 4, 2 or 1 possible locations.
	my @TilePositions;
	
	#The possible offsets
	my @TileTopLeft = ("0","0"); #Offsets for top left tile
	my @TileTopRight = (($CoverInfo->{"Width"}  * $CoverInfo->{"BPP"}) - $MosaicX, 0); #Possible offset for the top right tile
	my @TileBottomLeft = ("0", $CoverInfo->{"Height"} - $MosaicY);
	my @TileBottomRight = (($CoverInfo->{"Width"}  * $CoverInfo->{"BPP"}) - $MosaicX, $CoverInfo->{"Height"} - $MosaicY);
	
	#Determine tile positions are appropriate for the image which are appropriate
	if ($MosaicX * 2 < ($CoverInfo->{"Width"} * $CoverInfo->{"BPP"}) && $MosaicY * 2 < $CoverInfo->{"Height"})
	{
		push (@TilePositions, [@TileTopLeft]);
		push (@TilePositions, [@TileTopRight]);
		push (@TilePositions, [@TileBottomLeft]);
		push (@TilePositions, [@TileBottomRight]);
	}
	elsif ($MosaicX * 2 < ($CoverInfo->{"Width"} * $CoverInfo->{"BPP"}))
	{
		push (@TilePositions, [@TileTopLeft]);
		push (@TilePositions, [@TileTopRight]);
	}
	elsif ($MosaicY * 2 < $CoverInfo->{"Height"})
	{
		push (@TilePositions, [@TileTopLeft]);
		push (@TilePositions, [@TileBottomLeft]);
	}
	else
	{
		push (@TilePositions, [@TileTopLeft]);
	}
	
	my $x = 0;
	my $y = 0;
	
	#Read in the payload 1024 bytes at a time
	while (read($PayloadFH, my $PayloadBuffer, 1024))
	{
		#Iterate through each byte in buffer
		for (split //, $PayloadBuffer)
		{
			my $Byte = unpack("B8", $_);
			
			#Iterate through each bit in the byte
			for (split //, $Byte)
			{
				my $Bit = $_;
				
				#Hide the bit in all tile locations
				for (@TilePositions)
				{
					my $TileXOffset = @{$_}[0];
					my $TileYOffset = @{$_}[1];
					
					EmbedBit(\$Bit, \$CoverInfo->{"ImageData"}[$y + $TileYOffset][$x + $TileXOffset]);
				}
				
				$x++;
				
				if ($x == $MosaicX)
				{
					$x = 0;
					$y++;
				}
			}
		}
	}
}

sub MosaicExtract
{
	my $StegInfo = shift(@_);
	my $OutputFH = shift(@_);
	
	#MosaicX is multiplied by the number of bytes per pixel.
	$MosaicX = $MosaicX * $StegInfo->{"BPP"};
	
	my $XOffset; #Starting X position for the tile
	my $YOffset;
	
	if ($MosaicPos eq "TL")
	{
		$XOffset = 0;
		$YOffset = 0;
	}
	elsif ($MosaicPos eq "TR")
	{
		$XOffset = ($StegInfo->{"Width"} * $StegInfo->{"BPP"}) - $MosaicX;
		$YOffset = 0;
	}
	elsif ($MosaicPos eq "BL")
	{
		$XOffset = 0;
		$YOffset = $StegInfo->{"Height"} - $MosaicY;
	}
	elsif ($MosaicPos eq "BR")
	{
		$XOffset = ($StegInfo->{"Width"} * $StegInfo->{"BPP"}) - $MosaicX;
		$YOffset = $StegInfo->{"Height"} - $MosaicY;
	}
	else
	{
		die "Should be here.\n";
	}
	
	#Keep track of how many payload bytes have been extracted
	my $BytesExtracted = 0;
	
	my $y = 0;
	my $x = 0;
	my $PayloadByte = ""; #Build up a byte until it can be written to OutputFH
	
	EmbedLoop: while ($y < $MosaicY)
	{
		while ($x < $MosaicX)
		{
			ExtractBit(\$StegInfo->{"ImageData"}[$y + $YOffset][$x + $XOffset], \$PayloadByte);
			
			#If PayloadByte is 8 characters long we have an entire payload byte.
			if (length $PayloadByte == 8)
			{
				#Write out PayloadByte and reset PayloadByte
				print $OutputFH pack("B8", $PayloadByte);
				$PayloadByte = "";
				$BytesExtracted++;
			}
			
			if ($BytesExtracted == $PayloadSize)
			{
				last EmbedLoop;
			}

			$x++;
		}
		
		$x = 0;
		$y++;
	}
}

sub EmbedBit
{
	my $PayloadBitRef = shift(@_);
	my $CoverByteRef = shift(@_);
	
	if ($$PayloadBitRef == 1 && ($$CoverByteRef % 2 == 0 ))
	{
		#Cover LSB is 0 needs to be 1, increment it.
		$$CoverByteRef++;
	}				
	elsif ($$PayloadBitRef == 0 && ($$CoverByteRef % 2 == 1))
	{
		#Cover LSB is 1 needs to be 0, decrement it.
		$$CoverByteRef--;
	}
}

#Hide data sequentially
sub SeqHide
{
	my $CoverInfo = shift(@_);
	my $PayloadFH = shift(@_);
	my $StegFH = shift(@_);
	
	#Sequentially embed payload in $CoverInfo->{"ImageData"}
	my $x = 0;
	my $y = 0;
	
	#Begin reading payload file 1024 bytes at time
	while (read($PayloadFH, my $PayloadBuffer, 1024))
	{		
		#Iterate through each byte in payload buffer
		for (split //, $PayloadBuffer)
		{
			#Get binary representation of the byte
			my $Byte = unpack("B8", $_);
			
			#Iterate through each bit in the byte
			for (split //, $Byte)
			{
				my $Bit = $_;
				
				#Embed payload bit
				EmbedBit(\$Bit, \$CoverInfo->{"ImageData"}[$y][$x]);
				
				$x++;
				
				#Once we have reached the end of a row in the imagedata move to the next row.
				if ($x == ($CoverInfo->{"Width"} * $CoverInfo->{"BPP"}))
				{				
					$x = 0;
					$y++;
				}
			}
		}
	}
}

#Write header of the original cover image to the new steg image
sub WriteHeader
{
	my $Header = shift(@_);
	my $StegFH = shift(@_);

	print $StegFH pack("H*",$Header);
}

sub WriteImageData
{
	my $CoverInfo = shift(@_);
	my $StegFH = shift(@_);
	
	my $x = 0;
	my $y = 0;
	
	while ($y < $CoverInfo->{"Height"})
	{
		while ($x < ($CoverInfo->{"Width"} * $CoverInfo->{"BPP"}))
		{
			print $StegFH pack("C",$CoverInfo->{"ImageData"}[$y][$x]);
			$x++;
		}
		
		$x = 0;
		$y++;
	}
}

sub LoadImage
{	
	my $Image = shift(@_);
	my $Info = shift(@_);

	my @Whitespace = ("09", "0a", "0d", "20"); #Possible white space values
	my @NewLine = ("0a", "0d"); #Possible new line values

	$Info->{"Size"} = -s $Image;
	
	open (my $fh, "<", $Image) or die "Failed to open image. $!\n";

	binmode $fh;
	
	#States when reading the header of the cover image
	my $State = 0;
	#Possible states
	my %States =
	(
		0 => "TYPE",
		1 => "WHITESPACE",
		2 => "WIDTH",
		3 => "WHITESPACE",
		4 => "HEIGHT",
		5 => "WHITESPACE",
		6 => "MAXVAL",
		7 => "WHITESPACE",
		8 => "DATA",
	);
	#Special state when parsing comments as this state can occur at any time.
	my $CommentState = 0;

	#Parse header of the cover image
	while ($States{$State} ne "DATA")
	{
		read ($fh, my $Byte, 1);
		my $Hex = unpack("H*", $Byte);
		
		#Store the header of the image
		$Info->{"HeaderData"} .= $Hex;
		
		#If we are in a whitespace state and we encounter a hash then skip bytes until we encounter a new line
		if ($Hex eq "23" && $States{$State} eq "WHITESPACE")
		{
			$CommentState = 1;
		}
		else
		{
			#If we are in the comment state and we encounter a new line then the comment has ended. Leave comment state
			if ($CommentState)
			{
				if ($Hex ~~ @NewLine)
				{
					$CommentState = 0;
				}
			}
			else
			{
				#If we find whitespace and we are NOT in a whitespace state then transition to next state
				if ($Hex ~~ @Whitespace && $States{$State} ne "WHITESPACE")
				{
					$State++;
				}
				#If we do not find whitespace and we ARE in a whitespace state then transition to next state
				elsif ((not $Hex ~~ @Whitespace) && $States{$State} eq "WHITESPACE")
				{
					$State ++;
				}

				if ($States{$State} eq "TYPE")
				{
					$Info->{"Type"} .= $Byte;
					
					if (length $Info->{"Type"} > 2)
					{
						die "Image is not a NetPBM image.\n";
					}
				}
				elsif ($States{$State} eq "WIDTH")
				{
					$Info->{"Width"} .= $Byte;
				}
				elsif ($States{$State} eq "HEIGHT")
				{
					$Info->{"Height"} .= $Byte;
				}
				elsif ($States{$State} eq "MAXVAL")
				{
					$Info->{"MaxVal"} .= $Byte;
				}
			}
		}
	}
	
	#Save start position of image data
	$Info->{"DataStart"} = (tell $fh) - 1; #Remove 1 as we will have read the first byte of the data
	
	#Similar to above remove the last hex byte of the header data
	$Info->{"HeaderData"} = substr($Info->{"HeaderData"}, 0, -2);
	
	#Verify that type is supported
	if (not $Info->{"Type"} ~~ [keys %Types])
	{
		die "Image is not a supported file format. Type: $Info->{'Type'}.\n";
	}
	
	#Verify that width is numeric
	ValidateIsNumeric($Info->{"Width"},"Width");
	
	#Verify that height is numeric
	ValidateIsNumeric($Info->{"Height"},"Height");
	
	#Verify that maxval is numeric
	ValidateIsNumeric($Info->{"MaxVal"},"MaxVal");
	
	#Assign Bytes per pixel based on the images type
	$Info->{"BPP"} = $Types{$Info->{"Type"}}{"BPP"};
	
	my $DataSize = $Info->{"Size"} - $Info->{"DataStart"};
	
	if ($DataSize != $Info->{"Width"} * $Info->{"Height"} * $Info->{"BPP"})
	{
		print "DataSize is incorrect\n";
	}
	
	$Info->{"DataSize"} = $DataSize;
	
	#Read image data in 2 dimenional array.
	my @ImageData;
	
	seek ($fh, $Info->{"DataStart"}, 0);
	
	my $YCounter = 0;
	while ($YCounter < $Info->{"Height"})
	{
		#Read an entire row of the image
		read ($fh, my $Buffer, ($Info->{"Width"} * $Info->{"BPP"}));
		
		my $XCounter = 0;
		
		for (split //, $Buffer)
		{
			my $Byte = $_;
			$ImageData[$YCounter][$XCounter] = unpack("C", $Byte);
			$XCounter++;
		}
		
		$YCounter++;
	}
	
	$Info->{"ImageData"} = \@ImageData;
}

sub ValidateIsNumeric
{
	my $Value = shift(@_);
	my $Type = shift(@_);
	
	if (not $Value =~ /^\d+?$/)
	{
		die "$Type is not a numeric value. $Type: '$Value'.\n";
	}
}

sub PrintUsage
{
	my $Error = shift(@_);
	
	if ($Error)
	{
		print "Invalid configuration: $Error\n";
	}
	print "### Usage instructions ###\n\n";
	
	print "To hide a file:\n";
	print "-h CoverImage\n";
		print "\tMandatory.\n";
		print "\tThe image that will contain the payload. Must be a binary PPM or PGM file.\n";
	print "-p Payload\n";
		print "\tMandatory.\n";
		print "\tThe payload that will be hidden in the cover image.\n";
	print "-o OutputFile\n";
		print "\tOptional.\n";
		print "\tThe name of the output steg image.\n";
	print "Example:\n";
	print "\tStegTool.pl -h Cover.ppm -p HideMe.txt -o StegImage.ppm\n\n";
	
	print "To recover a file:\n";
	print "-e StegImage\n";
		print "\tMandatory.\n";
		print "\tThe name of the steg image that the payload will be extracted from.\n";
	print "-z PayloadSize\n";
		print "\tMandatory\n";
		print "\tThe size of the payload to extract.\n";
	print "-o OutputFile\n";
		print "\tOptional.\n";
		print "\tThe name of the extracted payload file.\n";
	print "Example:\n";
	print "\tStegTool.pl -e StegImage.ppm -z 1234 -o Extracted.zip\n";
	
	exit;
}