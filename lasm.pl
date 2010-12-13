#!/usr/bin/perl

use strict;
use warnings;
use POSIX qw/ceil/;

=for grammar

#<goal> -> ( <any_code_blk> | <const_blk> | <struct_blk> )*
#<any_code_blk> -> ( <code_blk> | <init_blk> | <main_blk> ) <str> <stmt>* .end;
#<code_blk> -> .sub
#<init_blk> -> .init
#<main_blk> -> .main
#<const_blk> -> .const <str> <const_stmt>* .end;
#<struct_blk> -> ( .struct | .data ) <str> <struct_stmt>* .end;
#<stmt> -> <label>? <dest>? <regtype>? <opcode> ( <lhs> ( ,?  | , <rhs> ) )? ( : ( <imm> | <offset> | <jmp> ) )? ;
#<const_stmt> -> <label>? <const> ;
#<struct_stmt> -> <label>? size <int> ;
#<label> -> <id>? :
#<dest>  -> <reg> =
#<regtype> -> ( INT | NUM | STR | PMC )
#<opcode> -> [ @OPS ]
#<lhs> -> <reg>
#<rhs> -> <reg>
#<imm> -> <int>
#<jmp> -> <id>
#<offset> -> ( <str> | <id> )? [ <id> ( [+-] <int> )? ]
#<const> -> ( <num> | <int> | <str> )

#<str> ~> m/(?:') ([^']*(?:(?:\')[^']*)*) (?:')/xms
#<str> ~> m/(?:") ([^"]*(?:(?:\")[^"]*)*) (?:")/xms
#<int> ~> m/(\d+)/xms
#<num> ~> m/( [+-]? \d+ (?: [.] \d+)? (?: [+-]? [e] \d+)? )/xms
#<id>  ~> m/( [[:alpha:]] \w* )/xms
#<reg> ~> m/[$] [I | N | S | P]? ([\d]+)/xms

=cut

# Load>    :r microcode.h
# Convert> :'<,'>!perl -ne 'if (m/BEGIN OPS_ENUM/..m/END OPS_ENUM/) { s[//][\#]; s/^\s*OP_/  /; s/=/=>/; print $_ }'
my %ops = (
  # BEGIN OPS_ENUM

  # Execution Ops
  noop       => 0,
  end        => 1,

  # Math Ops
  add        => 2,
  'sub'      => 3,
  mul        => 4,
  div        => 5,
  mod        => 6,
  and        => 7,
  or         => 8,
  xor        => 9,
  not        => 10,
  shl        => 11,
  shr        => 12,

  # Comparison Ops
  iseq       => 13,
  isgt       => 14,
  isge       => 15,

  # Flow Control Ops
  goto       => 16,
  if         => 17,

  # Register Manipulation Ops
  set        => 18,
  load_const => 19,
  load_imm   => 20,
  coerce_int => 21,
  coerce_num => 22,
  coerce_str => 23,

  # PMC Ops
  new        => 24,
  store      => 25,
  load       => 26,
  push_arg   => 27,
  pop_arg    => 28,
  cnt_arg    => 42,

  call       => 29,

  push_ret   => 39,
  pop_ret    => 40,
  cnt_ret    => 41,

  lookup     => 35,
  ctx        => 36,
  new_ctx    => 38,

  block      => 37,

  # Environment Ops
  loadlib    => 30,
  read       => 31,
  write      => 32,
  say        => 33,
  gripe      => 34,

  # These Ops
  hcf        => 63
  # END OPS_ENUM
); 

my %op_types = (

  PMC => 0,
  STR => 1,
  NUM => 2,
  INT => 3

);

my %flags = (
  INIT => 0x01,
  MAIN => 0x02,
);

my %seg_types = (
  CODE    => 0,
  CONST   => 1,
  DATADEF => 2,
);

