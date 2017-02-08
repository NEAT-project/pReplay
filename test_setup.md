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
* **rtt** : 0, 10, 20, 50, 100, 200 ms
* **plr** : 0, 3 %
* **# connections/streams** : 1, 6, 18
* **repititions** : 30

## Machine setup
```
client <<--->> dummynet router <<--->> server
```
