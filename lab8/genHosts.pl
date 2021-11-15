#!/usr/bin/perl

# Author: Chris Wieringa
# Date: 2019-09-17
#
# Changelog: 
#  2020-08-21 - add goldvm01 - goldvm10 to the list
#  2019-09-17 - rewritten to be 'smarter' when finding hosts; actually test
#   them for rsh login ability before adding them to the list.
#   Additionally checks for valid .rhosts files
#  2019-09-24 - updated home to use ENV{HOME} instead of assuming /home/ + 
#    ENV{HOME}
#  2021-10-11 - edited the script to always print the host machines name first
# 
# Edited by JACK WESTEL in 2021 for CS 374.
# 
# Assisted by Hyechan Jun and Duncan Van Keulen. They're really smart.
#
# Hopefully this will save you guys some time :)

use Socket;
use Net::Ping;
use List::Util 'shuffle';
use threads;
use strict;
use Sys::Hostname; # This sets 'hostname' to be the name of the machine the program is run on.
use List::MoreUtils 'first_index';

# # # # # # #
# VARIABLES
# # # # # # #
my $maxtimeout = 10;

# Stores the hostname of whatever PC this is being run on.
# I set it to a variable cause Perl is scary and I didn't want to reference something that didn't have a variable tag.
my $permhost = hostname;

# # # # # # #
# MAIN Program
# # # # # # #

# check for proper .rhosts file
my $rhostsfile = sprintf("%s/.rhosts",$ENV{HOME});
if(!(-f $rhostsfile)) { die "Your $rhostsfile does not exist!\nPlease setup .rhosts before running genHosts.pl\n"; }
my $mode = (stat($rhostsfile))[2];
my $modeformatted = sprintf("%04o",$mode & 07777);
if($modeformatted != "0600") { die "Your $rhostsfile has incorrect permissions ($modeformatted)!\nPlease change the .rhosts file permissions to 0600.\n"; }

# read in all the hosts in the .rhosts file
my %rhosts;
open(FILE,$rhostsfile);
foreach my $line (<FILE>) {
  chomp($line);  if(!(defined($rhosts{$line}))) { $rhosts{$line} = 0; }
}
close(FILE);

# check all the hosts in the rhosts file; sping off individual threads
foreach my $host (keys %rhosts) {
  threads->create({'context' => 'scalar'},'threadHost',$host); 
}

# array to store randomized hostnames
my @randomized;

# collect all the threads, and create the usablehosts array
my @usablehosts;
my $iterations = 0;
while(threads->list() && $iterations < $maxtimeout) {
  $iterations++;
  sleep(1);

  # check all joinable threads
  foreach my $jthread (threads->list(threads::joinable)) {
    my $msg = $jthread->join();
    if(defined($msg) && $msg ne "") { push(@usablehosts,$msg); }
  }
}

# assume that after 10 seconds, all other threads will fail, so...
foreach my $kthread (threads->list()) {
  $kthread->kill('KILL')->detach;
}

# randomize the usablehosts and store it to @randomized
# This used to print the randomized array.
foreach my $host (shuffle(@usablehosts)) {
  chomp($host);
  push (@randomized, $host);
}

# In theory, this removes the host name from the randomized array and then places it at the beginning
# It's not a theory anymore, it just does it.
my $index = first_index { $_ eq $permhost } @randomized;
splice(@randomized, $index, 1);
unshift @randomized, $permhost;
foreach my $host (@randomized) {
  chomp($host);
  printf("%s\n",$host);
}

# # # # # #
# Subroutines
# # # # # #

# threadHost($hostname) - process to check each host
# Returns: the hostname if available, else nothing
sub threadHost {
  my $host = shift;
  return if $host eq "";

  # thread deal with SIG{KILL}
  local $SIG{KILL} = sub { threads->exit };

  # verify host can be looked up via dns and isn't nonsense
  my $ip; my $a = inet_aton($host);
  if(defined($a)) { $ip = inet_ntoa($a); }
  unless($ip =~ /153\.106/) { return; }

  # ping the host via rsh service
  if(pingHostRsh($host) && checkHostRsh($host)) { return $host; }
  else { return; }
}

# pingHostRsh($hostname) - ping the given host via tcp, checking the rsh port
# Returns: 1 if pingable, 0 otherwise
sub pingHostRsh {
  my $host = shift;
  return 0 if $host eq "";

  # do the ping
  my $p = Net::Ping->new("tcp",3);
  $p->{port_num} = 514;  # rsh
  $p->service_check(1);
  return $p->ping($host);
}

# checkHostRsh($hostname) - check that rsh is responding appropriately
# Returns: 1 if pingable, 0 otherwise
sub checkHostRsh {
  my $host = shift;
  return 0 if $host eq "";

  # do the check
  my $output = `rsh $host "mount | grep /home | grep katz" 2>&1`;
  if($output =~ /katz/) { return 1; }
  else { return 0; }
}
