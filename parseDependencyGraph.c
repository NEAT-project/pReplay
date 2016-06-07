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

#define HTTP1 1
#define HTTP2 2
#define HTTPS 3

int debug=0;
double page_load_time;
unsigned long page_size=0;
int json_output=1;
int object_count=0;

void createActivity(char *job_id);
cJSON *json, *this_objs_array, *this_acts_array,*map_start, *map_complete;
struct timeval start;
pthread_mutex_t lock;

/* curl stuff */
#include <curl/curl.h>

#ifndef CURLPIPE_MULTIPLEX
/* This little trick will just make sure that we don't enable pipelining for
   libcurls old enough to not have this symbol. It is _not_ defined to zero in
   a recent libcurl header. */
#define CURLPIPE_MULTIPLEX 0
#endif

#define NUM_HANDLES 1000

void *curl_hnd[NUM_HANDLES];
CURL *easy[NUM_HANDLES];
CURLM *multi_handle;
int num_transfers;
int protocol;

int request_count=0;
void onComplete(cJSON *obj_name);

struct memory_chunk {
        char *memory;
        size_t size;
        int enabled;
};

#define  MAX_CON 6

CURL *easyh1[MAX_CON];

int worker_status[MAX_CON]={0,0,0,0,0,0};


static size_t memory_callback(void *contents, size_t size, size_t nmemb, void *userp)
{
	size_t realsize = size * nmemb;
	struct memory_chunk *chunk = userp;
	if(!chunk->enabled) return realsize;

	chunk->memory = realloc(chunk->memory, chunk->size + realsize + 1);
	if(chunk->memory == NULL) {
		perror("realloc");
		return 0;
	}

	memcpy(&(chunk->memory[chunk->size]), contents, realsize);
	chunk->size += realsize;
	chunk->memory[chunk->size] = 0;
	return realsize;
}

int init_worker()
{
  int i, res;
  for(i=0; i<MAX_CON; i++) {
    easyh1[i] = curl_easy_init();
    if (!easyh1[i])
        return -1;
    if((res = curl_easy_setopt(easyh1[i], CURLOPT_TCP_NODELAY, 1L)) != CURLE_OK) {
		fprintf(stderr, "cURL option error: %s\n", curl_easy_strerror(res));
	}
	


	/* some servers don't like requests that are made without a user-agent
	   field, so we provide one */
	if((res = curl_easy_setopt(easyh1[i], CURLOPT_USERAGENT, "get-http/0.1")) != CURLE_OK) {
		fprintf(stderr, "cURL option error: %s\n", curl_easy_strerror(res));
	}
  }
  return 0;
    	
}

int init_tls_worker()
{
  int i, res;
  for(i=0; i<MAX_CON; i++) {
    easyh1[i] = curl_easy_init();
    if (!easyh1[i])
        return -1;
    if((res = curl_easy_setopt(easyh1[i], CURLOPT_TCP_NODELAY, 1L)) != CURLE_OK ||
       (res = curl_easy_setopt(easyh1[i], CURLOPT_HEADER, 0L)) != CURLE_OK ||
       (res = curl_easy_setopt(easyh1[i], CURLOPT_SSL_VERIFYPEER, 0L)) != CURLE_OK ||
       (res = curl_easy_setopt(easyh1[i], CURLOPT_SSL_VERIFYHOST, 0L)) != CURLE_OK
    ) {
		fprintf(stderr, "cURL option error: %s\n", curl_easy_strerror(res));
	}
	


	/* some servers don't like requests that are made without a user-agent
	   field, so we provide one */
	if((res = curl_easy_setopt(easyh1[i], CURLOPT_USERAGENT, "get-http/0.1")) != CURLE_OK) {
		fprintf(stderr, "cURL option error: %s\n", curl_easy_strerror(res));
	}
  }
  return 0;    	
}


