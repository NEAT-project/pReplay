#!/usr/bin/perl



$server="10.0.4.1";   
$client="10.0.3.1";
$rctrl="root\@192.168.1.114";
$sctrl="root\@192.168.1.141";



# --- Transport layer initialization ---
# These values are the default TCP
# values.
#
# Caching is disabled though
# 
system("sudo sysctl -w net.ipv4.tcp_no_metrics_save=1");
system("ssh $sctrl sudo sysctl -w net.ipv4.tcp_no_metrics_save=1");
system("sudo sysctl -w net.ipv4.tcp_reordering=3");
system("ssh $sctrl sudo sysctl -w net.ipv4.tcp_reordering=3");
system("sudo sysctl -w net.ipv4.tcp_congestion_control=cubic");
system("ssh $sctrl sudo sysctl -w net.ipv4.tcp_congestion_control=cubic");
system("sudo sysctl -w net.ipv4.tcp_timestamps=1");
system("ssh $sctrl sudo sysctl -w net.ipv4.tcp_timestamps=1");
system("sudo sysctl -w net.ipv4.tcp_dsack=1");
system("ssh $sctrl sudo sysctl -w net.ipv4.tcp_dsack=1");
system("sudo sysctl -w net.core.optmem_max=16777216");
system("ssh $sctrl sudo sysctl -w net.core.optmem_max=16777216");
system("sudo sysctl -w net.core.rmem_default=16777216");
system("ssh $sctrl sudo sysctl -w net.core.rmem_default=16777216");
system("sudo sysctl -w net.core.rmem_max=16777216");
system("ssh $sctrl sudo sysctl -w net.core.rmem_max=16777216");
system("sudo sysctl -w net.core.wmem_max=16777216");
system("ssh $sctrl sudo sysctl -w net.core.wmem_max=16777216");
system("sudo sysctl -w net.core.wmem_default=16777216");
system("ssh $sctrl sudo sysctl -w net.core.wmem_default=16777216");
system("sudo sysctl -w net.ipv4.tcp_mem='16777216 16777216 16777216'");
#system("ssh $sctrl sudo sysctl -w net.ipv4.tcp_mem='16777216 16777216 16777216'");
system("sudo sysctl -w net.ipv4.tcp_rmem='16777216 16777216 16777216'");
#system("ssh $sctrl sudo sysctl -w net.ipv4.tcp_rmem='16777216 16777216 16777216'");
system("sudo sysctl -w net.ipv4.tcp_wmem='16777216 16777216 16777216'");
#system("ssh $sctrl sudo sysctl -w net.ipv4.tcp_wmem='16777216 16777216 16777216'");

#Disable SS after idle
system("sudo sysctl -w net.ipv4.tcp_slow_start_after_idle=0");
system("ssh $sctrl sudo sysctl -w net.ipv4.tcp_slow_start_after_idle=0");


# Disable offloading to get sensible dumps
system("sudo ethtool -K enp2s0 gso off tso off gro off lro off");
system("ssh $sctrl sudo ethtool -K eth1 gso off tso off gro off lro off");


# --- Experimental framework initialization ---

# Type of experiment (sysctl controlled vars)
# None for these experiments

$replications=1;
$outfilnamn="/root/test_logs/test_";

#$writesyslog=1;
$log_filnamn=$outfilnamn."log";

# Reset indices
$tcidx=0;
$lossidx=0;
$testcount=0;

# Log this file
system ("cat $0 >> $log_filnamn");

# Print experiment header
open (FDR, ">>$outfilnamn");
print FDR "Nr    BandwDown  BandwUp   Del Que Fsize  DrpCnt  Lossdiff  Time   \n";
close (FDR);

# Remove previous application instances
#system ("ssh $sctrl sudo killall tcp-source");
system ("ssh $sctrl sudo killall tcpdump");

# Add experimental routes


# --- Experiment parameters ---

# Emulated bandwidth (down and up)
#@bwdown=("5Mbit/s");
#@bwup=("5Mbit/s");
@bwdown=("0");
@bwup=("0");

# Emulated end-to-end delay
@delay=(10,40,97);
#@delay=(10,40,97,400);
#@delay=(10);
@plr=(0,0.03);
#@plr=(0,.015,0.03);
#@plr=(0);
@no_connects=(1,6,18);
#@no_connects=(1,6,18);
#@no_connects=(1);
@cookie_size=(0,512,2048);
#@cookie_size=(0,512,2048);
#@cookie_size=(0);

# Queue size used in network emulator
$queue=100;

$patternprefix="delay";

$packets=1;
$size=$packets;
$ckon=0;

$dir = "tree_folder/";
opendir(BIN, $dir) or die "Can't open $dir: $!";

