#!/usr/bin/perl

use strict;
use warnings;

#<goal> -> <seg>*
#<seg> -> sub <str> <stmt>* end;
#<stmt> -> <label>? <dest>? <regtype>? <opcode> ( <lhs> ( ,?  | , <rhs> ) )? ( : ( <imm> | <jmp> | <offset> ) )? ;
#<label> -> <id>? :
#<dest>  -> <reg> =
#<regtype> -> ( INT | NUM | STR | PMC )
#<opcode> -> [ @OPS ]
#<lhs> -> <reg>
#<rhs> -> <reg>
#<imm> -> <int>
#<jmp> -> <id>
#<offset> -> ( <id> | <int> )

#<str> ~> m/(?:') ([^']*(?:(?:\')[^']*)*) (?:')/xms
#<int> ~> m/(\d+)/xms
#<id>  ~> m/( [[:alpha:]] \w* )/xms
#<reg> ~> m/[$] [I | N | S | P]? ([\d]+)/xms

# Load>    :r microcode.h
# Convert> :'<,'>!perl -ne 'if (m/BEGIN OPS_ENUM/..m/END OPS_ENUM/) { s[//.*][]; s/^\s*OP_//; s/=/=>/; print $_ }'
my %ops = (
  noop       => 0,
  end        => 1,
  add        => 2,
  sub        => 3,
  mul        => 4,
  div        => 5,
  mod        => 6,
  not        => 7,
  mov        => 8,
  set        => 9,
  goto       => 10,
  if         => 11,
  iseq       => 12,
  isgt       => 13,
  isge       => 14,
  and        => 15,
  or         => 16,
  xor        => 17,
  new        => 18,
  call       => 19,
  loadbc     => 20,
  read       => 21,
  write      => 22,
  gripe      => 23,

  hcf        => 31
); 

#<goal> -> <seg>*
sub goal
{
  my $str = shift;
  my $pos = pos $$str;
  my $result  = [];

  while (my $seg = &seg($str))
  {
    push @$result, $seg;
  }

  $$str =~ m/\G \s* /ixmsgc;

  if (pos($$str) != length($$str))
  {
    pos($$str) = $pos;
    return;
  }

  return $result;
}

#<seg> -> sub <str> <stmt>* end;
sub seg
{
  my $str = shift;
  my $pos = pos $$str;
  my $result  = {};

  if ($$str !~ m/\G \s* [.]sub /ixmsgc)
  {
    pos($$str) = $pos;
    return;
  }

  my $named = &str($str);
  if (!defined $named)
  {
    pos($$str) = $pos;
    return;
  }
  $result->{named} = $named;

  $result->{stmts} = [];
  while (my $stmt = &stmt($str))
  {
    push @{$result->{stmts}}, $stmt;
  }

  if ($$str !~ m/\G \s* [.]end; /ixmsgc)
  {
    pos($$str) = $pos;
    return;
  }

  return $result;
}

#<stmt> -> <label>? <dest>? <regtype>? <opcode> ( <lhs> ( ,?  | , <rhs> ) )? ( : ( <imm> | <jmp> | <offset> ) )? ;
sub stmt
{
  my $str = shift;
  my $pos = pos $$str;
  my $result  = {};

  $result->{label}   = label($str);
  $result->{dest}    = dest ($str);
  $result->{regtype} = regtype($str);

  $result->{opcode} = opcode($str);
  if (!defined $result->{opcode})
  {
    pos($$str) = $pos;
    return;
  }

  my $lhs = lhs($str);
  if (defined $lhs)
  {
    $result->{lhs} = $lhs;
    if ($$str =~ m/\G \s* , /ixmsgc)
    {
      $result->{rhs} = rhs($str);
    }
  }

  if ($$str =~ m/\G \s* : /ixmsgc)
  {
    my $found = 0;
    if ($found == 0)
    {
      my $imm = imm($str);
      if (defined $imm)
      {
        $found = 1;
        $result->{imm} = $imm;
      }
    }
    if ($found == 0)
    {
      my $jmp = jmp($str);
      if (defined $jmp)
      {
        $found = 1;
        $result->{jmp} = $jmp;
      }
    }
    if ($found == 0)
    {
      my $offset = offset($str);
      if (defined $offset)
      {
        $found = 1;
        $result->{offset} = $offset;
      }
    }
  }

  if ($$str !~ m/\G \s* ; /ixmsgc)
  {
    pos($$str) = $pos;
    return;
  }

  return $result;
}

