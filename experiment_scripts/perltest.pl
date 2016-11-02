#!/usr/bin/perl
print("Hallo\n");
$ENV{HTTP_USE_PIPELINING}='NO';
system("./pReplay bsd3.nplab.de tests/bsd3.json phttpget");
