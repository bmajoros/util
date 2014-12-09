#!/usr/bin/perl
use strict;
use TempFilename;
$|=1;

my $MEAN0=-4;
my $MEAN1=4;
my $SD=3;
my $TRAIN_N=5;
my $TEST_N=1000;
my $ROUND=0.01;
my $NUM_BINS=30;
my $EPSILON=0.0000001; # to avoid division by zero

# Generate training data
my $train0=TempFilename::generate();
my $train1=TempFilename::generate();
system("sample-gaussian -- $MEAN0 $SD $TRAIN_N > $train0");
system("sample-gaussian -- $MEAN1 $SD $TRAIN_N > $train1");

# Train models
my $kde0=TempFilename::generate();
my $kde1=TempFilename::generate();
my $hist0=TempFilename::generate();
my $hist1=TempFilename::generate();
system("train-kde $train0 $ROUND $kde0");
system("train-kde $train1 $ROUND $kde1");
system("train-histogram -b $NUM_BINS $train0 $hist0");
system("train-histogram -b $NUM_BINS $train1 $hist1");

# Generate test data
my $test0=TempFilename::generate();
my $test1=TempFilename::generate();
system("sample-gaussian -- $MEAN0 $SD $TEST_N > $test0");
system("sample-gaussian -- $MEAN1 $SD $TEST_N > $test1");

# Make predictions
my ($kdeScores0,$histScores0)=predict($test0,0);
my ($kdeScores1,$histScores1)=predict($test1,1);
my (@kdeScores,@histScores);
push @kdeScores,@$kdeScores0; push @kdeScores,@$kdeScores1;
push @histScores,@$histScores0; push @histScores,@$histScores1;
randomize(\@kdeScores); randomize(\@histScores);
@kdeScores=sort {$a->[0] <=> $b->[0]} @kdeScores;
@histScores=sort {$a->[0] <=> $b->[0]} @histScores;

# Compute AUC
my $kdeRoc=roc(\@kdeScores);
my $histRoc=roc(\@histScores);
my $kdeAuc=auc($kdeRoc);
my $histAuc=auc($histRoc);
print "KDE=$kdeAuc\thistogram=$histAuc\n";

# Save curves
open(KDE,">kde.roc"); open(HIST,">hist.roc");
foreach my $p (@$kdeRoc) { my ($f,$t)=@$p; print KDE "$f\t$t\n" }
foreach my $p (@$histRoc) { my ($f,$t)=@$p; print HIST "$f\t$t\n" }
close(KDE); close(HIST);

# Clean up
unlink($train0); unlink($train1);
unlink($kde0);   unlink($kde1);
unlink($hist0);  unlink($hist1);
unlink($test0);  unlink($test1);

# =======================================================================
# SUBROUTINES

sub predict {
  my ($filename,$label)=@_;
  my $kdeScores=[]; my $histScores=[];
  open(IN,$filename) || die $filename;
  while(<IN>) {
    chomp;
    my $x=0+$_;
    my $kdeR= $EPSILON+`kde-tail $kde0 RIGHT -v $x`;
    #print "kde-tail $kde0 RIGHT -v $x\n";
    #print "$kdeR\n";
    my $kdeL= $EPSILON+`kde-tail $kde1 LEFT -v $x`;
    my $kdeScore=$kdeL/$kdeR;
    my $histR=$EPSILON+`histogram-tail $hist0 -- $x RIGHT`;
    my $histL=$EPSILON+`histogram-tail $hist1 -- $x LEFT`;
    my $histScore=$histL/$histR;
    push @$kdeScores,[$kdeScore,$label];
    push @$histScores,[$histScore,$label];
    #print "$kdeL $kdeR $kdeScore $histL $histR $histScore\n";
  }
  close(IN);
  return ($kdeScores,$histScores);
}

sub roc {
  my ($array)=@_;
  my $curve=[];
  my $POS=0; my $NEG=0;
  foreach my $rec (@$array) { if($rec->[1]){++$POS}else{++$NEG}}
  #my %seen;
  my $TP=$POS; my $FP=$NEG; my $TN=0; my $FN=0;
  my $N=@$array;
  for(my $i=0 ; $i<$N ; ++$i) {
    my $rec=$array->[$i];
    my ($score,$class)=@$rec;
    if($class==1) { --$TP; ++$FN }
    else { --$FP; ++$TN }
    my $TPR=$TP/($TP+$FN);  my $FPR=$FP/($FP+$TN);
    #$TPR=int($TPR*100+5/9)/100;
    #$FPR=int($FPR*100+5/9)/100;
    #my $line="$FPR\t$TPR\n";
    #next if $seen{$line};
    #$seen{$line}=1;
    unshift @$curve,[$FPR,$TPR];
  }
  return $curve;
}

sub auc {
  my ($roc)=@_;
  my $N=@$roc;
  my $auc=0;
  for(my $i=0 ; $i<$N-1 ; ++$i) {
    my $this=$roc->[$i]; my $next=$roc->[$i+1];
    my ($x1,$y1)=@$this; my ($x2,$y2)=@$next;
    my $dx=$x2-$x1;
    my $area=$dx*($y1+$y2)/2;
    $auc+=$area;
    #print "$x1\t$y1\n";
  }
  #print "AUC=$auc\n";
  #exit(0); ####
  $auc=int($auc*1000+5/9)/1000;
  return $auc;
}

sub randomize {
  my ($A)=@_;
  my $n=@$A;
  for(my $i=0 ; $i<$n-1 ; ++$i) {
    my $j=$i+int(rand($n-$i));
    my $a=$A->[$i]; my $b=$A->[$j];
    $A->[$i]=$b;
    $A->[$j]=$a;
  }
}

