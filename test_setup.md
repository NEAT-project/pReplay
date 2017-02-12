# Experiment setup

## Parameter set
* **pages**
  * www.google.com.hk_.json
  * www.dmm.co.jp_.json
  * www.siteadvisor.com_.json
  * www.amazon.co.jp_.json
  * pinterst.com_.json
  * www.mediafire.com_.json
* **bandwidth** : 10 Mbps
* **rtt** : 0, 20, 50, 100, 200, 800 ms
* **plr** : 0, 1.5, 3 %
* **# connections/streams** : 1, 6, 18
* **repetitions** : 30
* **no think time**
* **no cookie size*
* **TCP: pReplay + curl**
* **SCTP: pReplay + phttpget**

## Software
* thttpd
 * https://github.com/nplab/thttpd/tree/multistream
 * branch: **multistream**
* pReplay
 * https://github.com/NEAT-project/pReplay
 * branch: **master**
* phttpget
 * https://github.com/NEAT-project/HTTPOverSCTP
 * branch: **multistream**

## Machine setup
```
client (Linux) <<--->> dummynet router (FreeBSD) <<--->> server (FreeBSD)
```
### FHM
All machines are running the OS bare metal.
* client
 * Ubuntu 16.10
* dummynet router
 * FreeBSD HEAD (Feb 2017)
 * Kernel: GENERIC-NODEBUG
* server
 * FreeBSD HEAD (Feb 2017)
 * Kernel: GENERIC-NODEBUG
