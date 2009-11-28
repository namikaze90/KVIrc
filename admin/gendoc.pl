#!/usr/bin/perl

#============================================================================
#
#   File : gendoc.pl
#   Creation date : Sun 17 Dec 2006 20:36:07 by Szymon Stefanek
#
#   This file is part of the KVIrc IRC Client distribution
#   Copyright (C) 2000-2009 Szymon Stefanek <pragma at kvirc dot net>
#
#   This program is FREE software. You can redistribute it and/or
#   modify it under the terms of the GNU General Public License
#   as published by the Free Software Foundation; either version 2
#   of the License, or (at your opinion) any later version.
#
#   This program is distributed in the HOPE that it will be USEFUL,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
#   See the GNU General Public License for more details.
#
#   You should have received a copy of the GNU General Public License
#   along with this program. If not, write to the Free Software Foundation,
#   Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
#
#============================================================================

#################################################################################################
# GLOBAL CONFIGS
#################################################################################################

$g_currenttime=gmtime;
$g_currentuser = getlogin || getpwuid($<) || "Unknown";
$g_notetablebgcolor="#F0F0F0";
$g_notetextcolor="#909090";
$g_syntaxcolor="#802000";

$g_kvssyntaxcolor="#802000";
$g_kvstypenamecolor="#808080";
$g_kvstypedelimiterscolor="#8080a0";

$g_bodybgcolor="#FFFFFF";
$g_bodytextcolor="#000000";
$g_bodylinkcolor="#000000";
$g_tablebgcolor="#B4B4C8";
#$g_titletablebgcolor="#999999";
$g_titletextcolor="#31507B";
#$g_titletablebgcolor="#68838B"; #LightBlue4
$g_titletablebgcolor="#BEBEF0";
#$g_subtitletablebgcolor="#C5C5C5";
$g_subtitletablebgcolor="#D2D2D2";
$g_internaltablecolor="#D5D5D5";
$g_classfnctablecolor="#D5D5D5";
$g_classfncbodytablecolor="#E0E0E0";
$g_switchbodytablecolor="#D5D5D5";
$g_switchnametablecolor="#E0E0E0";
$g_bodytablebgcolor="#EAEAEA";
$g_commentcolor="#207500";
$g_fileextension=".html";
#$g_headerborderlightcolor="#A0A0A0";
#$g_headerborderdarkcolor="#000000";
$g_headerbgcolor="#FFFFFF";


$g_prefixes{'command'}="cmd";
$g_prefixes{'function'}="fnc";
$g_prefixes{'event'}="event";
$g_prefixes{'language'}="doc";
$g_prefixes{'class'}="class";
$g_prefixes{'module'}="module";
$g_prefixes{'widget'}="widget";

$g_version = "4.0.0";
$g_filehandle="";
$g_shortsIdx{"keyterms"}=0;
$g_directory = "";

#################################################################################################
# PARSE ARGS
#################################################################################################


sub usage
{
	print "\n";
	print "Usage:\n";
	print "  gendoc.pl [-v <kvirc_version>] <target_dir> <file1> <file2> ...\n";
	print "Parameters:\n";
	print "  <target_dir>       : directory where the doc files should be written\n";
	print "  <file1> <file2> ...: a list of files from which to extract docs\n";
	print "Options:\n";
	print "  -v forces the specified version to be displayed in the\n";
	print "     generated documents\n";
	print "\n";
}

$i = 0;
$cont = 1;
while($cont)
{
	if($ARGV[$i] eq "--version")
	{
		print "gendoc.pl 2.0.0: KVIrc documentation generator\n";
		exit(0);
	} elsif($ARGV[$i] eq "-v")
	{
		$i++;
		if($ARGV[$i] eq "")
		{
			usage();
			die "Switch -v requires a parameter"
		}
		$g_version = $ARGV[$i];
		$i++;
	} else {
		$cont = 0; # stop processing
	}
}

$g_directory = $ARGV[$i];
$i++;

if($g_directory eq "")
{
	usage();
	die "Missing target directory";
}

$j = 0;
while($ARGV[$i] ne "")
{
	$g_filesToProcess[$j] = $ARGV[$i];
	$j++;
	$i++;
}


#################################################################################################
# SUBS
#################################################################################################

sub print_header
{
	print $g_filehandle "<html>\n";
	print $g_filehandle "<head>\n";
	print $g_filehandle "<title>$_[0]</title>\n";

	print $g_filehandle "<style type=\"text/css\">\n";
	print $g_filehandle "body {\n";
	print $g_filehandle " font-size: 11pt;\n";
	print $g_filehandle " margin-left: 8px;\n";
	print $g_filehandle " margin-right: 8px;\n";
	print $g_filehandle " margin-top: 6px;\n";
	print $g_filehandle " margin-bottom: 6px;\n";
	print $g_filehandle " font-family:Helvetica,Arial,Verdana;\n";
	print $g_filehandle "}\n";

	print $g_filehandle "</style>\n";
	print $g_filehandle "<meta http-equiv=\"Content-Type\" content=\"text/html; charset=UTF-8\" >\n";
	print $g_filehandle "</head>\n";
	print $g_filehandle "<body bgcolor=\"$g_bodybgcolor\" text=\"$g_bodytextcolor\">\n";

	print $g_filehandle "<table width=\"100%\" cellpadding=\"1\" cellspacing=\"0\" border=\"0\">\n";
	print $g_filehandle " <tr>\n";
	print $g_filehandle "  <td align=\"left\" bgcolor=\"$g_headerbgcolor\">\n";
	print $g_filehandle "    <a href=\"index.html\"><img src=\"helplogoleft.png\" iorder=\"0\"></a>\n";
	print $g_filehandle "  </td>\n";
	print $g_filehandle "  <td align=\"right\" bgcolor=\"$g_headerbgcolor\">\n";
	print $g_filehandle "    <img src=\"helplogoright.png\">\n";
	print $g_filehandle "  </td>\n";
	print $g_filehandle " </tr>\n";
	print $g_filehandle "</table>\n";
}

sub print_footer
{
	print $g_filehandle "<hr>KVIrc $g_version Documentation<br>Generated by $g_currentuser at $g_currenttime\n";
	print $g_filehandle "</body>\n";
	print $g_filehandle "</html>\n";
}

sub print_tablestart
{
	print $g_filehandle "<table width=\"100%\" cellpadding=\"3\" cellspacing=\"1\" border=\"0\">\n";
	#print $g_filehandle "<tr><td bgcolor=\"$g_tablebgcolor\">\n";
}

sub print_tableend
{
	#print $g_filehandle "</td></tr>\n";
	print $g_filehandle "</table>\n";
}

