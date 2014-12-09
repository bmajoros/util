#!/usr/bin/perl
use strict;

my $S=3;
my $M=20;
my $N=100;
my $MAX_K=$N;

my $mean=0;
my $sum=0;
for(my $K=0 ; $K<$MAX_K ; ++$K) {
  my $P=0+`beta-binomial $K $N $S $M`;
  $sum+=$P;
  $mean+=$P*$K;
  #print "$K\t$P\n";
}
print STDERR "$sum\t$mean\n";
print "\n";

$mean=0;
$sum=0;
for(my $K=0 ; $K<$MAX_K ; ++$K) {
  my $P=0+`beta-binomial -b $K $N $S $M`;
  $sum+=$P;
  $mean+=$P*$K;
  #print "$K\t$P\n";
  for(my $i=0 ; $i<$K*$N ; ++$i) { print "$P\n" }
}
print STDERR "$sum\t$mean\n";
