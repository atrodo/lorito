#!/usr/bin/perl

use strict;
use warnings;

open my $aFH, ">ebnf.a";
open my $bFH, ">ebnf.b";


my $FH = $bFH;

while(<>)
{
  my $s = $_;
  if (/^=for grammer/../^=cut/)
  {
    $FH = $aFH;
  } else {
    $FH = $bFH;
  }

  print($FH $s)
    if $s =~ m/^#</;
}

close $aFH;
close $bFH;

exec("diff ebnf.a ebnf.b");