#<label> -> <id>? :
sub label
{
  my $str = shift;
  my $pos = pos $$str;

  my $id = id($str);

  if ($$str !~ m/\G \s* : /ixmsgc)
  {
    pos($$str) = $pos;
    return;
  }

  return $id;
}

#<dest>  -> <reg> =
sub dest
{
  my $str = shift;
  my $pos = pos $$str;

  my $reg = reg($str);
  if (!defined $reg)
  {
    pos($$str) = $pos;
    return;
  }

  if ($$str !~ m/\G \s* = /ixmsgc)
  {
    pos($$str) = $pos;
    return;
  }

  return $reg;
}

#<regtype> -> ( INT | NUM | STR | PMC )
sub regtype
{
  my $str = shift;
  my $pos = pos $$str;

  my ($result) = $$str =~ m/\G \s* (INT | NUM | STR | PMC)/ixmsgc;
  return $result;
}

#<opcode> -> [ @OPS ]
sub opcode
{
  my $str = shift;
  my $pos = pos $$str;

  my ($result) = $$str =~ m/\G \s* (\w*)/ixmsgc;
  
  if (!exists $ops{lc($result)})
  {
    pos($$str) = $pos;
    return;
  }
  return $result;
}

#<lhs> -> <reg>
sub lhs
{
  my $str = shift;
  my $pos = pos $$str;

  my $reg = reg($str);
  if (!defined $reg)
  {
    pos($$str) = $pos;
    return;
  }

  return $reg;
}

#<rhs> -> <reg>
sub rhs
{
  my $str = shift;
  my $pos = pos $$str;

  my $reg = reg($str);
  if (!defined $reg)
  {
    pos($$str) = $pos;
    return;
  }

  return $reg;
}

#<imm> -> <int>
sub imm
{
  my $str = shift;
  my $pos = pos $$str;

  my $reg = &int($str);
  if (!defined $reg)
  {
    pos($$str) = $pos;
    return;
  }

  return $reg;
}

#<jmp> -> <id>
sub jmp
{
  my $str = shift;
  my $pos = pos $$str;

  my $reg = id($str);
  if (!defined $reg)
  {
    pos($$str) = $pos;
    return;
  }

  return $reg;
}

#<offset> -> ( <id> | <int> )

#<str> ~> m/(?:') ([^']*(?:(?:\')[^']*)*) (?:')/xms
sub str
{
  my $str = shift;
  my $pos = pos $$str;

  my ($result) = $$str =~ m/\G \s* (?:') ([^']*(?:(?:\\')[^']*)*) (?:')/ixmsgc;
  return $result;
}

#<int> ~> m/(\d+)/xms
sub int
{
  my $str = shift;
  my $pos = pos $$str;

  my ($result) = $$str =~ m/\G \s* (\d+)/ixmsgc;
  return $result;
}

#<id>  ~> m/( [[:alpha:]] \w* )/xms
sub id
{
  my $str = shift;
  my $pos = pos $$str;

  my ($result) = $$str =~ m/\G \s* ( [[:alpha:]] \w* )/ixmsgc;
  return $result;
}
#<reg> ~> m/[$] [I | N | S | P]? ([\d]+)/xms
sub reg
{
  my $str = shift;
  my $pos = pos $$str;

  my ($result) = $$str =~ m/\G \s* \$ [I N S P]? ([\d]+)/ixmsgc;
  return $result;
}

# Go!

my $file = do { local $/; <> };

$file =~ s/[#] [^\n]* $//xmsg;

use Data::Dumper;
print Dumper(goal(\$file));

