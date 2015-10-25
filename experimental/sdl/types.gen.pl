use strict;
use warnings;

print << 'EOH';
#pragma once

namespace gl {
EOH

my @configs = qw(
vec2
vec3
vec4
dvec2
dvec3
dvec4
bvec2
bvec3
bvec4
ivec2
ivec3
ivec4
uvec2
uvec3
uvec4
mat2x2
mat2x3
mat2x4
mat3x2
mat3x3
mat3x4
mat4x2
mat4x3
mat4x4
mat2
mat3
mat4
dmat2x2
dmat2x3
dmat2x4
dmat3x2
dmat3x3
dmat3x4
dmat4x2
dmat4x3
dmat4x4
dmat2
dmat3
dmat4
);

sub get_binops($) {
  my ($prim) = @_;
  if ($prim eq "bool") {
    return ["&&","||"];
  } elsif ($prim eq "float32" or $prim eq "float64") {
    return ["+", "-", "*", "/"];
  } elsif ($prim eq "int32" or $prim eq "uint32") {
    return ["+", "-", "*", "/", "%", "&", "|", "^"];
  } else {
    die $prim;
  }
}

sub gen_binop($$$$) {
  my ($binop, $class, $prim, $len) = @_;

  my (@ops1n,@opsn1,@opsnn);
  for (my $i = 0; $i < $len; ++$i) {
    push(@ops1n, "k $binop v[$i]");
    push(@opsn1, "v[$i] $binop k");
    push(@opsnn, "a[$i] $binop b[$i]");
  }
  my $ops1n = join(", ", @ops1n);
  my $opsn1 = join(", ", @opsn1);
  my $opsnn = join(", ", @opsnn);

  print << "EOH";

constexpr $class operator $binop ($prim k, const $class& v) {
  return $class($ops1n);
}

constexpr $class operator $binop (const $class& v, $prim k) {
  return $class($ops1n);
}

constexpr $class operator $binop (const $class& a, const $class& b) {
  return $class($opsnn);
}

EOH

}

sub gen_vector($$$) {
  my ($class, $prim, $len) = @_;

  my @prim_args;
  my @prim_params;
  for (my $i = 0; $i < $len; ++$i) {
    my $vi = "v$i";
    push(@prim_params, "$prim $vi");
    push(@prim_args, $vi);
  }
  my $prim_params = join(", ", @prim_params);
  my $prim_args = join(", ", @prim_args);

  print << "EOH";

struct $class : std::array<$prim, $len> {
  constexpr $class($prim_params) : std::array<$prim, $len>({$prim_args}) {}
};

STATIC_ASSERT(sizeof($class) == $len * sizeof($prim));

EOH

  for my $binop (@{get_binops($prim)}) {
    gen_binop($binop, $class, $prim, $len);
  }
}

sub gen_matrix($$$$) {
  my ($class, $prim, $cols, $rows) = @_;

  my $len = $cols * $rows;

  my @prim_args;
  my @prim_params;
  for (my $i = 0; $i < $len; ++$i) {
    my $vi = "v$i";
    push(@prim_params, "$prim $vi");
    push(@prim_args, $vi);
  }
  my $prim_params = join(", ", @prim_params);
  my $prim_args = join(", ", @prim_args);

  print << "EOH";

struct $class : std::array<$prim, $len> {
  constexpr $class($prim_params) : std::array<$prim, $len>({$prim_args}) {}
};

STATIC_ASSERT(sizeof($class) == $len * sizeof($prim));

EOH

  for my $binop (@{get_binops($prim)}) {
    gen_binop($binop, $class, $prim, $len);
  }

}

for my $config (@configs) {
  if ($config =~ /^(.*)vec(.*)$/) {
    my $prefix = $1;
    my $suffix = $2;
    my $prim;
    if ($prefix eq "") {
      $prim = "float32";
    } elsif ($prefix eq "d") {
      $prim = "float64";
    } elsif ($prefix eq "b") {
      $prim = "bool";
    } elsif ($prefix eq "i") {
      $prim = "int32";
    } elsif ($prefix eq "u") {
      $prim = "uint32";
    } else {
      die $prim;
    }
    my $class = $config;
    my $len = $suffix;
    gen_vector($class, $prim, $len);
  } elsif ($config =~ /^(.*)mat(\d*)$/) {
    my $class = $config;
    my $prefix = $1;
    my $len = $2;
    print "using ${class} = ${prefix}mat${len}x${len};\n";
  } elsif ($config =~ /^(.*)mat(\d*)x(\d*)$/) {
    my $class = $config;
    my $prefix = $1;
    my $suffix1 = $2;
    my $suffix2 = $3;
    my $prim;
    if ($prefix eq "") {
      $prim = "float32";
    } elsif ($prefix eq "d") {
      $prim = "float64";
    } elsif ($prefix eq "b") {
      $prim = "bool";
    } elsif ($prefix eq "i") {
      $prim = "int32";
    } elsif ($prefix eq "u") {
      $prim = "uint32";
    } else {
      die $prim;
    }
    my $cols = $suffix1;
    my $rows = $suffix2;
    gen_matrix($class, $prim, $cols, $rows);
  } else {
    die $config;
  }
}

print << 'EOH';

}  // namespace gl
EOH