void *run_worker(void *arg)
{
	cJSON *obj_name=arg;
	char url[400];
	int j=cJSON_HasArrayItem(this_objs_array,cJSON_GetObjectItem(obj_name,"obj_id")->valuestring);
	
	if(j!=-1){
		cJSON *obj= cJSON_GetArrayItem(this_objs_array, j);
			cJSON * this_obj= cJSON_GetObjectItem(obj, cJSON_GetObjectItem(obj_name, "obj_id")->valuestring);
			if(protocol==HTTP1)
			snprintf(url, sizeof url,"%s%s","http://193.10.227.23",cJSON_GetObjectItem(this_obj,"path")->valuestring);
			if(protocol==HTTPS)
			snprintf(url, sizeof url,"%s%s","https://193.10.227.23",cJSON_GetObjectItem(this_obj,"path")->valuestring);
			
	int i;
	long response_code;
	CURL *curl;
	int idle_worker_found=0;
	
	pthread_mutex_lock(&lock);
	while(1)
		{
		for(i=0; i<MAX_CON; i++) {
			if(worker_status[i]==0){
				idle_worker_found=1;
				worker_status[i]=1;
				break;
			}
		}
		if (idle_worker_found==1)
			break;
	}
	
	pthread_mutex_unlock(&lock);
	
	struct memory_chunk chunk;
	int res;
	
	if((res = curl_easy_setopt(easyh1[i], CURLOPT_WRITEFUNCTION, memory_callback)) != CURLE_OK) {
		fprintf(stderr, "cURL option error: %s\n", curl_easy_strerror(res));
	}



	if((res = curl_easy_setopt(easyh1[i], CURLOPT_WRITEDATA, (void *)&chunk)) != CURLE_OK) {
		fprintf(stderr, "cURL option error: %s\n", curl_easy_strerror(res));
	}

	
	chunk.memory = NULL;
	chunk.size = 0;
	chunk.enabled = 0;
	
	//char *url1="http://example.com";
	
	curl_easy_setopt(easyh1[i], CURLOPT_URL, url);
	curl_easy_setopt(easyh1[i], CURLOPT_PRIVATE, url);
	chunk.size = 0;
	
	if((res=curl_easy_perform(easyh1[i])) != CURLE_OK){
		printf("i= %d\n",i);
		perror("Curl error");
	}

	worker_status[i]=0;
	 double bytes,avj_obj_size;
    long total_bytes;
    long header_bytes;
    double transfer_time;
    double end_time;
    struct timeval te;
    
    
    if((res = curl_easy_getinfo(easyh1[i], CURLINFO_SIZE_DOWNLOAD, &bytes)) != CURLE_OK ||
				(res = curl_easy_getinfo(easyh1[i], CURLINFO_HEADER_SIZE, &header_bytes)) != CURLE_OK ||
				(res = curl_easy_getinfo(easyh1[i], CURLINFO_TOTAL_TIME, &transfer_time)) != CURLE_OK ) {
				fprintf(stderr, "cURL error: %s\n", curl_easy_strerror(res));
			}
     
 
	gettimeofday(&te,NULL);
	page_load_time=end_time=((te.tv_sec-start.tv_sec)*1000+(double)(te.tv_usec-start.tv_usec)/1000);
	total_bytes=(long)bytes+header_bytes;
	page_size+=(long)bytes;
	
	object_count++;
	
    if(json_output==1)
		printf("{%ld,%f},\n",total_bytes, transfer_time);
	if (debug==1 && json_output==0)
		printf("[%f] Object_size: %ld, transfer_time: %f\n", end_time, (long)bytes+header_bytes, transfer_time);
	onComplete(obj_name);
}

}

int global_array_sum()
{
	int i, sum=0;
	for ( i=0; i< MAX_CON; i++){
		sum+=worker_status[i];
	}
	return sum;
}

/* a handle to number lookup, highly ineffective when we do many
   transfers... */
static int hnd2num(CURL *hnd)
{
	int i;
	for(i=0; i< num_transfers; i++) {
		if(curl_hnd[i] == hnd)
			return i;
	}
	return 0; /* weird, but just a fail-safe */
}
/*
static size_t write_data(void *ptr, size_t size, size_t nmemb, void *stream)
{
    printf ("Got it\n");
    return 1;
}
*/