# Start of pattern.  Items common to all parsing patterns, including ignoring
#  comments
my $som = qr/\G (?: \s* [#] [^\n]* $)* \s* /xms;

# Set the max position the match has gotten to and reset the position
my $max = 0;

sub max
{
  my $str = shift;
  my $last_pos = shift;

  $max = (pos($$str) || 0) > $max ? pos($$str) : $max;
  pos($$str) = $last_pos;
  #warn "$max\n";
}

#<goal> -> ( <any_code_blk> | <const_blk> | <struct_blk> )*
sub goal
{
  my $str = shift;
  my $pos = pos $$str;
  my $result  = [];
  
  $max = 0;

  #while (my $code = &code($str))
  while (pos($$str) || 0 != length($$str))
  {
    my $code_blk = &any_code_blk($str);
    if (defined $code_blk)
    {
      push @$result, $code_blk;
      next;
    }
    my $const_blk = &const_blk($str);
    if (defined $const_blk)
    {
      push @$result, $const_blk;
      next;
    }
    my $struct_blk = &struct_blk($str);
    if (defined $struct_blk)
    {
      push @$result, $struct_blk;
      next;
    }
    last;
  }

  $$str =~ m/$som /ixmsgc;

  if (pos($$str) != length($$str))
  {
    max($str, $pos);
    return;
  }

  return $result;
}

#<any_code_blk> -> ( <code_blk> | <init_blk> | <main_blk> ) <str> <stmt>* .end;
sub any_code_blk
{
  my $str = shift;
  my $pos = pos $$str;
  my $result  = {};

  for my $blk (\&code_blk, \&init_blk, \&main_blk)
  {
    my $flags = $blk->($str);
    if (defined $flags)
    {
      $result = {%$flags};
      last;
    }
  }

  $result->{typed} = "code";

  my $named = &str($str);
  if (!defined $named)
  {
    max($str, $pos);
    return;
  }
  $result->{named} = $named;

  $result->{stmts} = [];
  while (my $stmt = &stmt($str))
  {
    push @{$result->{stmts}}, $stmt;
  }

  if ($$str !~ m/$som [.]end; /ixmsgc)
  {
    max($str, $pos);
    return;
  }

  return $result;
}

#<code_blk> -> .sub
sub code_blk
{
  my $str = shift;
  my $pos = pos $$str;

  if ($$str !~ m/$som [.]sub /ixmsgc)
  {
    max($str, $pos);
    return;
  }
  
  return { flags => 0 };
}

#<init_blk> -> .init
sub init_blk
{
  my $str = shift;
  my $pos = pos $$str;

  if ($$str !~ m/$som [.]init /ixmsgc)
  {
    max($str, $pos);
    return;
  }
  
  return { flags => $flags{INIT} };
}

#<main_blk> -> .main
sub main_blk
{
  my $str = shift;
  my $pos = pos $$str;

  if ($$str !~ m/$som [.]main /ixmsgc)
  {
    max($str, $pos);
    return;
  }
  
  return { flags => $flags{MAIN} };
}


#<const_blk> -> .const <str> <const_stmt>* .end;
sub const_blk
{
  my $str = shift;
  my $pos = pos $$str;
  my $result  = {};

  if ($$str !~ m/$som [.] const /ixmsgc)
  {
    max($str, $pos);
    return;
  }

  $result->{typed} = "const";

  my $named = &str($str);
  if (!defined $named)
  {
    max($str, $pos);
    return;
  }
  $result->{named} = $named;

  $result->{consts} = [];
  while (my $const_stmt = &const_stmt($str))
  {
    push @{$result->{consts}}, $const_stmt;
  }

  if ($$str !~ m/$som [.]end; /ixmsgc)
  {
    max($str, $pos);
    return;
  }

  return $result;
}

#<struct_blk> -> ( .struct | .data ) <str> <struct_stmt>* .end;
sub struct_blk
{
  my $str = shift;
  my $pos = pos $$str;
  my $result  = {};

  $result->{typed} = "struct";

  if ($$str !~ m/$som [.] struct /ixmsgc)
  {
    if ($$str !~ m/$som [.] data /ixmsgc)
    {
      max($str, $pos);
      return;
    }
    $result->{typed} = "data";
  }

  my $named = &str($str);
  if (!defined $named)
  {
    max($str, $pos);
    return;
  }
  $result->{named} = $named;

  $result->{consts} = [];
  while (my $struct_stmt = &struct_stmt($str))
  {
    push @{$result->{consts}}, $struct_stmt;
  }

  if ($$str !~ m/$som [.]end; /ixmsgc)
  {
    max($str, $pos);
    return;
  }

  return $result;
}

#<stmt> -> <label>? <dest>? <regtype>? <opcode> ( <lhs> ( ,?  | , <rhs> ) )? ( : ( <imm> | <offset> | <jmp> ) )? ;
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
    max($str, $pos);
    return;
  }

  my $lhs = lhs($str);
  if (defined $lhs)
  {
    $result->{src1} = $lhs;
    if ($$str =~ m/$som , /ixmsgc)
    {
      $result->{src2} = rhs($str);
    }
  }

  if ($$str =~ m/$som : /ixmsgc)
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
      my $offset = offset($str);
      if (defined $offset)
      {
        $found = 1;
        $result->{offset} = $offset;
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
  }

  if ($$str !~ m/$som ; /ixmsgc)
  {
    max($str, $pos);
    return;
  }

  return $result;
}

