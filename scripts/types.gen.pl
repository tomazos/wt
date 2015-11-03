use strict;
use warnings;

print << 'EOH';
#pragma once

#include "experimental/sdl/types_base.h"

namespace gl {
EOH

my %configs = (
"vec2" => { "kind" => "vec", "len" => 2, "rows" => 2, "cols" => 1, "prim" => "float32" },
"vec3" => { "kind" => "vec", "len" => 3, "rows" => 3, "cols" => 1, "prim" => "float32" },
"vec4" => { "kind" => "vec", "len" => 4, "rows" => 4, "cols" => 1, "prim" => "float32" },
"dvec2" => { "kind" => "vec", "len" => 2, "rows" => 2, "cols" => 1, "prim" => "float64" },
"dvec3" => { "kind" => "vec", "len" => 3, "rows" => 3, "cols" => 1, "prim" => "float64" },
"dvec4" => { "kind" => "vec", "len" => 4, "rows" => 4, "cols" => 1, "prim" => "float64" },
"bvec2" => { "kind" => "vec", "len" => 2, "rows" => 2, "cols" => 1, "prim" => "bool" },
"bvec3" => { "kind" => "vec", "len" => 3, "rows" => 3, "cols" => 1, "prim" => "bool" },
"bvec4" => { "kind" => "vec", "len" => 4, "rows" => 4, "cols" => 1, "prim" => "bool" },
"ivec2" => { "kind" => "vec", "len" => 2, "rows" => 2, "cols" => 1, "prim" => "int32" },
"ivec3" => { "kind" => "vec", "len" => 3, "rows" => 3, "cols" => 1, "prim" => "int32" },
"ivec4" => { "kind" => "vec", "len" => 4, "rows" => 4, "cols" => 1, "prim" => "int32" },
"uvec2" => { "kind" => "vec", "len" => 2, "rows" => 2, "cols" => 1, "prim" => "uint32" },
"uvec3" => { "kind" => "vec", "len" => 3, "rows" => 3, "cols" => 1, "prim" => "uint32" },
"uvec4" => { "kind" => "vec", "len" => 4, "rows" => 4, "cols" => 1, "prim" => "uint32" },
"mat2x2" => { "kind" => "mat", "len" => 4, "rows" => 2, "cols" => 2, "prim" => "float32" },
"mat3x2" => { "kind" => "mat", "len" => 6, "rows" => 2, "cols" => 3, "prim" => "float32" },
"mat4x2" => { "kind" => "mat", "len" => 8, "rows" => 2, "cols" => 4, "prim" => "float32" },
"mat2x3" => { "kind" => "mat", "len" => 6, "rows" => 3, "cols" => 2, "prim" => "float32" },
"mat3x3" => { "kind" => "mat", "len" => 9, "rows" => 3, "cols" => 3, "prim" => "float32" },
"mat4x3" => { "kind" => "mat", "len" => 12, "rows" => 3, "cols" => 4, "prim" => "float32" },
"mat2x4" => { "kind" => "mat", "len" => 8, "rows" => 4, "cols" => 2, "prim" => "float32" },
"mat3x4" => { "kind" => "mat", "len" => 12, "rows" => 4, "cols" => 3, "prim" => "float32" },
"mat4x4" => { "kind" => "mat", "len" => 16, "rows" => 4, "cols" => 4, "prim" => "float32" },
"dmat2x2" => { "kind" => "mat", "len" => 4, "rows" => 2, "cols" => 2, "prim" => "float64" },
"dmat3x2" => { "kind" => "mat", "len" => 6, "rows" => 2, "cols" => 3, "prim" => "float64" },
"dmat4x2" => { "kind" => "mat", "len" => 8, "rows" => 2, "cols" => 4, "prim" => "float64" },
"dmat2x3" => { "kind" => "mat", "len" => 6, "rows" => 3, "cols" => 2, "prim" => "float64" },
"dmat3x3" => { "kind" => "mat", "len" => 9, "rows" => 3, "cols" => 3, "prim" => "float64" },
"dmat4x3" => { "kind" => "mat", "len" => 12, "rows" => 3, "cols" => 4, "prim" => "float64" },
"dmat2x4" => { "kind" => "mat", "len" => 8, "rows" => 4, "cols" => 2, "prim" => "float64" },
"dmat3x4" => { "kind" => "mat", "len" => 12, "rows" => 4, "cols" => 3, "prim" => "float64" },
"dmat4x4" => { "kind" => "mat", "len" => 16, "rows" => 4, "cols" => 4, "prim" => "float64" },
);


my %explicit_conversions = (
  "float32" => ["bool", "int32", "uint32"],
  "float64" => ["bool", "float32", "int32", "uint32"],
  "int32" => ["bool", "float32", "uint32"],
  "uint32" => ["bool", "float32", "int32"]
);

my %implicit_conversions = (
  "bool" => ["float32", "float64", "int32", "uint32"],
  "float32" => ["float64"],
  "int32" => ["float64"],
  "uint32" => ["float64"],
);

sub get_conv($$) {
  my ($from, $to) = @_;
  if (exists $explicit_conversions{$from}) {
    for my $x (@{$explicit_conversions{$from}}) {
      if ($x eq $to) {
        return "explicit";
      }
    }
  }

  if (exists $implicit_conversions{$from}) {
    for my $x (@{$implicit_conversions{$from}}) {
      if ($x eq $to) {
        return "implicit";
      }
    }
  }
  return "none";
}


sub get_binops($) {
  my ($prim) = @_;
  if ($prim eq "bool") {
    return ["&&","||"];
  } elsif ($prim eq "float32" or $prim eq "float64") {
    return ["+", "-", "/"];
  } elsif ($prim eq "int32" or $prim eq "uint32") {
    return ["+", "-", "/", "%", "&", "|", "^"];
  } else {
    die $prim;
  }
}

sub get_assops($) {
  my ($prim) = @_;
  if ($prim eq "bool") {
    return [];
  } elsif ($prim eq "float32" or $prim eq "float64") {
    return ["+=", "-=", "/="];
  } elsif ($prim eq "int32" or $prim eq "uint32") {
    return ["+=", "-=", "/=", "%=", "&=", "|=", "^="];
  } else {
    die $prim;
  }
}

sub gen_binop($$$$) {
  my ($binop, $class, $prim, $len) = @_;

  print << "EOH";

constexpr $class operator $binop ($prim k, const $class& v) {
  return binary_operation_kv<$class>([]($prim a, $prim b){ return a $binop b; }, k, v);
}

constexpr $class operator $binop (const $class& v, $prim k) {
  return binary_operation_vk<$class>([]($prim a, $prim b){ return a $binop b; }, v, k);
}

constexpr $class operator $binop (const $class& a, const $class& b) {
  return binary_operation_vv<$class>([]($prim a, $prim b){ return a $binop b; }, a, b);
}

EOH

}

sub gen_assop($$$$) {
  my ($assop, $class, $prim, $len) = @_;

  my (@ops1,@opsn);
  for (my $i = 0; $i < $len; ++$i) {
    push(@ops1, "v[$i] $assop k;\n");
    push(@opsn, "a[$i] $assop b[$i];\n");
  }
  my $ops1 = join("", @ops1);
  my $opsn = join("", @opsn);

  print << "EOH";

inline $class operator $assop ($class& v, $prim k) {
  $ops1
  return v;
}

inline $class operator $assop ($class& a, const $class& b) {
  $opsn

  return a;
}

EOH

}

my %done;

for my $config (sort keys %configs) {
  my $class = $config;
  my $kind = $configs{$class}->{"kind"};
  my $len = $configs{$class}->{"len"};
  my $rows = $configs{$class}->{"rows"};
  my $cols = $configs{$class}->{"cols"};
  my $prim = $configs{$class}->{"prim"};

  my @prim_args;
  my @prim_params;
  my @my_args;
  my @other_args;
  for (my $i = 0; $i < $len; ++$i) {
    my $vi = "v$i";
    push(@prim_params, "$prim $vi");
    push(@prim_args, $vi);
    push(@my_args, "(\*this)[$i]");
    push(@other_args, "other[$i]");
  }
  my $prim_params = join(", ", @prim_params);
  my $prim_args = join(", ", @prim_args);
  my $my_args = join(", ", @my_args);
  my $other_args = join(", ", @other_args);

  print << "EOH";

struct $class : std::array<$prim, $len> {
  constexpr $class($prim_params) : std::array<$prim, $len>({$prim_args}) {}

EOH

  for my $other (sort keys %done) {
    my $other_class = $other;
    my $other_kind = $configs{$other}->{"kind"};
    my $other_len = $configs{$other}->{"len"};
    my $other_rows = $configs{$other}->{"rows"};
    my $other_cols = $configs{$other}->{"cols"};
    my $other_prim = $configs{$other}->{"prim"};

    if ($other_kind eq $kind and $other_rows == $rows and $other_cols == $cols) {
      my $fwd_conv = get_conv($prim, $other_prim);
      my $bkd_conv = get_conv($other_prim, $prim);
      if ($bkd_conv ne "none") {

        if ($bkd_conv eq "explicit") {
          print " explicit ";
        }
print << "EOH";

  $class($other_class other) : $class($other_args) {}

EOH
      }
      if ($fwd_conv ne "none") {
        if ($fwd_conv eq "explicit") {
          print " explicit ";
        }

print << "EOH";

   operator $other_class() const {
     return $other_class($my_args);
   }

EOH
  }
    }
  }

print << "EOH";

};

STATIC_ASSERT(sizeof($class) == $len * sizeof($prim));

EOH

  for my $binop (@{get_binops($prim)}) {
    gen_binop($binop, $class, $prim, $len);
  }

  for my $assop (@{get_assops($prim)}) {
    gen_assop($assop, $class, $prim, $len);
  }

  $done{$class} = 1;
}

print << 'EOH';

}  // namespace gl
EOH