sub print_title
{
	print $g_filehandle "  <tr bgcolor=\"$g_titletablebgcolor\">\n";
	print $g_filehandle "    <td>\n";
	print $g_filehandle "      <font color=\"$g_titletextcolor\">\n";

	print $g_filehandle "      <h1>$_[0]</h1>\n";
	if($_[1] ne "")
	{
		print $g_filehandle "$_[1]\n";
	} else {
		print $g_filehandle "\n";
	}

	print $g_filehandle "      </font>\n";
	print $g_filehandle "    </td>\n";
	print $g_filehandle "  </tr>\n";
}

sub print_twocolumntitle
{
	print $g_filehandle "  <tr bgcolor=\"$g_titletablebgcolor\">\n";
	print $g_filehandle "    <td colspan=\"2\">\n";
	print $g_filehandle "      <font color=\"$g_titletextcolor\">\n";

	print $g_filehandle "      <h1>$_[0]</h1>\n";
	if($_[1] ne "")
	{
		print $g_filehandle "$_[1]\n";
	} else {
		print $g_filehandle "\n";
	}

	print $g_filehandle "      </font>\n";
	print $g_filehandle "    </td>\n";
	print $g_filehandle "  </tr>\n";
}


sub print_subtitle
{
	print $g_filehandle "  <tr bgcolor=\"$g_subtitletablebgcolor\">\n";
	print $g_filehandle "    <td><b>$_[0]</b></td>\n";
	print $g_filehandle "  </tr>\n";
}

sub print_twocolumnsubtitle
{
	print $g_filehandle "  <tr bgcolor=\"$g_subtitletablebgcolor\">\n";
	print $g_filehandle "    <td colspan=\"2\"><b>$_[0]</b></td>\n";
	print $g_filehandle "  </tr>\n";
}

sub print_body
{
	print $g_filehandle "  <tr bgcolor=\"$g_bodytablebgcolor\">\n";
	print $g_filehandle "    <td>$_[0]</td>\n";
	print $g_filehandle "  </tr>\n";
}

sub print_twocolumnbody
{
	print $g_filehandle "<tr bgcolor=\"$g_bodytablebgcolor\">\n";
	print $g_filehandle "  <td>$_[0]</td>\n";
	print $g_filehandle "  <td>$_[1]</td>\n";
	print $g_filehandle "</tr>\n";
}

sub print_entry
{
	if($_[1] ne "")
	{
		if($_[0] ne "")
		{
			print_subtitle($_[0]);
		}
		print_body($_[1]);
	}
}

sub make_single_token
{
	$_[0] =~ s/^[	 ]*//g;
	$_[0] =~ s/[	 ]*$//g;
	$_[0] =~ s/\n//g;
}

sub make_single_line
{
	$_[0] =~ s/^[ 	\n]*//g;
	$_[0] =~ s/[	 \n]*$//gs;
}

sub make_syntax
{
	$_[0] =~ s/^[ 	\n]*//g;
	$_[0] =~ s/[	 \n]*$//gs;
	my @arry;
	@arry = split('\n',$_[0]);
	$_[0] = "";
	foreach(@arry)
	{
		$_ =~ s/^[ 	\n]*//g;
		if($_ ne "")
		{
			if($_[0] ne "")
			{
				$_[0] = "$_[0]\n$_";
			} else {
				$_[0] = "$_";
			}
		}
	}
}

sub extract_keyterms
{
	my($docfilename);
	my(%parts);
	my($part); # Part title
	my($partbody);
	my($tabblock);
	my($tmp);
	my($type);

	if(!open(CPPFILE,"$_[0]"))
	{
		return;
	}
	# Process the entire file

	while(<CPPFILE>)
	{
		if(/^[ 	]*\@doc:[ 	a-z_]*/)
		{
			# Process a single document block

			$docfilename="$_";
			$docfilename=~ s/[ 	]*//g;
			$docfilename=~ s/\@doc://g;
			$docfilename=~ s/\n//g;
			$docfilename=~ s/\r//g;
			$docfilename=~ s/([a-zA-Z_]*)/\L\1/g;

			undef %parts;
			$part = "";

			INNERLOOP: while(<CPPFILE>)
			{

				if(/^[	 ]*\*\/[ 	]*/)
				{
					# End of comment
					if(($part ne "") && ($partbody ne "") && ($partbody ne "\n"))
					{
						# We have an entire part to store
						$parts{$part}="$partbody";
					}
					last INNERLOOP;
				} else {
					# Inside a part
					if(/^[	 ]*\@[a-z]*:[	 ]*/)
					{
						# A part title
						if(($part ne "") && ($partbody ne "") && ($partbody ne "\n"))
						{
							# We have an entire part to store
							$parts{$part}="$partbody";
						}
						# Start of the part
						# Extract the title
						$part="$_";
						$part=~ s/[	 ]*//g;
						$part=~ s/\@//g;
						$part=~ s/://g;
						$part=~ s/\n//g;
						# Clear the body (begin)
						$partbody="";
					} else {
						# Somewhere in a part body
						if(($_ ne "") && ($_ ne "\n"))
						{
							if($partbody eq "")
							{
								# If it is the first line of the part body
								# Extract the amount of tabs that the part has
								# We will use it to remove the C++ indentation
								$tabblock = "$_";
								$tabblock =~ s/^([	]*).*/\1/g;
								$tabblock =~ s/\n//g;
							}

							if($tabblock ne "")
							{
								# If we have the initial tabblock , remove it from the line (remove indentation)
								$_ =~ s/^$tabblock//g;
							}

							process_body_line($_);

							$partbody="$partbody$_";
						}
					}
				}
			}

			# Ok...we have a document in $parts
			make_single_line($parts{'short'});
			make_single_token($parts{'title'});
			make_single_token($parts{'type'});

			$parts{'type'}=~ s/\@//g;
			$parts{'type'}=~ s/://g;

			if($parts{'type'} eq "")
			{
				$parts{'type'}="generic";
			}

			$type = $parts{'type'};

			$tmp = $g_prefixes{$type};
			if($tmp eq "")
			{
				$tmp="doc";
			}

			$docfilename="$tmp\_$docfilename";

			$zzz="keyterms";

			if($parts{'keyterms'} ne "")
			{
				$keyterms = $parts{'keyterms'};
				$keyterms =~ s/\n//gs;

				for $term (split(/,/,$keyterms))
				{
					make_single_token($term);
					if($term ne "")
					{
						$termx=$term;
						$termx=~ s/([\+\(\)\?\.\:\*\|\=\;\^\!\~])/\\$1/g;

						$g_keyterms{$term} = "$docfilename$g_fileextension";
						$g_keytermsclean{$term} = "$termx";

						$tmp="keyterms_$g_shortsIdx{$zzz}";
						$g_shorts{$tmp}="$term<!>$parts{'short'}<!>$docfilename$g_fileextension";
						#print "GOT $g_shorts{$tmp} ($tmp)";
						$g_shortsIdx{$zzz}++;
					}
				}
			}

			$tmp="keyterms_$g_shortsIdx{$zzz}";
			$g_shorts{$tmp}="$parts{'title'}<!>$parts{'short'}<!>$docfilename$g_fileextension";
			#print "GOT $g_shorts{$tmp} ($tmp)";
			$g_shortsIdx{$zzz}++;
		}
	}

	close(CPPFILE);
}



