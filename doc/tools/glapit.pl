#!/usr/bin/perl
#
# Generate glapit.h from api.h
#


sub makeGLAPItable
{
    my ( $infile, $outfile ) = @_;
    open API, $infile or die "ERROR: cannot open $infile\n";
    @data = <API>;
    close API;
    open GLAPIT, "> $outfile" or die "ERROR: cannot open $outfile\n";
    foreach $line (@data) {
	# $line = [const GLubyte *glGetString (GLenum name);\n]
	# $line = [void glInterleavedArrays (GLenum format, GLsizei stride, const GLvoid *pointer);\n]
	$line =~ m/[a-zA-Z \*]+gl/;
	# $& = [const GLubyte *gl]
	# $& = [void gl]
	if (substr($&, -3) eq " gl") {
	    $rv = substr($&, 0, -3);
	} else {
	    $rv = substr($&, 0, -2);
	}
	# $rv = [const GLubyte *]
	# $rv = [void]
	if ($rv eq "void") {
	    print GLAPIT "ENTRY_NR";
	} else {
	    print GLAPIT "ENTRY_RV";
	}
	# $' = [GetString (GLenum name);\n]
	# $' = [InterleavedArrays (GLenum format, GLsizei stride, const GLvoid *pointer);\n]
	$tmp = $';
	$tmp =~ m/[a-zA-Z0-9]+/;
	# $& = [GetString]
	# $& = [InterleavedArrays]
	$name = $&;
	# $' = [ (GLenum name);\n]
	# $' = [ (GLenum format, GLsizei stride, const GLvoid *pointer);\n]
	$para = $';
	$para =~ s/;/\n/;
	chomp($para);
	chomp($para);
	# $para = [ (GLenum name)]
	# $para = [ (GLenum format, GLsizei stride, const GLvoid *pointer)]
	$para =~ m/\(/;
	# $' = [GLenum name)]
	# $' = [GLenum format, GLsizei stride, const GLvoid *pointer)]
	print GLAPIT "($rv, $name, ($', (";
	$tmp = $';
	$tmp =~ s/\)/,/;
	# $tmp = [GLenum name,]
	# $tmp = [GLenum format, GLsizei stride, const GLvoid *pointer,]
	$l = 0;
	while ($tmp =~ m/[a-zA-Z_0-9]+,/g) {
	    $type = $`;
	    if ($l != 0) {
		print GLAPIT ", ";
	    }
	    $p = substr($&, 0, -1);
	    if ($p ne "void") {
		if ($type =~ m/\*/) {
		    $l = $l + 4;
		} elsif ($type =~ m/GLdouble/) {
		    $l = $l + 8;
		} elsif ($type =~ m/GLclampd/) {
		    $l = $l + 8;
		} else {
		    $l = $l + 4;
		}
		print GLAPIT "$p";
	    }
	}
	print GLAPIT "), $l)\n";
    }
    close GLAPIT;
}

&makeGLAPItable("api.h", "glapit.h");
