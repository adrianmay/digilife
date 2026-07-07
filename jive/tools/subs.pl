#!/usr/bin/perl -p

BEGIN { 
  ($p, $r) = splice @ARGV, 0, 2;
}
@append = ();
s{
    (\w*$p\w*)
}{
    my $orig = $1;
    (my $new = $orig) =~ s/$p/$r\E/g;
    push @append, $orig;
    $new;
}gex;
if (@append) {
    chomp;
    $_ .= " // " . join(" // ", @append) . "\n";
}
