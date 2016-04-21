/* The following program is 
used to parse dependency 
trees based page load process 
in todays browser

written by-- Mohd Rajiullah*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>
#include <ctype.h>
#include <pthread.h>
#include "cJSON.h"

void createActivity(char *job_id);
cJSON *json, *this_objs_array, *this_acts_array,*map_start, *map_complete;
struct timeval start;
pthread_mutex_t lock;



static int cJSON_strcasecmp(const char *s1,const char *s2)
{
        if (!s1) return (s1==s2)?0:1;if (!s2) return 1;
	/* QUESTION: is this below statements to compare the first letter? */
        for(; tolower(*s1) == tolower(*s2); ++s1, ++s2) if(*s1 == 0)    return 0;
        return tolower(*(const unsigned char *)s1) - tolower(*(const unsigned char *)s2);
}

int cJSON_HasArrayItem(cJSON *array, const char *string)
{
	int i;
	char *out, c1, c2;

	pthread_mutex_lock(&lock);
	for(i=0; i<cJSON_GetArraySize(array); i++)
		{
		cJSON * obj= cJSON_GetArrayItem(array, i);
		if (!obj) {printf("Error before: [%s]\n",cJSON_GetErrorPtr());}
		out=cJSON_Print(obj);
		char *C1=malloc(20);
		C1=strtok(out,":");
		char * C2=C1+4;
		C2[strlen(C2)-1]=0;
		if(cJSON_strcasecmp(C2,string)==0){
			free(C1);
			pthread_mutex_unlock(&lock);
			return i;
		}
		free(C1);
	}
	pthread_mutex_unlock(&lock);

	return -1;
}

int checkDependedActivities(char *id)
{
	int is_all_complete=1;
	int i=cJSON_HasArrayItem(this_acts_array,id);
	int j;
	if(i!=-1){
		cJSON * objs= cJSON_GetArrayItem(this_acts_array, i);
		cJSON * obj= cJSON_GetObjectItem(objs, id);
		cJSON *deps=cJSON_GetObjectItem(obj,"deps");
		for(j=0; j<cJSON_GetArraySize(deps); j++){
			cJSON * dep= cJSON_GetArrayItem(deps, j);
			if(cJSON_GetObjectItem(dep,"time")->valueint < 0){
				if(cJSON_HasObjectItem(map_complete,cJSON_GetObjectItem(dep,"id")->valuestring)){
					if(cJSON_GetObjectItem(map_complete,cJSON_GetObjectItem(dep,"id")->valuestring)->valueint!=1){
						is_all_complete=0;
						break;
					}
				}
				else{
						is_all_complete=0;
						break;
				}
			}
			else{
				if(cJSON_HasObjectItem(map_start,cJSON_GetObjectItem(dep,"id")->valuestring)){
					if(cJSON_GetObjectItem(map_start,cJSON_GetObjectItem(dep,"id")->valuestring)->valueint!=1){
						is_all_complete=0;
						break;
					}
				}
				else{
						is_all_complete=0;
						break;
				}
			}

		}

	}
	return is_all_complete;
}