sub substitute_keyterms
{
	my(@lines);
	my(@tmp);
	my($left);
	my($right);
	my($line);
	my($term);

	# Kinda complex keyword substitution routine


	# For each keyterm we have
	for $term (@g_keytermsSorted)
	{
		# If the doc we're scanning isn't the keyword target
		if($_[1] ne $g_keyterms{$term})
		{
			$termclean=$g_keytermsclean{$term};

			# If the doc matches the keyterm at least once
			if($_[0] =~ /$termclean/)
			{
				$ref=$g_keyterms{$term};
				$ref =~ s/([a-zA-Z_]*)/\L\1/g;

				@tmp = split(/</,$_[0]);
				$_[0]="";
				$first=1;
				$skipIt = 0;
				for(@tmp)
				{
					if($skipIt)
					{
						if(/[A-Za-z0-9\"]>/)
						{
							$skipIt=0 if /^[ ]*\/a[ ]*>/;
						}
					}

					if($skipIt)
					{
						$_[0]="$_[0]<$_";
					} else {
						if(/[A-Za-z0-9\"]>/)
						{
							if(/^[ ]*a[ 	][ 	]*h/)
							{
								$skipIt=1;
								$first ? $_[0] .= $_ : $_[0] .= "<$_";
							} else {
								($left,$right) = split(/>/,$_);
								$right=~s/([^A-Za-z0-9<>\+\-\=_])($termclean)([^A-Za-z0-9<>\+\-\=_])/$1<a href=\"$ref\">$2<\/a>$3/ig;
								$_[0] .= "<$left>$right";
							}
						} else {
							$_=~s/([^A-Za-z0-9<>\+\-\=_\.])($termclean)([^A-Za-z0-9<>\+\-\=_\.])/$1<a href=\"$ref\">$2<\/a>$3/ig;
							$_[0] .= $_;
						}
						$first=0;
					}
				}
			}
		}
	}
}


sub build_usage_from_kvs_syntax_line
{
	$_[0] =~ s/\&lt\;([A-Za-z0-9_]+):([A-Za-z0-9_]+)\&gt\;/\&lt\;\1\&gt\;/g;
	$_[0] =~ s/\[([A-Za-z0-9_]+):([A-Za-z0-9_]+)\]/\[;\1\]/g;
	$_[0] =~ s/^\&lt\;([A-Za-z0-9_]+)\&gt\;//g;
}

sub build_usage_from_kvs_syntax
{
	my @arry;
	@arry = split('\n',$_[0]);
	$tmp = "";
	foreach(@arry)
	{
		build_usage_from_kvs_syntax_line($_);
		if($tmp ne "")
		{
			$tmp = "$tmp\n$_";
		} else {
			$tmp = "$_";
		}
	}
	return $tmp;
}

sub process_kvs_syntax_line
{
	$_[0] =~ s/\&lt\;([A-Za-z0-9_]+):([A-Za-z0-9_]+)\&gt\;/\&lt\;\1\<font color=\"$g_kvstypenamecolor\"\>:\2\<\/font\>\&gt\;/g;
	$_[0] =~ s/\[([A-Za-z0-9_]+):([A-Za-z0-9_]+)\]/\[\1\<font color=\"$g_kvstypenamecolor\"\>:\2\<\/font\>\]/g;
	$_[0] =~ s/^\&lt\;([A-Za-z0-9_]+)\&gt\;/\&lt\;\<font color=\"$g_kvstypenamecolor\"\>\1\<\/font\>\&gt\;/g;
	$_[0] =~ s/^([A-Za-z0-9_\.]+)/\<b\>\1\<\/b\>/g;
	$_[0] =~ s/(\$[A-Za-z0-9_\.]*)/\<b\>\1\<\/b\>/g;
	$_[0] =~ s/\(/\<b\>\(<\/b\>/g;
	$_[0] =~ s/\)/\<b\>\)<\/b\>/g;
	$_[0] =~ s/\&lt\;/\<font color=\"$g_kvstypedelimiterscolor\"\>\&lt\;\<\/font\>/g;
	$_[0] =~ s/\&gt\;/\<font color=\"$g_kvstypedelimiterscolor\"\>\&gt\;\<\/font\>/g;
	$_[0] =~ s/\[/\<font color=\"$g_kvstypedelimiterscolor\"\>\[\<\/font\>/g;
	$_[0] =~ s/\]/\<font color=\"$g_kvstypedelimiterscolor\"\>\]\<\/font\>/g;
}

sub process_kvs_syntax
{
	my @arry;
	@arry = split('\n',$_[0]);
	$_[0] = "";
	foreach(@arry)
	{
		process_kvs_syntax_line($_);
		if($_[0] ne "")
		{
			$_[0] = "$_[0]\n$_";
		} else {
			$_[0] = "$_";
		}
	}
}




sub process_body_line
{
	$_[0] =~ s/	/\&nbsp\;\&nbsp\;\&nbsp\;\&nbsp\;/g;
	$_[0] =~ s/\</\&lt\;/g;
	$_[0] =~ s/\>/\&gt\;/g;
	$_[0] =~ s/\[br\]/\<br\>/g;
	$_[0] =~ s/\[b\]/\<b\>/g;
	$_[0] =~ s/\[p\]/\<p\>/g;
	$_[0] =~ s/\[\/p\]/\<\/p\>/g;
	$_[0] =~ s/\[\/b\]/\<\/b\>/g;
	# [big][/big] is an alias to [title][/title]
	$_[0] =~ s/\[big\]/\<\/td\>\<\/tr\>\<tr\>\<td bgcolor=\"#A7D3DB\"\>\<div style=\"font-size:16pt;font-weight:800;\"\>/g;
	$_[0] =~ s/\[\/big\]/\<\/div\>\<\/td\>\<\/tr\>\<tr\>\<td bgcolor=\"$g_bodytablebgcolor\"\>/g;
#	$_[0] =~ s/\[title\]/\<\/td\>\<\/tr\>\<tr\>\<td bgcolor=\"#A7D3DB\"\>\<div style=\"font-size:16pt;font-weight:800;\"\>/g;
#	$_[0] =~ s/\[\/title\]/\<\/div\>\<\/td\>\<\/tr\>\<tr\>\<td bgcolor=\"$g_bodytablebgcolor\"\>/g;
	$_[0] =~ s/\[title\]/\<\/td\>\<\/tr\>\<tr\>\<td bgcolor=\"#A7D3DB\"\>\<div style=\"font-size:16pt;font-weight:800;\"\>/g;
	$_[0] =~ s/\[\/title\]/\<\/div\>\<\/td\>\<\/tr\>\<tr\>\<td bgcolor=\"$g_bodytablebgcolor\"\>/g;
#	$_[0] =~ s/\[title\]/\<table width="100%" border="0" cellpadding="1" cellspacing="0" bgcolor=\"#80B3BB\"\>\<tr\>\<td\>\<table align=\"left\" width="100%" border="0" cellpadding="3" cellspacing="0" bgcolor=\"#A7D3DB\"\>\<tr\>\<td\>\<div style=\"font-size:16pt;font-weight:800;\"\>/g;
#	$_[0] =~ s/\[\/title\]/\<\/div\>\<\/td\>\<\/tr\>\<\/table\>\<\/td\>\<\/tr\>\<\/table\>/g;
	$_[0] =~ s/\[subtitle\]/\<\/td\>\<\/tr\>\<tr\>\<td bgcolor=\"#D6E0E8\"\>\<div style=\"font-size:13pt;font-weight:800;\"\>/g;
	$_[0] =~ s/\[\/subtitle\]/\<\/div\>\<\/td\>\<\/tr\>\<tr\>\<td bgcolor=\"$g_bodytablebgcolor\"\>/g;
#	$_[0] =~ s/\[subtitle\]/\<table width="100%" border="0" cellpadding="1" cellspacing="0" bgcolor=\"#A6C0C8\"\>\<tr\>\<td\>\<table align=\"left\" width="100%" border="0" cellpadding="3" cellspacing="0" bgcolor=\"#D6E0E8\"\>\<tr\>\<td\>\<div style=\"font-size:13pt;font-weight:800;\"\>/g;
#	$_[0] =~ s/\[\/subtitle\]/\<\/div\>\<\/td\>\<\/tr\>\<\/table\>\<\/td\>\<\/tr\>\<\/table\>/g;
	$_[0] =~ s/\[pre\]/\<pre\>/g;
	$_[0] =~ s/\[\/pre\]/\<\/pre\>/g;
	$_[0] =~ s/\[center\]/\<center\>/g;
	$_[0] =~ s/\[\/center\]/\<\/center\>/g;
	$_[0] =~ s/\[i\]/\<i\>/g;
	$_[0] =~ s/\[\/i\]/\<\/i\>/g;
	$_[0] =~ s/\[ul\]/\<ul\>/g;
	$_[0] =~ s/\[\/ul\]/\<\/ul\>/g;
	$_[0] =~ s/\[li\]/\<li\>/g;
	$_[0] =~ s/\[\/li\]/\<\/li\>/g;
	$_[0] =~ s/\[table\]/\<table bgcolor=\"$g_internaltablecolor\" width=\"100\%\"\>/g;
	$_[0] =~ s/\[\/table\]/\<\/table\>/g;
	$_[0] =~ s/\[tr\]/\<tr\>/g;
	$_[0] =~ s/\[\/tr\]/\<\/tr\>/g;
	$_[0] =~ s/\[td\]/\<td\>/g;
	$_[0] =~ s/\[\/td\]/\<\/td\>/g;
#	$_[0] =~ s/\[example\][ 	\n]*/<p><table width=\"100%\" cellpadding=\"1\" cellspacing=\"0\" border=\"0\" bgcolor=\"#D5D5D5\"><tr><td><table align=\"left\" width=\"100%\" cellpadding=\"3\" cellspacing=\"0\" border=\"0\" bgcolor=\"#F5F5F5\">\<tr\>\<td\>\<pre\>\<code\>\<font color=\"#FF0000\"\>/g;
#	$_[0] =~ s/\[\/example\]/<\/font><\/code><\/pre><\/td><\/tr><\/table><\/td><\/tr><\/table><\/p>/g;
	$_[0] =~ s/\[example\][ 	\n]*/<p><table width=\"100%\" cellpadding=\"3\" cellspacing=\"1\" border=\"0\" bgcolor=\"#D5D5D5\">\<tr\>\<td bgcolor=\"#F5F5F5\"\>\<pre\>\<code\>\<font color=\"#FF0000\"\>/g;
	$_[0] =~ s/\[\/example\]/<\/font><\/code><\/pre><\/td><\/tr><\/table><\/p>/g;
	$_[0] =~ s/\[comment\]/\<font color=\"$g_commentcolor\"\>/g;
	$_[0] =~ s/\[\/comment\]/\<\/font\>/g;
	$_[0] =~ s/\[doc\]([a-zA-Z0-9_]*)\[\/doc\]/\<a href=\"doc_\L\1$g_fileextension">\1\<\/a\>/g;
	$_[0] =~ s/\[doc:([a-zA-Z0-9_]*)\]([a-zA-Z0-9_\-\&\;\. ]*)\[\/doc\]/\<a href=\"doc_\L\1$g_fileextension"\>\2\<\/a\>/g;
	$_[0] =~ s/\[cmd\]([a-zA-Z0-9_\.]*)\[\/cmd\]/\<a href=\"cmd_\L\1$g_fileextension">\1\<\/a\>/g;
	$_[0] =~ s/\[cmd:([a-zA-Z0-9_\.]*)\]([a-zA-Z0-9_\-\&\;\. ]*)\[\/cmd\]/\<a href=\"cmd_\L\1$g_fileextension"\>\2\<\/a\>/g;
	$_[0] =~ s/\[fnc\]\$([a-zA-Z0-9_\.]*)\[\/fnc\]/\<a href=\"fnc_\L\1$g_fileextension">\$\1\<\/a\>/g;
	$_[0] =~ s/\[fnc\]\$([a-zA-Z0-9_\.]*)\(\)\[\/fnc\]/\<a href=\"fnc_\L\1$g_fileextension">\$\1()\<\/a\>/g;
	$_[0] =~ s/\[fnc\]([a-zA-Z0-9_\.]*)\[\/fnc\]/\<a href=\"fnc_\L\1$g_fileextension">\$\1\<\/a\>/g;
	$_[0] =~ s/\[fnc:\$([a-zA-Z0-9_\.]*)\]\$([a-zA-Z0-9_\-\&\;\. ]*)\[\/fnc\]/\<a href=\"fnc_\L\1$g_fileextension"\>\$\2\<\/a\>/g;
	$_[0] =~ s/\[event\]\$([a-zA-Z0-9_]*)\[\/event\]/\<a href=\"event_\L\1$g_fileextension">\$\1\<\/a\>/g;
	$_[0] =~ s/\[event:([a-zA-Z0-9_]*)\]([a-zA-Z0-9_]*)\[\/event\]/\<a href=\"event_\L\1$g_fileextension">\2\<\/a\>/g;
	$_[0] =~ s/\[class\]([a-zA-Z0-9_]*)\[\/class\]/\<a href=\"class_\L\1$g_fileextension">\1\<\/a\>/g;
	$_[0] =~ s/\[class:([a-zA-Z0-9_]*)\]([a-zA-Z0-9_ ]*)\[\/class\]/\<a href=\"class_\L\1$g_fileextension">\2\<\/a\>/g;
	$_[0] =~ s/\[module:([a-zA-Z0-9_]*)\]([a-zA-Z0-9_ ]*)\[\/module\]/\<a href=\"module_\L\1$g_fileextension">\2\<\/a\>/g;
	$_[0] =~ s/\[widget:([a-zA-Z0-9_]*)\]([a-zA-Z0-9_ ]*)\[\/widget\]/\<a href=\"widget_\L\1$g_fileextension">\2\<\/a\>/g;
	$_[0] =~ s/\[classfnc:([a-zA-Z0-9_]*)\]\$([a-zA-Z0-9_]*)\[\/classfnc\]/\<a href=\"class_\L\1$g_fileextension#\2">\$\2\<\/a\>/g;
	$_[0] =~ s/\[classfnc\]\$([a-zA-Z0-9_]*)\[\/classfnc\]/\<a href=\"#\1">\$\1\<\/a\>/g;
	$_[0] =~ s/\[classsignal:([a-zA-Z0-9_]*)\]([a-zA-Z0-9_]*)\[\/classsignal\]/\<a href=\"class_\L\1$g_fileextension#\2">\2\<\/a\>/g;
	$_[0] =~ s/\[classsignal\]([a-zA-Z0-9_]*)\[\/classsignal\]/\<a href=\"#\L\1">\1\<\/a\>/g;
	$_[0] =~ s/\[anchorlink:([a-zA-Z0-9_]*)\]/\<a href=\"#\1"\>/g;
	$_[0] =~ s/\[\/anchorlink\]/\<\/a\>/g;
	$_[0] =~ s/\[anchor:([a-zA-Z0-9_]*)\]/\<a name=\"\1"\>/g;
	$_[0] =~ s/\[\/anchor\]/\<\/a\>/g;
	$_[0] =~ s/\[note\][ 	\n]*/<p>\<table width=\"100%\"\ cellpadding=\"2\">\<tr\>\<td\ bgcolor=\"$g_notetablebgcolor\">\<font color=\"$g_notetextcolor\" size=\"-1\"\>/g;
	$_[0] =~ s/\[\/note\]/\<\/font\>\<\/td\>\<\/tr\>\<\/table\><\/p>/g;
}

sub process_file
{
	my($docfilename);
	my(%parts);
	my($part); # Part title
	my($partbody);
	my($tabblock);
	my($tmp);
	my($type);


	if(!open(CPPFILE,"$_[0]"))
	{
		return;
	}
	# Process the entire file

	while(<CPPFILE>)
	{
		if(/^[ 	]*\@doc:[ 	a-z_]*/)
		{
			# Process a single document block

			$docfilename="$_";
			$docfilename=~ s/[ 	]*//g;
			$docfilename=~ s/\@doc://g;
			$docfilename=~ s/\n//g;
			$docfilename=~ s/([a-zA-Z_]*)/\L\1/g;

			undef %parts;
			$part = "";

			INNERLOOP: while(<CPPFILE>)
			{

				if(/^[	 ]*\*\/[ 	]*/)
				{
					# End of comment
					if(($part ne "") && ($partbody ne "") && ($partbody ne "\n"))
					{
						# We have an entire part to store
						$parts{$part}="$partbody";
					}
					last INNERLOOP;
				} else {
					# Inside a part
					if(/^[	 ]*\@[a-z]*:[	 ]*/)
					{
						# A part title
						if(($part ne "") && ($partbody ne "") && ($partbody ne "\n"))
						{
							# We have an entire part to store
							$parts{$part}="$partbody";
						}
						# Start of the part
						# Extract the title
						$part="$_";
						$part=~ s/[	 ]*//g;
						$part=~ s/\@//g;
						$part=~ s/://g;
						$part=~ s/\n//g;
						# Clear the body (begin)
						$partbody="";
					} else {
						# Somewhere in a part body
						if(($_ ne "") && ($_ ne "\n"))
						{
							if($partbody eq "")
							{
								# If it is the first line of the part body
								# Extract the amount of tabs that the part has
								# We will use it to remove the C++ indentation
								$tabblock = "$_";
								$tabblock =~ s/^([	]*).*/\1/g;
								$tabblock =~ s/\n//g;
							}

							if($tabblock ne "")
							{
								# If we have the initial tabblock , remove it from the line (remove indentation)
								$_ =~ s/^$tabblock//g;
							}

							process_body_line($_);

							$partbody="$partbody$_";
						}
					}
				}
			}

			# Ok...we have a document in $parts
			# Process the title
			if($parts{'title'} eq "")
			{
				print "\n ($_[0]) Warning: no title specified for $docfilename\n";
				$parts{'title'}="No title specified";
			}

			make_single_token($parts{'title'});
			if($parts{'syntax'} ne "")
			{
				if($parts{'usage'} eq "")
				{
					$parts{'usage'} = build_usage_from_kvs_syntax($parts{'syntax'})
				}
			}
			make_syntax($parts{'usage'});
			make_syntax($parts{'syntax'});
			make_single_line($parts{'parameters'});
			make_single_line($parts{'inherits'});
			make_single_token($parts{'short'});
			make_single_token($parts{'window'});
			make_single_token($parts{'type'});

			$parts{'type'}=~ s/\@//g;
			$parts{'type'}=~ s/://g;

			if($parts{'type'} eq "")
			{
				$parts{'type'}="generic";
			}

			$type = $parts{'type'};

			$tmp = $g_prefixes{$type};
			if($tmp eq "")
			{
				$tmp="doc";
			}
			$docfilename="$tmp\_$docfilename";

			if($g_shortsIdx{$type} eq "")
			{
				$g_shortsIdx{$type} = 0;
			}

			$tmp="$type\_$g_shortsIdx{$type}";
			$g_shorts{$tmp}="$parts{'title'}<!>$parts{'short'}<!>$docfilename$g_fileextension";
			#print "$tmp, $g_shorts{$tmp}\n";
			$g_shortsIdx{$type}++;

			if($parts{'body'} ne "")
			{
				substitute_keyterms($parts{'body'},"$docfilename$g_fileextension");
			}
			if($parts{'description'} ne "")
			{
				substitute_keyterms($parts{'description'},"$docfilename$g_fileextension");
			}
			if($parts{'switches'} ne "")
			{
				substitute_keyterms($parts{'switches'},"$docfilename$g_fileextension");
			}
			use File::Path;
			mkpath("$g_directory");
			if(open(DOCFILE,">$g_directory/$docfilename$g_fileextension"))
			{
				$g_filehandle=DOCFILE;

				print_header($parts{'title'});
				print_tablestart();

				print_title($parts{'title'},$parts{'short'});
				if($parts{'usage'} ne "")
				{
					print_entry("Usage","<font color=\"$g_syntaxcolor\"><pre><code>$parts{'usage'}</code></pre></font>");
				}
				if($parts{'parameters'} ne "")
				{
					print_entry("Parameters","<font color=\"$g_syntaxcolor\"><pre><code>$parts{'parameters'}</code></pre></font>");
				}

				print_entry("Inherits","$parts{'inherits'}");
				print_entry("Window","$parts{'window'}");
				print_entry("","$parts{'body'}");
				print_entry("Description","$parts{'description'}");

				if($parts{'switches'} ne "")
				{
					print_subtitle("Switches");

					print DOCFILE "  <tr bgcolor=\"$g_bodytablebgcolor\">\n";
					print DOCFILE "    <td>\n";
					print DOCFILE "      <table bgcolor=\"$g_switchbodytablecolor\">\n";

					@lines = split("\n","$parts{'switches'}");
					$swbody = "";
					for(@lines)
					{
						if(/!sw:.*/)
						{
							if("$swbody" ne "")
							{
								print DOCFILE "<tr><td>$swbody</td></tr>\n";
								$swbody = "";
							}
							$_ =~ s/!sw:[ 	]*//g;
							$_ =~ s/^[	 ]*//g;
							$_ =~ s/\n//gs;
							$tmp = $_;
							$tmp =~ s/\(.*\)//g;
							$tmp =~ s/\$//g;
							print DOCFILE "<tr bgcolor=\"$g_switchnametablecolor\"><td><b>$_</b></td></tr>\n";
						} else {
							if($swbody ne "")
							{
								$swbody="$swbody $_";
							} else {
								$swbody="$_";
							}
						}

					}
					if("$swbody" ne "")
					{
						print DOCFILE "<tr><td>$swbody</td></tr>\n";
					}

					print DOCFILE "      </table>\n";
					print DOCFILE "    </td>\n";
					print DOCFILE "  </tr>\n";
				}


				if($parts{'functions'} ne "")
				{
					print_subtitle("Functions");

					print DOCFILE "  <tr bgcolor=\"$g_bodytablebgcolor\">\n";
					print DOCFILE "    <td>\n";
					print DOCFILE "      <table bgcolor=\"$g_classfncbodytablecolor\">\n";

					@lines = split("\n","$parts{'functions'}");
					$fncbody = "";
					for(@lines)
					{
						if(/!fn:.*/)
						{
							if("$fncbody" ne "")
							{
								print DOCFILE "<tr><td>$fncbody</td></tr>\n";
								$fncbody = "";
							}
							$_ =~ s/!fn:[ 	]*//g;
							$_ =~ s/^[	 ]*//g;
							$_ =~ s/\n//gs;
							$tmp = $_;
							$tmp =~ s/\(.*\)//g;
							$tmp =~ s/\$//g;
							print DOCFILE "<tr bgcolor=\"$g_classfnctablecolor\"><td><code><b><font color=\"$g_examplecolor\"><a name=\"$tmp\">$_</a></font></b></code></td></tr>\n";
						} else {
							if($fncbody ne "")
							{
								$fncbody="$fncbody $_";
							} else {
								$fncbody="$_";
							}
						}

					}
					if("$fncbody" ne "")
					{
						print DOCFILE "<tr><td>$fncbody</td></tr>\n";
					}

					print DOCFILE "      </table>\n";
					print DOCFILE "    </td>\n";
					print DOCFILE "  </tr>\n";
				}

				if($parts{'signals'} ne "")
				{
					print_subtitle("Signals");

					print DOCFILE "  <tr bgcolor=\"$g_bodytablebgcolor\">\n";
					print DOCFILE "    <td>\n";
					print DOCFILE "      <table bgcolor=\"$g_classfncbodytablecolor\">\n";

					@lines = split("\n","$parts{'signals'}");
					$sigbody = "";
					for(@lines)
					{
						if(/!sg:.*/)
						{
							if("$sigbody" ne "")
							{
								print DOCFILE "<tr><td>$sigbody</td></tr>\n";
								$sigbody = "";
							}
							$_ =~ s/!sg:[ 	]*//g;
							$_ =~ s/^[	 ]*//g;
							$_ =~ s/\n//gs;
							$tmp = $_;
							$tmp =~ s/\(.*\)//g;
							$tmp =~ s/\$//g;
							print DOCFILE "<tr bgcolor=\"$g_classfnctablecolor\"><td><code><b><font color=\"$g_examplecolor\"><a name=\"$tmp\">$_</a></font></b></code></td></tr>\n";
						} else {
							if($sigbody ne "")
							{
								$sigbody="$sigbody $_";
							} else {
								$sigbody="$_";
							}
						}

					}
					if("$sigbody" ne "")
					{
						print DOCFILE "<tr><td>$sigbody</td></tr>\n";
					}

					print DOCFILE "      </table>\n";
					print DOCFILE "    </td>\n";
					print DOCFILE "  </tr>\n";
				}

				if($parts{'syntax'} ne "")
				{
					process_kvs_syntax($parts{'syntax'});
					print_entry("Syntax Specification","<font color=\"$g_kvssyntaxcolor\"><pre><code>$parts{'syntax'}</code></pre></font>");
				}

				print_entry("Examples","$parts{'examples'}");
				print_entry("See also","$parts{'seealso'}");
				print_tableend();


				if($parts{'type'} eq "command")
				{
					print DOCFILE "<hr><a href=\"index$g_fileextension\">Index</a>, <a href=\"doc_command_alphabetic_a$g_fileextension\">Commands</a>\n";
				} elsif($parts{'type'} eq "function")
				{
					print DOCFILE "<hr><a href=\"index$g_fileextension\">Index</a>, <a href=\"doc_function_alphabetic_a$g_fileextension\">Functions</a>\n";
				} elsif($parts{'type'} eq "event")
				{
					print DOCFILE "<hr><a href=\"index$g_fileextension\">Index</a>, <a href=\"doc_event_alphabetic_a$g_fileextension\">Events</a>\n";
				} elsif($parts{'type'} eq "generic")
				{
					print DOCFILE "<hr><a href=\"index$g_fileextension\">Index</a>, <a href=\"doc_generic_index_all$g_fileextension\">Miscellaneous</a>\n";
				} elsif($parts{'type'} eq "language")
				{
					print DOCFILE "<hr><a href=\"index$g_fileextension\">Index</a>, <a href=\"doc_language_index_all$g_fileextension\">Language Overview</a>\n";
				} elsif($parts{'type'} eq "class")
				{
					print DOCFILE "<hr><a href=\"index$g_fileextension\">Index</a>, <a href=\"doc_class_index_all$g_fileextension\">Object Classes</a>\n";
				} elsif($parts{'type'} eq "module")
				{
					print DOCFILE "<hr><a href=\"index$g_fileextension\">Index</a>, <a href=\"doc_module_index_all$g_fileextension\">Modules</a>\n";
				} elsif($parts{'type'} eq "widget")
				{
					print DOCFILE "<hr><a href=\"index$g_fileextension\">Index</a>, <a href=\"doc_widget_index_all$g_fileextension\">Features</a>\n";
				}

				print_footer(DOCFILE);

				close(DOCFILE);
			} else { print "Can't open $g_directory/$docfilename$g_fileextension for writing\n"; }
		}
	}

	close(CPPFILE);
}


#################################################################################################
# COMMAND/FUNCTION.... INDEXES
#################################################################################################

sub generate_indexes
{
	my(@oldCommands);
	my(@commands);
	my(@sortedCommands);
	my(@chars);
	my($alllinks);
	my($upcase);
	my($count);

	my($doctitle);
	my($category);
	my($number);

	$doctitle = $_[0];
	$category = $_[1];
	$number = $_[2];

	#print("Generating indexes for $doctitle $category $number\n");

	######################################################
	# generate some helper stuff (alphabetic index links)

	@chars=("\$","a","b","c","d","e","f","g","h","i","j","k","l","m","n","o","p","q","r","s","t","u","v","w","x","y","z");

	$i = 0;

	$alllinks="<table width=\"100%\" cellpadding=\"0\" cellspacing=\"0\"><tr>";

	for(@chars)
	{
		$alllinks = "$alllinks <td><a href=\"doc_$category\_alphabetic_$_$g_fileextension\">$_</a></td>\n";
		$i++;
		if($i > 13)
		{
			$i = 0;
			$alllinks = "$alllinks</tr><tr>\n";
		}
	}

	$alllinks ="$alllinks <td><a href=\"doc_$category\_index_all$g_fileextension\">All</a></td></tr></table>";

	#####################################
	# Simple plain long index

	$i=0;

	undef %terms;

	while($i < $g_shortsIdx{$category})
	{
		$tmp="$category\_$i";
		($cmd,$short,$link) = split("<!>",$g_shorts{$tmp});
		$terms{$cmd} = "1";
		$commands[$i]=$g_shorts{$tmp};
		$commands[$i] =~ s/\n//g;
		$i++;
	}

	#####################################
	# Load the other terms from the db (if there)
	# Make sure that we do not overwrite the current entries
	if(open(OLDDB,"$g_directory/_db_$category.idx"))
	{
		while(<OLDDB>)
		{
			$_ =~ s/\n//g;
			if($_ ne "")
			{
				($cmd,$short,$link) = split("<!>",$_);
				if($terms{$cmd} ne "1")
				{
					$terms{$cmd} = "1";
					$commands[$i]=$_;
					$i++;
				}
			}
		}
		close(OLDDB);
	}

	$count = $i;


	#####################################
	# Re-dump them
	if(open(OLDDB,"> $g_directory/_db_$category.idx"))
	{
		$i = 0;
		while($i < $count)
		{
			print OLDDB "$commands[$i]\n";
			$i++;
		}
		close(OLDDB);
	}

	@sortedCommands = sort @commands;

	if(open(CMDINDEX,">$g_directory/doc_$category\_index_all$g_fileextension"))
	{
		$g_filehandle=CMDINDEX;

		print_header("$doctitle: All");
		print_tablestart();
		print_title("$doctitle: All","");
		print_tableend();

		print_tablestart();
		print_body($alllinks);
		print_tableend();

		print_tablestart();

		$i=0;

		while($i < $count)
		{
			($cmd,$short,$link) = split("<!>",$sortedCommands[$i]);
			$link =~ s/([a-zA-Z_]*)/\L\1/g;
			print_twocolumnbody("<a href=\"$link\">$cmd</a>",$short);
			$i++;
		}

		print_tableend();

		print_tablestart();
		print_body($alllinks);
		print_tableend();

		print_footer();

		close(CMDINDEX);
	} else {
		print "Can't open $g_directory/doc_$category\_index_all$g_fileextension for writing\n";
	}


	#######################
	# Alphabetic

	for(@chars)
	{
		if(open(CMDINDEX,">$g_directory/doc_$category\_alphabetic_$_$g_fileextension"))
		{

			$upcase=$_;
			$upcase=~ tr/abcdefghijklmnopqrstuvwxyz/ABCDEFGHIJKLMNOPQRSTUVWXYZ/;


			$g_filehandle=CMDINDEX;

			print_header("$doctitle: $_");
			print_tablestart();
			print_title("$doctitle: $_","");
			print_tableend();

			print_tablestart();
			print_body($alllinks);
			print_tableend();

			print_tablestart();

			$i=0;
			$j=0;
			if($number > 0)
			{
				#print("NUMBER > 0,COUNT = $count\n");
				while($i < $count)
				{
					($cmd,$short,$link) = split("<!>",$sortedCommands[$i]);
					#$left=substr($cmd,0,$number);
					#$right=$cmd;
					#$right=~ s/^$left//;
					$left = substr($cmd,0,$number);
					$right = substr($cmd,$number);
					$link =~ s/([a-zA-Z_]*)/\L\1/g;


					#print("RIGHT=$right,_=$_,LEFT=$left,UPCASE=$upcase\n");

					if(($right =~ /^$_/) or ($right =~ /^$upcase/) or ($right eq $_))
					{
						$j++;
						print_twocolumnbody("<a href=\"$link\">$left$right</a>",$short);
					}
					$i++;
				}

			} else {
				#print("NUMBER == 0,COUNT = $count\n");
				while($i < $count)
				{
					($cmd,$short,$link) = split("<!>",$sortedCommands[$i]);
					$link =~ s/([a-zA-Z_]*)/\L\1/g;
					if(($cmd =~ /^$_/) || ($cmd =~ /^$upcase/))
					{
						$j++;
						print_twocolumnbody("<a href=\"$link\">$cmd</a>",$short);
					}
					$i++;
				}
			}

			if($j == 0)
			{
				print_body("No matches");
			}

			print_tableend();

			print_tablestart();
			print_body($alllinks);
			print_tableend();

			print_footer();

			close(CMDINDEX);
		} else {
			print "Can't open $g_directory/doc_$category\_alphabetic_$_$g_fileextension for writing\n";
		}
	}
}

#################################################################################################
# MAIN
#################################################################################################

# Force flusing of STDOUT
$|=1;

print "--\n";
print "-- Generating documentation, this may take a while :)\n";
print "--\n";
print "-- Extracting keyterms\n";
# Extract the keywords to generate the crossreferences
$i = 0;

while($g_filesToProcess[$i] ne "")
{
	print ".";
	extract_keyterms($g_filesToProcess[$i]);
	$i++;
}

$g_files=$i - 1;

print "\n";
print "-- Extracting documents and generating crossreferences\n";

# Sort them
@g_keytermsSorted = sort {length($b) <=> length($a)} keys(%g_keyterms);

# Process the files now
$i = 0;

while($g_filesToProcess[$i] ne "")
{
	print ".";
	process_file($g_filesToProcess[$i]);
	$i++;
}

print "\n";
print "-- Generating indexes\n";

print ".";
generate_indexes("Commands","command",0);
print ".";
generate_indexes("Functions","function",1);
print ".";
generate_indexes("Modules","module",0);
print ".";
generate_indexes("Classes","class",0);
print ".";
generate_indexes("Events","event",2);
print ".";
generate_indexes("Language Documentation","language",0);
print ".";
generate_indexes("Features","widget",0);
print ".";
generate_indexes("Misc. Documentation","generic",0);
print ".";
generate_indexes("Keyterms & Concepts","keyterms",0);
print ".";

if(open(DOCINDEX,">$g_directory/index$g_fileextension"))
{
	$g_filehandle=DOCINDEX;
	print_header("Documentation Index");
	print_tablestart();
	print_twocolumntitle("Index","");
	print_twocolumnsubtitle("Fundamentals");
	print_twocolumnbody("<a href=\"doc_ircintro$g_fileextension\">Introduction to IRC</a>","A \"must read\" for beginners");
	print_twocolumnbody("<a href=\"doc_kvircintro$g_fileextension\">Introduction to KVIrc</a>","A couple of words about KVIrc");

	print_twocolumnsubtitle("Scripting Concepts: The KVS Manual");
	print_twocolumnbody("<a href=\"doc_kvs_introduction$g_fileextension\">Introduction to KVS</a>","Introduction to the KVIrc Scripting Language");
	print_twocolumnbody("<a href=\"doc_kvs_basicconcepts$g_fileextension\">Basic KVS Concepts</a>","The first steps in the KVS world");
	print_twocolumnbody("<a href=\"doc_kvs_aliasesandfunctions$g_fileextension\">Aliases and Functions</a>","How to write aliases/functions");
	print_twocolumnbody("<a href=\"doc_kvs_datatypes$g_fileextension\">Variables</a>","Which types of variables are available and how to handle them");
	print_twocolumnbody("<a href=\"doc_operators$g_fileextension\">Operators</a>","Describes simple operations with variables");
	print_twocolumnbody("<a href=\"doc_events$g_fileextension\">Events</a>","How to handle network events in KVS ?");
	print_twocolumnbody("<a href=\"doc_objects$g_fileextension\">Objects</a>","Object oriented scripting");
	print_twocolumnbody("<a href=\"doc_kvs_addons$g_fileextension\">Addons</a>","How to write nice addons for KVIrc");
	print_twocolumnbody("<a href=\"doc_kvs_codingtips$g_fileextension\">Coding Tips</a>","Some tips that may help you");
	print_twocolumnbody("<a href=\"doc_language_index_all$g_fileextension\">All the language documents</a>","All the documents related to KVS");

	print_twocolumnsubtitle("Scripting Reference");
	print_twocolumnbody("<a href=\"doc_command_alphabetic_a$g_fileextension\">Commands</a>","The listing of available commands");
	print_twocolumnbody("<a href=\"doc_function_alphabetic_a$g_fileextension\">Functions</a>","The listing of available functions");
	print_twocolumnbody("<a href=\"doc_event_alphabetic_a$g_fileextension\">Events</a>","The listing of available events");
	print_twocolumnbody("<a href=\"doc_class_index_all$g_fileextension\">Object Classes</a>","The listing of available object classes");
	print_twocolumnsubtitle("Other Documents");
	print_twocolumnbody("<a href=\"doc_keyboard$g_fileextension\">Keyboard shortcuts</a>","Map of the global keyboard shortcuts");
	print_twocolumnbody("<a href=\"doc_module_index_all$g_fileextension\">Modules</a>","Documentation related to specific modules");
	print_twocolumnbody("<a href=\"doc_widget_index_all$g_fileextension\">Features</a>","&nbsp;");
	print_twocolumnbody("<a href=\"doc_generic_index_all$g_fileextension\">Miscellaneous</a>","Misc documentation that didn't find any other place");
	print_twocolumnbody("<a href=\"doc_keyterms_index_all$g_fileextension\">Keyterms</a>","The (long) listing of all the keyterms");
	print_tableend();
	print_footer();

	close(DOCINDEX);
} else {
	print "Can't open $g_directory/index$g_fileextension for writing\n";
}

if(open(NOHELP,">$g_directory/nohelpavailable$g_fileextension"))
{
	$g_filehandle=NOHELP;
	print_header("No help available");

	print_tablestart();
	print_title("No help available","");
	print_tableend();

	print $g_filehandle "<center><h3>Sorry :(<br>Unfortunately there is no documentation related to the item you have selected</h3><br>\n";
	print $g_filehandle "Please try a different search term or take a look at the <a href=\"index$g_fileextension\">documentation index</a> ";
	print $g_filehandle "and try to locate the topic manually.</center><br><br>";
	print_footer();
	close(NOHELP);
} else {
	printf "Can't open $g_directory/nohelpavailable$g_fileextension for writing\n";
}

print "\n";
print "--\n";
print "-- Done! (Processed $g_files files)\n";
print "--\n";