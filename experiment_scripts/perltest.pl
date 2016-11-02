#!/usr/bin/perl
print("Hallo\n");
$ENV{FELIX}='HSDKFS';
system("./pReplay bsd3.nplab.de tests/bsd3.json phttpget");
