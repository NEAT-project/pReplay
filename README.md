./dParser Input

Input: Http traces (Json files) collected using Wprof tool.
Traces contain dependency graph of a web page.

dParser replays the dependency graph. It starts from the 
first activity which is loading the root html. Then 
according to the graph, it it encounters a network activity, 
it makes a request for the corresponding url, or if it 
encounters a computation activity, it waits for a specific 
amount of time (mentioned in the graph). Once a particular 
activity is finshed it checks  whether it should trigger 
dependent activities based on whether all activities 
that a dependent activity depends on are finished. 
dParser keeps walking through the dependency graph this way until 
all activities in the dependency graph have been visited. 

