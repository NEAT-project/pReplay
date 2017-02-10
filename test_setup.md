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

## Machine setup
```
client (Linux) <<--->> dummynet router (FreeBSD) <<--->> server (FreeBSD)
```
