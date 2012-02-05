my $t = XML::Twig->new( # The twig will include
  twig_roots => {       # just the root and the
    'section/title' => \&print_n_purge, # selected
    'annex/title'   => \&print_n_purge  # titles.
  });
$t->parsefile('doc.xml');

sub print_n_purge
{
  my ($t, $elt) = @_;

  print $elt->text(); # Print the text
                      # (including sub-elements).
  $t->purge();        # Frees the memory.
}
