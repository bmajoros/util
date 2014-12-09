#!/usr/bin/perl
use strict;
use ProgramName;
use TempFilename;

my $name=ProgramName::get();
die "$name <*.kde> <#bins>\n" unless @ARGV==2;
my ($infile,$numBins)=@ARGV;

my $tempFile=TempFilename::generate();

open(OUT,">$tempFile") || die $tempFile;
open(IN,$infile) || die $infile;
<IN>;<IN>;
while(<IN>) {
  chomp;
  my @fields=split;
  next unless @fields>=2;
  my ($x,$count)=@fields;
  for(my $i=0 ; $i<$count ; ++$i) { print OUT "$x\n" }
}
close(IN);
close(OUT);
system("cat $tempFile | histogram -l $numBins");
unlink($tempFile);