void onComplete(cJSON *obj_name)
{
	struct timeval te;
	int i;
	gettimeofday(&te,NULL);
	double end_time=((te.tv_sec-start.tv_sec)*1000+(double)(te.tv_usec-start.tv_usec)/1000);
	cJSON_AddNumberToObject(obj_name,"ts_e",end_time);

	printf("=== [onComplete][%f] {\"id\":%s,\"type\":%s,\"is_started\":%d,\"ts_s\":%f,\"ts_e\":%f}\n",end_time,cJSON_GetObjectItem(obj_name,"id")->valuestring,cJSON_GetObjectItem(obj_name,"type")->valuestring,cJSON_GetObjectItem(obj_name,"is_started")->valueint,cJSON_GetObjectItem(obj_name,"ts_s")->valuedouble,cJSON_GetObjectItem(obj_name,"ts_e")->valuedouble);


	// TO DO update task completion maps
	if(!cJSON_HasObjectItem(map_complete,cJSON_GetObjectItem(obj_name,"id")->valuestring))
		cJSON_AddNumberToObject(map_complete,cJSON_GetObjectItem(obj_name,"id")->valuestring,1);
	
	// Check whether should trigger dependent activities when 'time' == -1	
	if(cJSON_HasObjectItem(obj_name,"triggers")){
		cJSON *triggers=cJSON_GetObjectItem(obj_name,"triggers");
		for(i=0; i<cJSON_GetArraySize(triggers); i++){
			cJSON *trigger=cJSON_GetArrayItem(triggers,i);
			if(cJSON_GetObjectItem(trigger,"time")->valueint==-1){
				// Check whether all activities that trigger.id depends on are finished
				if (checkDependedActivities(cJSON_GetObjectItem(trigger,"id")->valuestring))
					createActivity(cJSON_GetObjectItem(trigger,"id")->valuestring);
			}
		}
	}

}

void setTimeout(int ms)
{
	struct timeval ts,te;
	gettimeofday(&ts,NULL);
	printf("Timeout starts: %f ms \n",((ts.tv_sec-start.tv_sec)*1000+(double)(ts.tv_usec-start.tv_usec)/1000));
	usleep(ms*1000);
	gettimeofday(&te,NULL);
	printf("Timeout ends: %f ms \n",((te.tv_sec-start.tv_sec)*1000+(double)(te.tv_usec-start.tv_usec)/1000));

	
}

void *compActivity(void *arg)
{
	cJSON *obj_name= arg;
	//printf("Time out value: %d\n",cJSON_GetObjectItem(obj_name,"time")->valueint);
	setTimeout(cJSON_GetObjectItem(obj_name,"time")->valueint);
	onComplete(obj_name);
	return ((void*)0);
}

void *createActivityAfterTimeout(void *arg)
{
	cJSON *trigger= arg;
	setTimeout(cJSON_GetObjectItem(trigger,"time")->valueint);
	createActivity(cJSON_GetObjectItem(trigger,"id")->valuestring);
	return ((void*)0);
}