static
void dump(const char *text, int num, unsigned char *ptr, size_t size,
		  char nohex)
{
	size_t i;
	size_t c;

	unsigned int width=0x10;

	if(nohex)
		// without the hex output, we can fit more on screen
		width = 0x40;

	fprintf(stderr, "%d %s, %ld bytes (0x%lx)\n",
			num, text, (long)size, (long)size);

	for(i=0; i<size; i+= width) {

		fprintf(stderr, "%4.4lx: ", (long)i);

		if(!nohex) {
			// hex not disabled, show it
			for(c = 0; c < width; c++)
				if(i+c < size)
					fprintf(stderr, "%02x ", ptr[i+c]);
				else
					fputs("   ", stderr);
		}

		for(c = 0; (c < width) && (i+c < size); c++) {
			// check for 0D0A; if found, skip past and start a new line of output
			if(nohex && (i+c+1 < size) && ptr[i+c]==0x0D && ptr[i+c+1]==0x0A) {
				i+=(c+2-width);
				break;
			}
			fprintf(stderr, "%c",
					(ptr[i+c]>=0x20) && (ptr[i+c]<0x80)?ptr[i+c]:'.');
			// check again for 0D0A, to avoid an extra \n if it's at width
			if(nohex && (i+c+2 < size) && ptr[i+c+1]==0x0D && ptr[i+c+2]==0x0A) {
				i+=(c+3-width);
				break;
			}
		}
		fputc('\n', stderr); // newline
	}
}

static
int my_trace(CURL *handle, curl_infotype type,
			 char *data, size_t size,
			 void *userp)
{
	const char *text;
	int num = hnd2num(handle);
	(void)handle; // prevent compiler warning
	(void)userp;
	switch (type) {
		case CURLINFO_TEXT:
			fprintf(stderr, "== %d Info: %s", num, data);
		default: // in case a new one is introduced to shock us
			return 0;

		case CURLINFO_HEADER_OUT:
			text = "=> Send header";
			break;
		case CURLINFO_DATA_OUT:
			text = "=> Send data";
			break;
		case CURLINFO_SSL_DATA_OUT:
			text = "=> Send SSL data";
			break;
		case CURLINFO_HEADER_IN:
			text = "<= Recv header";
			break;
		case CURLINFO_DATA_IN:
			text = "<= Recv data";
			break;
		case CURLINFO_SSL_DATA_IN:
			text = "<= Recv SSL data";
			break;
	}

	dump(text, num, (unsigned char *)data, size, 1);
	return 0;
}


static void setup(CURL *hnd, int num, char *url)
{
	FILE *out;
	char filename[128];

	snprintf(filename, 128, "dl-%d", num);

	out = fopen(filename, "wb");

	// write to this file
	curl_easy_setopt(hnd, CURLOPT_WRITEDATA, out);

	/* send all data to this function  */
	//curl_easy_setopt(hnd, CURLOPT_WRITEFUNCTION, write_data);

     

	/* set the same URL */
	curl_easy_setopt(hnd, CURLOPT_URL, url);

	/* send it verbose for max debuggaility */
	//curl_easy_setopt(hnd, CURLOPT_VERBOSE, 1L);
	//curl_easy_setopt(hnd, CURLOPT_DEBUGFUNCTION, my_trace);

	/* HTTP/2 please */
	curl_easy_setopt(hnd, CURLOPT_HTTP_VERSION, CURL_HTTP_VERSION_2_0);

	/* we use a self-signed test server, skip verification during debugging */
	curl_easy_setopt(hnd, CURLOPT_SSL_VERIFYPEER, 0L);
	curl_easy_setopt(hnd, CURLOPT_SSL_VERIFYHOST, 0L);

#if (CURLPIPE_MULTIPLEX > 0)
	/* wait for pipe connection to confirm */
	curl_easy_setopt(hnd, CURLOPT_PIPEWAIT, 1L);
#endif

	curl_hnd[num] = hnd;
}



