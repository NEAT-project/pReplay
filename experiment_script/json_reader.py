import os, json
#import pandas as pd

path_to_json = 'temp_json/'
json_files = [pos_json for pos_json in os.listdir(path_to_json) if pos_json.endswith('json')]
for js in json_files:
    if (os.stat(path_to_json+js).st_size!=0):
    	with open(os.path.join(path_to_json, js)) as json_file:
        	data=json.load(json_file)
		print data["url"],data["num_objects"],data["page_size"], data["RTT"], data["PLR"], \
				data["CSZ"], data["NC"],data["PROTOCOL"],data["COUNT"],data["PLT"]

