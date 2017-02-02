#!/usr/bin/perl

$server="10.0.4.1";   
$client="10.0.3.1";
$rctrl="root\@192.168.60.178";
$sctrl="root\@192.168.60.170";

system("ssh $sctrl sysctl net.inet.tcp.hostcache.expire=1");
system("ssh $sctrl sysctl net.inet.tcp.hostcache.purge=1");
system("ssh $sctrl  sysctl net.inet.tcp.hostcache.purgenow=1");

system("ssh $sctrl  sysctl net.inet.tcp.cc.algorithm=newreno");

system("ssh $sctrl sysctl net.inet.tcp.rfc1323=1");
system("ssh $sctrl sysctl net.inet.tcp.sendspace=1864135");
system("ssh $sctrl sysctl net.inet.tcp.recvspace=1864135");
system("ssh $sctrl sysctl net.inet.tcp.recvbuf_auto=1");
system("ssh $sctrl sysctl net.inet.tcp.tso=0");

#SCTP settings
system("ssh $sctrl sysctl net.inet.sctp.initial_cwnd=10");
system("ssh $sctrl sysctl net.inet.sctp.maxburst=10");
system("ssh $sctrl sysctl net.inet.sctp.heartbeat_interval=3000000");

#To run the thttpd server
#system("ssh $sctrl /usr/local/sbin/thttpd -C /usr/local/www/thttpd_config");