void  *request_url(void * arg)
{
	cJSON *obj_name=arg;
	char url[400];
	int i=cJSON_HasArrayItem(this_objs_array,cJSON_GetObjectItem(obj_name,"obj_id")->valuestring);
	struct timeval te;
	double end_time;
	if(i!=-1){
			cJSON *obj= cJSON_GetArrayItem(this_objs_array, i);
			cJSON * this_obj= cJSON_GetObjectItem(obj, cJSON_GetObjectItem(obj_name, "obj_id")->valuestring);
			snprintf(url, sizeof url,"%s%s","https://193.10.227.23:8000",cJSON_GetObjectItem(this_obj,"path")->valuestring);
			if (debug==1 && json_output==0)
			printf("URL: %s\n",url);		
			//printf("when_comp_start--: %d\n",cJSON_GetObjectItem(this_obj,"when_comp_start")->valueint);
		


			gettimeofday(&te,NULL);
			end_time=((te.tv_sec-start.tv_sec)*1000+(double)(te.tv_usec-start.tv_usec)/1000);
			if (debug==1 && json_output==0)
				printf("[%f] URL: %s\n",end_time,url);
			int still_running; /* keep number of running handles */

			CURL *eh = curl_easy_init();
			/* set options */
			
			FILE *out;
			char filename[128];
			int num=0;

			snprintf(filename, 128, "dl-%d", num);

			out = fopen(filename, "wb");

			// write to this file
			curl_easy_setopt(eh, CURLOPT_WRITEDATA, out);

			/* send all data to this function  */
			//curl_easy_setopt(hnd, CURLOPT_WRITEFUNCTION, write_data);

             /*Nagle off please */
			curl_easy_setopt(eh, CURLOPT_TCP_NODELAY, 1L);

			/* set the same URL */
			curl_easy_setopt(eh, CURLOPT_URL, url);

			/* send it verbose for max debuggaility */
			//curl_easy_setopt(hnd, CURLOPT_VERBOSE, 1L);
			//curl_easy_setopt(hnd, CURLOPT_DEBUGFUNCTION, my_trace);

			/* HTTP/2 please */
			curl_easy_setopt(eh, CURLOPT_HTTP_VERSION, CURL_HTTP_VERSION_2_0);

			/* we use a self-signed test server, skip verification during debugging */
			curl_easy_setopt(eh, CURLOPT_SSL_VERIFYPEER, 0L);
			curl_easy_setopt(eh, CURLOPT_SSL_VERIFYHOST, 0L);

			#if (CURLPIPE_MULTIPLEX > 0)
			/* wait for pipe connection to confirm */
			curl_easy_setopt(eh, CURLOPT_PIPEWAIT, 1L);
			#endif

			/* add the individual transfer */
			curl_multi_add_handle(multi_handle, eh);


			curl_multi_setopt(multi_handle, CURLMOPT_PIPELINING, CURLPIPE_MULTIPLEX);
    
			pthread_mutex_lock(&lock);

			/* we start some action by calling perform right away */
			curl_multi_perform(multi_handle, &still_running);
			pthread_mutex_unlock(&lock);

			do {
        struct timeval timeout;
        int rc; /* select() return code */
        CURLMcode mc; /* curl_multi_fdset() return code */

        fd_set fdread;
        fd_set fdwrite;
        fd_set fdexcep;
        int maxfd = -1;

        long curl_timeo = -1;

        FD_ZERO(&fdread);
        FD_ZERO(&fdwrite);
        FD_ZERO(&fdexcep);

        /* set a suitable timeout to play around with */
        timeout.tv_sec = 1;
        timeout.tv_usec = 0;

        curl_multi_timeout(multi_handle, &curl_timeo);
        if(curl_timeo >= 0) {
            timeout.tv_sec = curl_timeo / 1000;
            if(timeout.tv_sec > 1)
                timeout.tv_sec = 1;
            else
                timeout.tv_usec = (curl_timeo % 1000) * 1000;
        }

        /* get file descriptors from the transfers */
        mc = curl_multi_fdset(multi_handle, &fdread, &fdwrite, &fdexcep, &maxfd);

        if(mc != CURLM_OK) {
            fprintf(stderr, "curl_multi_fdset() failed, code %d.\n", mc);
            break;
        }

        /* On success the value of maxfd is guaranteed to be >= -1. We call
           select(maxfd + 1, ...); specially in case of (maxfd == -1) there are
           no fds ready yet so we call select(0, ...) --or Sleep() on Windows--
           to sleep 100ms, which is the minimum suggested value in the
           curl_multi_fdset() doc. */

        if(maxfd == -1) {
#ifdef _WIN32
            Sleep(100);
      rc = 0;
#else
            /* Portable sleep for platforms other than Windows. */
            struct timeval wait = { 0, 100 * 1000 }; /* 100ms */
            rc = select(0, NULL, NULL, NULL, &wait);
#endif
        }
        else {
            /* Note that on some platforms 'timeout' may be modified by select().
               If you need access to the original value save a copy beforehand. */
            rc = select(maxfd+1, &fdread, &fdwrite, &fdexcep, &timeout);
        }

        switch(rc) {
            case -1:
                /* select error */
                break;
            case 0:
            default:
                /* timeout or readable/writable sockets */
                pthread_mutex_lock(&lock);
                curl_multi_perform(multi_handle, &still_running);
                pthread_mutex_unlock(&lock);
                break;
        }
    } while(still_running);

	
	 double bytes,avj_obj_size;
    long total_bytes;
    long header_bytes;
    double transfer_time;
    int res;
    
    if((res = curl_easy_getinfo(eh, CURLINFO_SIZE_DOWNLOAD, &bytes)) != CURLE_OK ||
				(res = curl_easy_getinfo(eh, CURLINFO_HEADER_SIZE, &header_bytes)) != CURLE_OK ||
				(res = curl_easy_getinfo(eh, CURLINFO_TOTAL_TIME, &transfer_time)) != CURLE_OK ) {
				fprintf(stderr, "cURL error: %s\n", curl_easy_strerror(res));
			}
     
 
	gettimeofday(&te,NULL);
	page_load_time=end_time=((te.tv_sec-start.tv_sec)*1000+(double)(te.tv_usec-start.tv_usec)/1000);
	total_bytes=(long)bytes+header_bytes;
	page_size+=(long)bytes;
	
	object_count++;
	
	if(json_output==1)
		printf("{%ld,%f},\n",total_bytes, transfer_time);
	if (debug==1 && json_output==0)
		printf("[%f] Object_size: %ld, transfer_time: %f\n", end_time, (long)bytes+header_bytes, transfer_time);
    onComplete(obj_name);
}

}