void createActivity(char *job_id)
{
	pthread_t tid1, tid2;


	int i=cJSON_HasArrayItem(this_acts_array,job_id);
	if(i!=-1){
		cJSON * obj= cJSON_GetArrayItem(this_acts_array, i);
		cJSON * obj_name=cJSON_GetObjectItem(obj,job_id);
		if(cJSON_HasObjectItem(obj_name,"is_started")){
			if(cJSON_GetObjectItem(obj_name,"is_started")->valueint==1)
				return;
			else
				cJSON_GetObjectItem(obj_name,"is_started")->valueint=1;
		}
		else
			{
				cJSON_AddNumberToObject(obj_name,"is_started",1);
		}

	
	struct timeval ts_s;
	gettimeofday(&ts_s, NULL);
	cJSON_AddNumberToObject(obj_name,"ts_s",((ts_s.tv_sec-start.tv_sec)*1000+(double)(ts_s.tv_usec-start.tv_usec)/1000));
	printf("Object id: %s, type: %s started at %f ms\n",cJSON_GetObjectItem(obj_name,"obj_id")->valuestring,cJSON_GetObjectItem(obj_name,"type")->valuestring,((ts_s.tv_sec-start.tv_sec)*1000+(double)(ts_s.tv_usec-start.tv_usec)/1000));
	
	if(cJSON_strcasecmp(cJSON_GetObjectItem(obj_name,"type")->valuestring,"download")==0){
		i=cJSON_HasArrayItem(this_objs_array,cJSON_GetObjectItem(obj_name,"obj_id")->valuestring);
		if(i!=-1){
			obj= cJSON_GetArrayItem(this_objs_array, i);
			cJSON * this_obj= cJSON_GetObjectItem(obj, cJSON_GetObjectItem(obj_name, "obj_id")->valuestring);
			printf("Host: %s\n",cJSON_GetObjectItem(this_obj,"host")->valuestring);
			printf("Path: %s\n",cJSON_GetObjectItem(this_obj,"path")->valuestring);
			printf("when_comp_start: %d\n",cJSON_GetObjectItem(this_obj,"when_comp_start")->valueint);
			onComplete(obj_name);


		}
	}
	else {
	// For comp activity
		//setTimeout(cJSON_GetObjectItem(obj_name,"time")->valueint);
		//onComplete(obj_name);
		pthread_create(&tid1,NULL , compActivity, (void *) obj_name);
	}
	// TO DO update task start maps
	if(!cJSON_HasObjectItem(map_start,cJSON_GetObjectItem(obj_name,"id")->valuestring))
		cJSON_AddNumberToObject(map_start,cJSON_GetObjectItem(obj_name,"id")->valuestring,1);
	
	// Check whether should trigger dependent activities when 'time' != -1	
	if(cJSON_HasObjectItem(obj_name,"triggers")){
		cJSON *triggers=cJSON_GetObjectItem(obj_name,"triggers");
		for(i=0; i<cJSON_GetArraySize(triggers); i++){
			cJSON *trigger=cJSON_GetArrayItem(triggers,i);
			if(cJSON_GetObjectItem(trigger,"time")->valueint!=-1){
				// Check whether all activities that trigger.id depends on are finished
				if(checkDependedActivities(cJSON_GetObjectItem(trigger,"id")->valuestring)){
					//createActivityAfterTimeout(trigger);
					pthread_create(&tid2,NULL , createActivityAfterTimeout, (void *) trigger);
					//setTimeout(cJSON_GetObjectItem(trigger,"time")->valueint);
					//createActivity(cJSON_GetObjectItem(trigger,"id")->valuestring);
				}
			}
		}
	}


	/*char *out;
	out=cJSON_Print(this_acts_array);
	printf("%s\n",out);*/

	}
	

}

void run()
{
	map_start=cJSON_CreateObject();
	map_complete=cJSON_CreateObject();
	gettimeofday(&start, NULL);
	createActivity(cJSON_GetObjectItem(json,"start_activity")->valuestring);
	sleep(4);
	//printf("start_activity:%s\n",cJSON_GetObjectItem(json,"start_activity")->valuestring); 
}


