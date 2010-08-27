#!/usr/bin/perl

use strict;
use warnings;
use autodie;

use Data::Dumper;

# Verify core.c, lasm.pl, ops_skel.pod and microcode.h
#  and generate ops.pod

my $core = {};
my $lasm = {};
my $skel = {};
my $micr = {};

my $prefix = "";
my $suffix = "";

# core.c
{
  my $curFH;
  my $current;
  my $op_cnt = 0;
  my $switch_indent = "";

  open $curFH, "<", "core.c";
  while (my $line = <$curFH>)
  {
    if ($line =~ m[// \s+ BEGIN \s+ CORE_OPS]xms .. $line =~ m[// \s+ END \s+ CORE_OPS]xms )
    {
      if ($line =~ m[^ (\s+) switch \s* \( regtype \)]xms .. $line =~ m[^ $switch_indent break;]xms)
      {
        if (defined $1)
        {
          $switch_indent = qr/$1/;
        }

        if ($line =~ m/case \s+ OP_(INT|NUM|STR|PMC) \s* :/xms)
        {
          $core->{$current}->{$1} = $op_cnt++;
        }
      }
      elsif ($line =~ m/case \s+ OP_(\w+) \s* :/xms)
      {
        $current = $1;
        $core->{$current} = {};
        $core->{$current}->{cnt} = $op_cnt++;
      }
      else
      {
        $switch_indent = ""
      }
    }
  }

  print Dumper($core);
}

# lasm.pl
{
  my $curFH;
  my $current;

  open $curFH, "<", "lasm.pl";
  while (my $line = <$curFH>)
  {
    if ($line =~ m[\# \s+ BEGIN \s+ OPS_ENUM]xms .. $line =~ m[\# \s+ END \s+ OPS_ENUM]xms )
    {
      if ($line =~ m/'?(\w+)'? \s* => \s* (\d+),/xms)
      {
        $lasm->{$1} = $2;
      }
    }
  }

  print Dumper($lasm)
}

# ops_skel.pod
{
  my $curFH;
  my $current;
  my $in_prefix = 1;
  my $group = "";

  open $curFH, "<", "ops_skel.pod";
  while (my $line = <$curFH>)
  {
    if ($line =~ m[^=begin \s+ verify_begin]xms .. $line =~ m[^=end \s+ verify_end]xms )
    {
      $DB::single = 1;
      $in_prefix = 0;
      if ($line =~ m[^=head2 \s+ (.+)$ ]xms)
      {
        $group = $1;
      }
      if ($line =~ m[^=head3 \s+ (\w+)]xms)
      {
        $current =
        {
          group => $group,
        };
        $skel->{$1} = $current;
      }
      if ($line =~ m[^=item \s+ Registers]xms .. $line =~ m[^=(item|back) \s+]xms)
      {
        $current->{reg} ||= "";
        $current->{reg} .= $line;
      }
      if ($line =~ m[^=item \s+ Description]xms .. $line =~ m[^=(item|back) \s+]xms)
      {
        $current->{desc} ||= "";
        $current->{desc} .= $line;
      }
    }
    else
    {
      $prefix .= $line if $in_prefix == 1;
      $suffix .= $line if $in_prefix == 0;
    }
  }

  print Dumper($skel)
}

# micrcode.h
{
  my $curFH;
  my $current;

  open $curFH, "<", "microcode.h";
  while (my $line = <$curFH>)
  {
    if ($line =~ m[// \s+ BEGIN \s+ OPS_ENUM]xms .. $line =~ m[// \s+ END \s+ OPS_ENUM]xms )
    {
      if ($line =~ m/OP_ (\w+) \s* = \s* (\d+)/xms)
      {
        $micr->{$1} = $2;
      }
    }
  }

#  print Dumper($micr)
}

# Compare everything to microcode.h, make sure it's all legit.
my @ops = sort { $micr->{$a} <=> $micr->{$b} } keys %$micr;

print Dumper(\@ops);

foreach my $op (@ops)
{
  warn sprintf("%-10s does not exist in lasm.pl\n", $op)
    if !exists $lasm->{$op};

  warn sprintf("%-10s does not exist in ops_skel.pod\n", $op)
    if !exists $skel->{$op};

  warn sprintf("%-10s does not exist in core.c\n", $op)
    if !exists $core->{$op};

  warn sprintf("%-10s is not in order in core.c\n", $op)
    if ($core->{$op}->{cnt} || -1) != $micr->{$op};

  warn sprintf("%-10s does not have a register section in ops_skel.pod\n", $op)
    if !exists $skel->{$op}->{reg};

  warn sprintf("%-10s does not have a description section in ops_skel.pod\n", $op)
    if !exists $skel->{$op}->{desc};
}

# Generate ops.pod
{
  my $ouFH;
  my $group = "";

  open $ouFH, ">", "ops.pod";

  select $ouFH;

  print $prefix;

  foreach my $op (@ops)
  {
    if ($skel->{$op}->{group} ne $group)
    {
      $group = $skel->{$op}->{group};
      print "=head2 $group\n\n";
    }
    print "=head3 $op\n\n";
    print "=over\n\n";

    print "=item Valid Forms:\n\n";
    my $seen_any = 0;
    for my $item (qw/PMC STR NUM INT/)
    {
      if (exists $core->{$op}->{$item})
      {
        $seen_any = 1;
        print "$item\n";
      }
    }
    print "Any\n" if $seen_any == 0;
    print "\n";

    print $skel->{$op}->{reg} || "=item Registers\n\nNone\n";
    print "\n";

    print $skel->{$op}->{desc} || "=item Description\n";
    print "\n";

    print "=back\n\n";
  }

  print $suffix;
}