static int cJSON_strcasecmp(const char *s1,const char *s2)
{
        if (!s1) return (s1==s2)?0:1;if (!s2) return 1;
        for(; tolower(*s1) == tolower(*s2); ++s1, ++s2) if(*s1 == 0)    return 0;
        return tolower(*(const unsigned char *)s1) - tolower(*(const unsigned char *)s2);
}

/*int cJSON_HasArrayItem(cJSON *array, const char *string)
{
	int i;
	char *out;
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
		
	}
    pthread_mutex_unlock(&lock);

    return -1;
}*/

int cJSON_HasArrayItem(cJSON *array, const char *string)
{
	int i;
    for(i=0; i<cJSON_GetArraySize(array); i++)
		{
		cJSON * obj= cJSON_GetArrayItem(array, i);
		if(cJSON_HasObjectItem(obj,string)){
			return i;}
		}
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

	if (debug==1)
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
	//printf("Timeout starts: %f ms \n",((ts.tv_sec-start.tv_sec)*1000+(double)(ts.tv_usec-start.tv_usec)/1000));
	usleep(ms*1000);
	gettimeofday(&te,NULL);
	//printf("Timeout ends: %f ms \n",((te.tv_sec-start.tv_sec)*1000+(double)(te.tv_usec-start.tv_usec)/1000));

	
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
	char url[400];


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

	int error;
	struct timeval ts_s;
	gettimeofday(&ts_s, NULL);
	cJSON_AddNumberToObject(obj_name,"ts_s",((ts_s.tv_sec-start.tv_sec)*1000+(double)(ts_s.tv_usec-start.tv_usec)/1000));
	if (debug==1 && json_output==0)
	  printf("Object id: %s, type: %s started at %f ms\n",cJSON_GetObjectItem(obj_name,"obj_id")->valuestring,cJSON_GetObjectItem(obj_name,"type")->valuestring,((ts_s.tv_sec-start.tv_sec)*1000+(double)(ts_s.tv_usec-start.tv_usec)/1000));
	
	if(cJSON_strcasecmp(cJSON_GetObjectItem(obj_name,"type")->valuestring,"download")==0){
		i=cJSON_HasArrayItem(this_objs_array,cJSON_GetObjectItem(obj_name,"obj_id")->valuestring);
		if(i!=-1){
			obj= cJSON_GetArrayItem(this_objs_array, i);
			cJSON * this_obj= cJSON_GetObjectItem(obj, cJSON_GetObjectItem(obj_name, "obj_id")->valuestring);

            if(protocol==HTTP2)
				pthread_create(&tid2,NULL , request_url, (void *) obj_name);
			else if(protocol==HTTP1 || protocol==HTTPS){
				 while(1){
					if (global_array_sum()<MAX_CON){ 
					error = pthread_create(&tid2,NULL,run_worker,(void *)obj_name);
					if(0 != error)
						fprintf(stderr, "Couldn't run thread number %d, errno %d\n", i, error);
					break;	
				}
			}
			 
			}
			else {}	
			//request_count++;
			//onComplete(obj_name);


		}
	}
	else {
	// For comp activity
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
					pthread_create(&tid2,NULL , createActivityAfterTimeout, (void *) trigger);

				}
			}
		}
	}




	}
	

}

