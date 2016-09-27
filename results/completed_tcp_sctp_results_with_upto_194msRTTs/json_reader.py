import os, json
import sys
import matplotlib.pyplot as plt 

params = {
   'axes.labelsize': 12,
   'text.fontsize': 12,
   'legend.fontsize': 12,
   'xtick.labelsize': 10,
   'ytick.labelsize': 10,
   'text.usetex': False,
   'figure.figsize': [4.5, 4.5]
   }
plt.rcParams.update(params)

tcp1=[]
tcp6=[]
tcp18=[]
sctp1=[]

plt1_1=0
plt6_1=0
plt18_1=0
plts1_1=0

plt1_2=0
plt6_2=0
plt18_2=0
plts1_2=0

plt1_3=0
plt6_3=0
plt18_3=0
plts1_3=0


count=0

rtts=[20,80,194]

path_to_json = sys.argv[1]+'/'
json_files = [pos_json for pos_json in os.listdir(path_to_json) if pos_json.endswith('json')]
for js in json_files:
 if (os.stat(path_to_json+js).st_size!=0):
  with open(os.path.join(path_to_json, js)) as json_file:
   data=json.load(json_file)
   #print (data["url"],data["RTT"],data["PLR"],data["CSZ"],data["NC"],data["PROTOCOL"],data["COUNT"],data["PLT"])

   if data["url"]==sys.argv[2] and data["PLR"]==float(sys.argv[3]) and data["CSZ"]==int(sys.argv[4]):
    if data["RTT"]==20 and data["PROTOCOL"]=="http" and data["NC"]==1:
     plt1_1+=data["PLT"]  

    if data["RTT"]==20 and data["PROTOCOL"]=="http" and data["NC"]==6:
     plt6_1+=data["PLT"]  
    if data["RTT"]==20 and data["PROTOCOL"]=="http" and data["NC"]==18:
     plt18_1+=data["PLT"]  
    if data["RTT"]==20 and data["PROTOCOL"]=="phttpget":
     plts1_1+=data["PLT"]  

    if data["RTT"]==80 and data["PROTOCOL"]=="http" and data["NC"]==1:
     plt1_2+=data["PLT"]
    if data["RTT"]==80 and data["PROTOCOL"]=="http" and data["NC"]==6:
     plt6_2+=data["PLT"]  
    if data["RTT"]==80 and data["PROTOCOL"]=="http" and data["NC"]==18:
     plt18_2+=data["PLT"]  
    if data["RTT"]==80 and data["PROTOCOL"]=="phttpget":
     plts1_2+=data["PLT"]  

    if data["RTT"]==194 and data["PROTOCOL"]=="http" and data["NC"]==1:
     plt1_3+=data["PLT"]  
    if data["RTT"]==194 and data["PROTOCOL"]=="http" and data["NC"]==6:
     plt6_3+=data["PLT"]  
    if data["RTT"]==194 and data["PROTOCOL"]=="http" and data["NC"]==18:
     plt18_3+=data["PLT"]  
    if data["RTT"]==194 and data["PROTOCOL"]=="phttpget":
     plts1_3+=data["PLT"]  
   
tcp1.append(plt1_1/5000)
tcp6.append(plt6_1/5000)
tcp18.append(plt18_1/5000)
sctp1.append(plts1_1/5000)
tcp1.append(plt1_2/5000)
tcp6.append(plt6_2/5000)
tcp18.append(plt18_2/5000)
sctp1.append(plts1_2/5000)
tcp1.append(plt1_3/5000)
tcp6.append(plt6_3/5000)
tcp18.append(plt18_3/5000)
sctp1.append(plts1_3/5000)
plt.plot(rtts, tcp1, marker='o', color='red', linewidth=2.5, linestyle='-', label="1 TCP")
plt.plot(rtts, tcp6, marker='o', color='blue', linewidth=2.5, linestyle='-', label="6 TCPs")
plt.plot(rtts, tcp18, marker='o', color='#aa0000', linewidth=2.5, linestyle='-', label="18 TCPs")
plt.plot(rtts, sctp1, marker='o', color='black', linewidth=2.5, linestyle='-', label="1 multistreaming SCTP")

plt.xlabel('RTT [ms]')
plt.ylabel('Web load time [s]')
plt.title('Site: '+ sys.argv[2]+ ' PLR: '+sys.argv[3]+' % CSZ: '+sys.argv[4]+' B')

plt.legend(frameon=False)
#plt.show()
plt.savefig('Site_'+ sys.argv[2]+'_PLR_'+sys.argv[3]+'_CSZ_'+sys.argv[4]+'.pdf',format='pdf')

 
