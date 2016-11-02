#!/usr/bin/perl
print("Hallo\n");
$ENV{HTTP_USE_PIPELINING}='NO';
system("./pReplay bsd3.nplab.de tests/bsd3.json phttpget");	

$ENV{HTTP_USE_PIPELINING}='YES';
$ENV{HTTP_SCTP_MAX_STREAMS}='1988';
system("./pReplay bsd3.nplab.de tests/bsd3.json phttpget");