void run()
{
	struct timeval end;
	map_start=cJSON_CreateObject();
	map_complete=cJSON_CreateObject();
	if(protocol==HTTP1)
		init_worker();
	if(protocol==HTTPS)
		init_tls_worker();
	gettimeofday(&start, NULL);
	createActivity(cJSON_GetObjectItem(json,"start_activity")->valuestring);
	sleep(10);
	printf("],\"num_objects\":%d,\"PLT\":%f, \"page_size\":%ld}\n",object_count,page_load_time,page_size);
	
	
	//gettimeofday(&end, NULL);
	//double end_time=((end.tv_sec-start.tv_sec)*1000+(double)(end.tv_usec-start.tv_usec)/1000);
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
			 
			
			// cJSON_AddStringToObject(temp2,"type",cJSON_GetObjectItem(comp,"type")->valuestring);			
			//printf ("==%s==\n",cJSON_GetObjectItem(comp,"type")->valuestring);
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
		

		//printf("=I am here=\n");
		
		 // out=cJSON_Print(dep);
		// printf("%s\n",out);

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
    if(debug==1 && json_output==0) {
		printf("===[objects]");
		out=cJSON_Print(this_objs_array);
		printf("%s\n",out);	
		printf("===[activities]");
		out=cJSON_Print(this_acts_array);
		printf("%s\n",out);
	} 

   
   //printf("start_activity:%s\n",cJSON_GetObjectItem(json,"start_activity")->valuestring); 
		
    /*for(i=0; i<cJSON_GetArraySize(this_acts_array); i++)
		{
			cJSON * obj= cJSON_GetArrayItem(this_acts_array, i);
			printf("......\n");
			out=cJSON_Print(obj);
			printf("%s\n",out);
		}*/

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
	
	if(argc !=3){
		printf("usage: %s protocol(http1/s/2) filename\n",argv[0]);
		exit(0);
	}
	if(strcmp(argv[1],"http2")==0)
		protocol=HTTP2;
	else if(strcmp(argv[1],"http1")==0)
		protocol=HTTP1;
	else if(strcmp(argv[1],"https")==0)
		protocol=HTTPS;
	else{
		printf("Not supported protocol.\n");
		exit(0);
	}
	

	/* init a multi stack */
	multi_handle = curl_multi_init();
    if (pthread_mutex_init(&lock, NULL) != 0) {
        printf("\n mutex init failed\n");
        return 1;
    }
    
    if(json_output==1)
		printf("{\"url_file\": \"%s\",\"OLT\":[",strrchr(argv[2],'/')+1);
	dofile(argv[2]);

    pthread_mutex_destroy(&lock);

    return 0;
}