while( $file=readdir BIN ){
         push @array,$file;
}

@array = @array[ 2 .. $#array ];


$replikat=1;


# --- Experiment execution ---

$tcidx=0;
while ($tcidx < @bwdown) {
	foreach $delay (@delay) {
	 foreach $plr (@plr) {
	  foreach $no_connects (@no_connects) {
	    foreach $cookie_size (@cookie_size) {
		foreach $array (@array){
			#print "$delay $plr $no_connects $cookie_size\n";
			$repcount=$replications;
			$testcount=0;
			while ($repcount--) {

				system("sudo sysctl -w net.ipv4.tcp_moderate_rcvbuf=1");
				system("ssh $sctrl sudo sysctl -w net.ipv4.tcp_moderate_rcvbuf=1");

				# Remove old emulation settings 
	  			system("ssh $rctrl ipfw -f flush");
	 			system("ssh $rctrl ipfw -f pipe flush");
 	  			system("ssh $rctrl ipfw add drop icmp from any to any out icmptypes 4");

				# Create new emulation pipes 
	  			system("ssh $rctrl ipfw add 3 pipe 200 tcp from $client to $server in");
	  			system("ssh $rctrl ipfw add 4 pipe 300 tcp from $server to $client in");

				# Configure new emulation settings
				system("ssh $rctrl ipfw pipe 200 config bw $bwdown[$tcidx] delay $delay queue $queue");
	 			system("ssh $rctrl ipfw pipe 300 config bw $bwup[$tcidx] delay $delay  plr $plr queue $queue");
	 			#system("ssh $rctrl ipfw pipe 300 config bw $bwup[$tcidx] delay $delay plr 0.2 queue $queue");
	
	  			if ($writesyslog==1) {
	    				open (FDR, ">>$log_filnamn");
		    			print FDR "\n=========================\n";
		    			print FDR "testnr:$testcount  $bwdown[$tcidx]-$bwup[$tcidx]  $delay ms   $queue packets \n";
	    				print FDR "Size:$size Loss=$packlfile lossdiff:$ckon \n========================\n\n";
	    				open (SOUT,"ssh $rctrl ipfw pipe show|");
	    				@tmp=<SOUT>;
		    			print FDR @tmp;
		    			close (SOUT);
	    				close (FDR);    
					system("date >> $log_filnamn");
	  			}
	  
				open (FDR, ">>$outfilnamn");
	  			printf FDR ("%4i %10s %10s %3d %3d %3d     %s      %1d \n ",$testcount,$bwdown[$tcidx],$bwup[$tcidx],$delay,$queue,$size,$plr,$ckon); 
	  			close (FDR);    

				# Log traffic at server, pause, and ping a little :)
 	  			system("ssh -f $sctrl sudo tcpdump -n -i eth1 -s 0 -U -w /tmp/temp.pcap  src host 10.0.3.1 or dst host 10.0.3.1 &");
	#			system("sleep 5");
		#		system("ping -c 10 $server");
		#		system("ssh $sctrl ping -c 10 $client");
				system("sleep 5");

				# Start logging and execution of experiment at the client
				$rtt=$delay*2;
                                $outfilname="/root/test_log/".'results_rtt.'.$rtt.'_plr.'.$plr.'_cookie_sz'.$cookie_size.'_no_coonects'.$no_connects.'_site:'.$array.'_';


				system("sudo tcpdump -n -i enp2s0 -U -s 0 -w /tmp/temp.pcap  src host 10.0.3.1 or dst host 10.0.3.1 &");
				system("sleep 1");

				system("./pReplay $server tree_folder/$array http $no_connects  $cookie_size > $outfilname");
				
				system("sudo killall tcpdump");
				system("sudo gzip -f /tmp/temp.pcap");
				system("cp /tmp/temp.pcap.gz ".$outfilname.$testcount.".pcap.gz");
				system("ssh $sctrl sudo killall tcpdump");
				system("sleep 1");
				system("ssh -f $sctrl sudo gzip -f /tmp/temp.pcap");
				system("sleep 1");
				#system("ssh -f $sctrl cp /tmp/temp.pcap.gz ".$outfilname."srv".$testcount.".pcap.gz");
				system("scp $sctrl:/tmp/temp.pcap.gz ".$outfilname."srv".$testcount.".pcap.gz");
	
	  			# Do detailed logging of router statistics
				if ($writesyslog==1) {
					open (FDR, ">>$log_filnamn");
					open (SOUT,"ssh $rctrl ipfw pipe show|");
					@tmp=<SOUT>;
					print FDR @tmp;
					close (SOUT);
					close (FDR);    
				}
				$testcount++;
			}
		 }
		}
	    }
          }
	}
	$tcidx++;
}

system("sleep 1");