#<const_stmt> -> <label>? <const> ;
sub const_stmt
{
  my $str = shift;
  my $pos = pos $$str;
  my $result = {};

  $result->{label} = label($str);

  my ($type, $const) = const($str);
  if (!defined $const)
  {
    max($str, $pos);
    return;
  }

  $result->{type} = $type;
  $result->{const} = $const;

  if ($$str !~ m/$som ; /ixmsgc)
  {
    max($str, $pos);
    return;
  }

  return $result;
}

#<struct_stmt> -> <label>? size <int> ;
sub struct_stmt
{
  my $str = shift;
  my $pos = pos $$str;
  my $result = {};

  $result->{label} = label($str);

  if ($$str !~ m/$som size /ixmsgc)
  {
    max($str, $pos);
    return;
  }

  my $size = &int($str);
  if (!defined $size)
  {
    max($str, $pos);
    return;
  }

  $result->{size} = $size;

  if ($$str !~ m/$som ; /ixmsgc)
  {
    max($str, $pos);
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

  if ($$str !~ m/$som : /ixmsgc)
  {
    max($str, $pos);
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
    max($str, $pos);
    return;
  }

  if ($$str !~ m/$som = /ixmsgc)
  {
    max($str, $pos);
    return;
  }

  return $reg;
}

#<regtype> -> ( INT | NUM | STR | PMC )
sub regtype
{
  my $str = shift;
  my $pos = pos $$str;

  my ($result) = $$str =~ m/$som (INT | NUM | STR | PMC)/ixmsgc;
  return $result;
}

#<opcode> -> [ @OPS ]
sub opcode
{
  my $str = shift;
  my $pos = pos $$str;

  my ($result) = $$str =~ m/$som (\w*)/ixmsgc;
  
  if (!exists $ops{lc($result)})
  {
    max($str, $pos);
    return;
  }
  return lc $result;
}

#<lhs> -> <reg>
sub lhs
{
  my $str = shift;
  my $pos = pos $$str;

  my $reg = reg($str);
  if (!defined $reg)
  {
    max($str, $pos);
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
    max($str, $pos);
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
    max($str, $pos);
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
    max($str, $pos);
    return;
  }

  return $reg;
}

#<offset> -> ( <str> | <id> )? [ <id> ( [+-] <int> )? ]
sub offset
{
  my $str = shift;
  my $pos = pos $$str;
  my $result = {};

  my $block = str($str);
  if (!defined $block)
  {
    $block = id($str);
  }
  $result->{block} = $block;

  if ($$str !~ m/$som [[] /ixmsgc)
  {
    max($str, $pos);
    return;
  }

  my $id = id($str);

  if (!defined $id)
  {
    max($str, $pos);
    return;
  }
  $result->{id} = $id;

  if ($$str =~ m/$som ([+-]) /ixmsgc)
  {
    my $sign = $1;
    my $int = &int($str);
    if (!defined $int)
    {
      max($str, $pos);
      return;
    }
    $result->{offset} = $sign.$int;
  }

  if ($$str !~ m/$som []] /ixmsgc)
  {
    max($str, $pos);
    return;
  }

  return $result;
}

#<const> -> ( <num> | <int> | <str> )
sub const
{
  my $str = shift;
  my $pos = pos $$str;
  my $result;

  $result = num($str);
  return (num => $result)
    if defined $result;

  $result = &int($str);
  return ('int' => $result)
    if defined $result;

  $result = str($str);
  return (str => $result)
    if defined $result;

  return;
}

#<str> ~> m/(?:') ([^']*(?:(?:\')[^']*)*) (?:')/xms
#<str> ~> m/(?:") ([^"]*(?:(?:\")[^"]*)*) (?:")/xms
sub str
{
  my $str = shift;
  my $pos = pos $$str;

  my $result;

  ($result) = $$str =~ m/$som (?:') ([^']*(?:(?:\\')[^']*)*) (?:')/ixmsgc;
  return $result
    if defined $result;

  ($result) = $$str =~ m/$som (?:") ([^"]*(?:(?:\\")[^"]*)*) (?:")/ixmsgc;
  return $result;
}

#<int> ~> m/(\d+)/xms
sub int
{
  my $str = shift;
  my $pos = pos $$str;

  my ($result) = $$str =~ m/$som (\d+)/ixmsgc;
  return $result;
}

# The num re is not exactly correct, since we also match ints, but this is
#  easier to read and understand for the grammar
#<num> ~> m/( [+-]? \d+ (?: [.] \d+)? (?: [+-]? [e] \d+)? )/xms
sub num
{
  my $str = shift;
  my $pos = pos $$str;

  my ($result) = $$str =~ m/$som ( 
      [+-]? \d+ 
      (?: 
        [.] \d+ 
        |
        [+-]? [e] \d+
        |
        [.] \d+ [+-]? [e] \d+
      )
    ) /ixmsgc;
  return $result;
}

#<id>  ~> m/( [[:alpha:]] \w* )/xms
sub id
{
  my $str = shift;
  my $pos = pos $$str;

  my ($result) = $$str =~ m/$som ( [[:alpha:]] \w* )/ixmsgc;
  return $result;
}
#<reg> ~> m/[$] [I | N | S | P]? ([\d]+)/xms
sub reg
{
  my $str = shift;
  my $pos = pos $$str;

  my ($result) = $$str =~ m/$som \$ [I N S P]? ([\d]+)/ixmsgc;
  return $result;
}

# Given an op hash, turn it into binary
# const MUST be decided from imm|jmp|offset before calling this.
sub gen_op
{
  my $stmt = shift;

  # Compose the opcode
  my $op = $ops{$stmt->{opcode}};
  die "Could not load opcode: ".$stmt->{opcode}
    unless defined $op;
  return pack("CCCCI",
    ($op % (2**6-1)) | ( ($op_types{$stmt->{regtype} || "PMC"}) << 6 ),
    ($stmt->{dest} || 0)+0,
    ($stmt->{src1} || 0)+0,
    ($stmt->{src2} || 0)+0,
    ($stmt->{const} || 0)+0,
  );
}

# Go!

my $file = do { local $/; <> };

# Parse the file
my $ast = goal(\$file);

if (!defined $ast)
{
  warn "Could not parse lasm file\n";
  my $to_max = substr($file, 0, $max);
  my @lines = $to_max =~ m/(\n)/g;

  $file =~ s/\s/ /xmsg;
  warn "At line:  ".scalar(@lines)."\n";
  warn "Error near:\n\t".sprintf("%s[%s]%s\n", substr($file, $max-10, 10), substr($file, $max, 1), substr($file, $max+1, 10) );
  exit;
}

use Data::Dumper;
#warn Dumper($ast);

# Generate the output

# Generate all the block labels
my %block;
my %block_cache;

foreach my $seg (@$ast)
{
  if ($seg->{typed} eq "struct")
  {
    $block{$seg->{named}} = {};
    my $current = $block{$seg->{named}};
    my $curr_size = 0;
    foreach my $const (@{$seg->{consts}})
    {
      if (defined $const->{label})
      {
        $current->{$const->{label}} = $curr_size;
      }
      $curr_size += $const->{size};
    }
  }
  if ($seg->{typed} eq "data")
  {
    $block{$seg->{named}} = {};
    my $current = $block{$seg->{named}};
    my $curr_size = 0;
    foreach my $const (@{$seg->{consts}})
    {
      if (defined $const->{label})
      {
        $current->{$const->{label}} = $curr_size;
      }
      $curr_size += $const->{size};
    }
    my $output = "";
    $output .= pack("I", $seg_types{DATADEF});
    $output .= pack("I", $seg->{flags} || 0);
    $output .= pack("IZ*", length($seg->{named})+1, $seg->{named});
    my $len = ceil(length($curr_size) / 8);
    $output .= pack("I", $curr_size);
    print $output;
  }
  if ($seg->{typed} eq "const")
  {
    $block{$seg->{named}} = {};
    my $current = $block{$seg->{named}};
    my $block_output = "";
    foreach my $const (@{$seg->{consts}})
    {
      if (defined $const->{label})
      {
        $current->{$const->{label}} = length($block_output);
      }
      if ($const->{type} eq "str")
      {
        $block_output .= pack("U0Z*", $const->{const});
      } elsif ($const->{type} eq "int") {
        if ($const->{const} < 0)
        {
          $block_output .= pack("I", $const->{const});
        } else {
          $block_output .= pack("i", $const->{const});
        }
      } elsif ($const->{type} eq "num") {
        $block_output .= pack("d", $const->{const});
      }
    }
    if (length $block_output > 0)
    {
      my $output = "";
      $output .= pack("I", $seg_types{CONST});
      $output .= pack("I", $seg->{flags} || 0);
      $output .= pack("IZ*", length($seg->{named})+1, $seg->{named});
      my $len = ceil(length($block_output) / 8);
      $output .= pack("I", $len);
      $output .= $block_output;
      $output .= pack("a".($len * 8 - length($block_output)), "");
      print $output;
    }
  }
}

# Generate all the output for the segments
foreach my $seg (@$ast)
{
  my $output = "";
  if ($seg->{typed} eq "code")
  {
    $output .= pack("I", $seg_types{CODE});
    $output .= pack("I", $seg->{flags} || 0);
    $output .= pack("IZ*", length($seg->{named})+1, $seg->{named});
    $output .= pack("I", scalar(@{$seg->{stmts}}));

    my %labels;
    # Find all the labels first
    foreach my $i (0..scalar(@{$seg->{stmts}})-1)
    {
      my $label = $seg->{stmts}->[$i]->{label};
      $labels{$label} = $i
        if defined $label and $label ne "";
    }

    # Generate the code
    foreach my $stmt (@{$seg->{stmts}})
    {
      # Decide const
      # <imm> | <jmp> | <offset>
      if (exists $stmt->{imm})
      {
        $stmt->{const} = $stmt->{imm}+0;
      } elsif (exists $stmt->{jmp}) {
        $stmt->{const} = $labels{$stmt->{jmp}}+0;
      } elsif (exists $stmt->{offset}) {
        #die "offsets are awkward right now, sorry";
        my $ref_block;
        if (defined $stmt->{offset}->{block})
        {
          $ref_block = $block{$stmt->{offset}->{block}};
        }
        elsif (defined $block{$seg->{named}})
        {
          $ref_block = $block{$seg->{named}};
        } else {
          $ref_block = $block{''};
        }
        die "Could not find a usable block name"
          if !defined $ref_block;

        my $offset = $ref_block->{$stmt->{offset}->{id}};
        die "Could not find a usable id in block"
          if !defined $offset;
        $stmt->{const} = $offset + ($stmt->{offset}->{offset} || 0);
      } else {
        $stmt->{const} = 0;
      }

      $output .= gen_op($stmt);
    }
  }
  elsif ($seg->{typed} eq "const")
  {
    #$output .= pack("I", 1);
    #$output .= pack("IZ*", length($seg->{named})+1, $seg->{named});
    # ...
  } else {
    die "Could create segment of type ".$seg->{typed};
  }

  print $output;

}