/* Parse text to JSON, then render back to text, and print! */
void doit(char *text)
{
   cJSON *temp_obj;
   json=cJSON_Parse(text);
   if (!json) {printf("Error before: [%s]\n",cJSON_GetErrorPtr());}


   cJSON *objs=cJSON_GetObjectItem(json,"objs");
   if (!objs) {printf("Error before: [%s]\n",cJSON_GetErrorPtr());}
   
   cJSON *deps=cJSON_GetObjectItem(json,"deps");
   if (!deps) {printf("Error before: [%s]\n",cJSON_GetErrorPtr());}
   
   int i,j;
   char *out, *a1, *a2;
   
   this_objs_array=cJSON_CreateArray();
   this_acts_array=cJSON_CreateArray();
   cJSON *this_objs, *temp1, *temp2, *this_acts, *comps, *root, *b1, *b2, *temp, *temp_array;
   
   for(i=0; i<cJSON_GetArraySize(objs); i++)
		{
		cJSON * obj= cJSON_GetArrayItem(objs, i);
		/* Handle array of comp */
	        comps=cJSON_GetObjectItem(obj,"comps");
      		if (!comps) {printf("Error before: [%s]\n",cJSON_GetErrorPtr());}
		
		/* Handle download */
		cJSON * download=cJSON_GetObjectItem(obj,"download");
        	cJSON_AddStringToObject(download, "obj_id",cJSON_GetObjectItem(obj,"id")->valuestring);
			cJSON_DeleteItemFromObject(download,"type");
			cJSON_AddStringToObject(download, "type","download");
        	cJSON_AddStringToObject(download, "mime","download");
		
		/* Index obj by object id */

		temp1=cJSON_CreateObject();
		cJSON_AddStringToObject(temp1,"id",cJSON_GetObjectItem(obj,"id")->valuestring);
		cJSON_AddStringToObject(temp1,"host",cJSON_GetObjectItem(obj,"host")->valuestring);
		cJSON_AddStringToObject(temp1,"path",cJSON_GetObjectItem(obj,"path")->valuestring);
		cJSON_AddNumberToObject(temp1,"when_comp_start",cJSON_GetObjectItem(obj,"when_comp_start")->valueint);
		cJSON_AddItemReferenceToObject(temp1,"download",download);
		cJSON_AddItemReferenceToObject(temp1,"comps",comps);

		this_objs=cJSON_CreateObject();
		
		cJSON_AddItemReferenceToObject(this_objs,cJSON_GetObjectItem(obj,"id")->valuestring,temp1);
		cJSON_AddItemReferenceToArray(this_objs_array,this_objs);
		
		
		this_acts=cJSON_CreateObject();
		cJSON_AddItemReferenceToObject(this_acts,cJSON_GetObjectItem(download,"id")->valuestring,download);
		cJSON_AddItemReferenceToArray(this_acts_array,this_acts);


	        
	
		/* printf("||||||\n");
		out=cJSON_Print(obj);
		printf("%s\n",out);
		printf("||||||\n"); */

		 for (j=0; j< cJSON_GetArraySize(comps); j++)
			{
			
	
			cJSON * comp= cJSON_GetArrayItem(comps, j);
			cJSON_AddStringToObject(comp, "obj_id",cJSON_GetObjectItem(obj,"id")->valuestring);
			 /*Add natural dependency
      			   Depends on download activity
      			   Depends on its previous comp activity*/
		 	 if (j==0){
				a1=cJSON_GetObjectItem(download,"id")->valuestring;
        		  }
			else{
				a1=cJSON_GetObjectItem(cJSON_GetArrayItem(comps, j-1),"id")->valuestring;
			}
			 root=cJSON_CreateObject();
        		 int deps_length=cJSON_GetArraySize(deps);
			 char dep_id[6];
			 sprintf(dep_id,"dep%d",deps_length+1);
        		 cJSON_AddStringToObject(root,"id",dep_id);	
        		 cJSON_AddStringToObject(root,"a1",a1);	
        		 cJSON_AddStringToObject(root,"a2",cJSON_GetObjectItem(comp,"id")-> valuestring);	
			 cJSON_AddNumberToObject(root,"time",-1);
			 cJSON_AddItemReferenceToArray(deps,root);
			 //out=cJSON_Print(deps);
			
			 temp2=cJSON_CreateObject();
		          
                         
	
			 cJSON_AddStringToObject(temp2,"id",cJSON_GetObjectItem(comp,"id")->valuestring);
			 

			if (cJSON_GetObjectItem(comp,"type")->valuestring!=NULL)
				cJSON_AddStringToObject(temp2,"type",cJSON_GetObjectItem(comp,"type")->valuestring);
			else
				cJSON_AddNumberToObject(temp2,"type",cJSON_GetObjectItem(comp,"type")->valueint);
			
			 //printf("%s\n",a1);
			
			 cJSON_AddNumberToObject(temp2,"time",cJSON_GetObjectItem(comp,"time")->valueint);
			 cJSON_AddStringToObject(temp2,"obj_id",cJSON_GetObjectItem(obj,"id")->valuestring);
			
			 this_acts=cJSON_CreateObject();
                		
		         cJSON_AddItemReferenceToObject(this_acts,cJSON_GetObjectItem(comp,"id")->valuestring,temp2);
		         cJSON_AddItemReferenceToArray(this_acts_array,this_acts);
			

			
		 }
	       
		
		}
     
		


    /* Add dependencies to activities */
    for (i=0; i< cJSON_GetArraySize(deps); i++){
	cJSON * dep= cJSON_GetArrayItem(deps,i);
    
	
	
	for (j=0; j< cJSON_GetArraySize(this_acts_array); j++){
		cJSON * obj= cJSON_GetArrayItem(this_acts_array, j);
		
		if (!obj) {printf("Error before: [%s]\n",cJSON_GetErrorPtr());}
		

		 out=cJSON_Print(obj);
		 //printf("%s\n",out);
		
	 	 char *Cidd=malloc(20);
		 //sprintf(Cid,"%.9s",out);
		 Cidd=strtok(out,":");
		 char* Cid=Cidd+4; 
		 Cid[strlen(Cid)-1] = 0;
		
   
		 char *pch = strstr(Cid,cJSON_GetObjectItem(dep,"a1")->valuestring);
	
     		
		 if(pch && (strlen(Cid)==strlen(cJSON_GetObjectItem(dep,"a1")->valuestring))){
			b1= cJSON_GetObjectItem(obj,cJSON_GetObjectItem(dep,"a1")->valuestring);
		}
                
		
		char *pch1 = strstr(Cid, cJSON_GetObjectItem(dep,"a2")->valuestring);
		 
		if(pch1 &&  (strlen(Cid)==strlen(cJSON_GetObjectItem(dep,"a2")->valuestring))){
			b2= cJSON_GetObjectItem(obj,cJSON_GetObjectItem(dep,"a2")->valuestring);
			}	
		free(Cidd);
	}
	
	

 
	
	/* Add to a2 that 'a2 depends on a1' */

	temp=cJSON_CreateObject();
	temp_array=cJSON_CreateArray();
	cJSON_AddStringToObject(temp,"id",cJSON_GetObjectItem(b1,"id")->valuestring);

	

	cJSON_AddNumberToObject(temp,"time",cJSON_GetObjectItem(dep,"time")->valueint);

	
	
	
	if (cJSON_HasObjectItem(b2,"deps"))
		cJSON_AddItemReferenceToArray(cJSON_GetObjectItem(b2,"deps"),temp);
	else{
		cJSON_AddItemToArray(temp_array,temp);		
		cJSON_AddItemReferenceToObject(b2,"deps",temp_array);
	}

	

	temp=cJSON_CreateObject();
	temp_array=cJSON_CreateArray();
	cJSON_AddStringToObject(temp,"id",cJSON_GetObjectItem(b2,"id")->valuestring);
	cJSON_AddNumberToObject(temp,"time",cJSON_GetObjectItem(dep,"time")->valueint);	
	
	

	if (cJSON_HasObjectItem(b1,"triggers"))
		cJSON_AddItemReferenceToArray(cJSON_GetObjectItem(b1,"triggers"),temp);
	else{
		cJSON_AddItemToArray(temp_array,temp);
		cJSON_AddItemReferenceToObject(b1,"triggers",temp_array);
		
	}

	

	
	
    }

      printf("===[objects]");
	out=cJSON_Print(this_objs_array);
	printf("%s\n",out);	
	printf("===[activities]");
	out=cJSON_Print(this_acts_array);
	printf("%s\n",out);
   

	run();
	
}




/* Read a file, parse, render back, etc. */
void dofile(char *filename)
{
	FILE *f;long len;char *data;
	
	f=fopen(filename,"rb");
	if (f == NULL)
   		 {
        	perror("Error opening file\n");
		exit(0);
   	}
	fseek(f,0,SEEK_END);len=ftell(f);fseek(f,0,SEEK_SET);
	data=(char*)malloc(len+1);fread(data,1,len,f);fclose(f);
	doit(data);
	free(data);
}


int main (int argc, char * argv[]) {
	

	if(argc !=2){
		printf("usage: %s filename\n",argv[0]);
		exit(0);
	}

	if (pthread_mutex_init(&lock, NULL) != 0) {
		printf("\n mutex init failed\n");
		return 1;
	}

	dofile(argv[1]); 

	pthread_mutex_destroy(&lock);



	
	return 0;
}
