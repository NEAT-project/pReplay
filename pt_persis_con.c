/* <DESC>
 * 6 persistent connections
 * </DESC>
 */
 
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <pthread.h>
#include <curl/curl.h>

pthread_mutex_t lock;

int download=0;

//static const char *urls[]={
char *urls[]={
	"http://example.com/",
	"http://example.com/",
	"http://example.com/",
	"http://example.com/",
	"http://example.com/",
	"http://example.com/",
	"http://example.com/",
	"http://example.com/",
	"http://example.com/",
	"http://example.com/"
};

struct memory_chunk {
        char *memory;
        size_t size;
        int enabled;
};

#define  MAX_CON 6

CURL *easy[MAX_CON];

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
    easy[i] = curl_easy_init();
    if (!easy[i])
        return -1;
    if((res = curl_easy_setopt(easy[i], CURLOPT_TCP_NODELAY, 1L)) != CURLE_OK) {
		fprintf(stderr, "cURL option error: %s\n", curl_easy_strerror(res));
	}
	


	/* some servers don't like requests that are made without a user-agent
	   field, so we provide one */
	if((res = curl_easy_setopt(easy[i], CURLOPT_USERAGENT, "get-http/0.1")) != CURLE_OK) {
		fprintf(stderr, "cURL option error: %s\n", curl_easy_strerror(res));
	}
  }
  return 0;
    	
}

void *run_worker(void *url)
{
	
	printf("URL: %s \n",(char *)url);
	double bytes;
	long header_bytes;
	double transfer_time;
	/*int res,i;
	
	CURL *curl;
	curl=curl_easy_init();*/
	
	int i;
	long response_code;
	CURL *curl;
	int idle_worker_found=0;
	for (i=0;i<MAX_CON;i++)
		printf("%d ",worker_status[i]);
	printf("\n");
	
	pthread_mutex_lock(&lock);
	while(1)
		{
		for(i=0; i<MAX_CON; i++) {
		//printf("I am here ");
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
	
	/*if(!curl)
		return -1;*/
	
	/* send all data to this function  */
	if((res = curl_easy_setopt(easy[i], CURLOPT_WRITEFUNCTION, memory_callback)) != CURLE_OK) {
		fprintf(stderr, "cURL option error: %s\n", curl_easy_strerror(res));
	}


	/* we pass our 'chunk' struct to the callback function */
	if((res = curl_easy_setopt(easy[i], CURLOPT_WRITEDATA, (void *)&chunk)) != CURLE_OK) {
		fprintf(stderr, "cURL option error: %s\n", curl_easy_strerror(res));
	}

	
	chunk.memory = NULL;
	chunk.size = 0;
	chunk.enabled = 0;
	//char *p="http://example.com";
	
	curl_easy_setopt(easy[i], CURLOPT_URL, url);
	chunk.size = 0;
	
	if((res=curl_easy_perform(easy[i])) != CURLE_OK){
		printf("i= %d\n",i);
		perror("Curl error");
	}
	else{
		if((res = curl_easy_getinfo(easy[i], CURLINFO_SIZE_DOWNLOAD, &bytes)) != CURLE_OK ||
				(res = curl_easy_getinfo(easy[i], CURLINFO_HEADER_SIZE, &header_bytes)) != CURLE_OK ||
				(res = curl_easy_getinfo(easy[i], CURLINFO_TOTAL_TIME, &transfer_time)) != CURLE_OK ) {
				fprintf(stderr, "cURL error: %s\n", curl_easy_strerror(res));
			}
		printf("Time: %f Size: %lu, Thread no: %d\n", transfer_time, (long)bytes+header_bytes,i);
		 
	}
	//pthread_mutex_lock(&lock);
	worker_status[i]=0;
	//pthread_mutex_unlock(&lock);
	
	return NULL;
}

int global_array_sum()
{
	int i, sum=0;
	for ( i=0; i< MAX_CON; i++){
		sum+=worker_status[i];
	}
	return sum;
}


int main()
{
	
	int i;
	pthread_t tid;
    int error;
    
    if (pthread_mutex_init(&lock, NULL) != 0) {
        printf("\n mutex init failed\n");
        return 1;
    }
    
	init_worker();

	for(i=0; i<10; i++) {
		
		  while(1){
			  if (global_array_sum()<6){ 
		  error = pthread_create(&tid,
                           NULL, // default attributes please 
                           run_worker,
                           (void *)urls[i]);
				if(0 != error)
					fprintf(stderr, "Couldn't run thread number %d, errno %d\n", i, error);
				break;	
				}
		  }	 
	}	 
	    sleep(5);
				
    
	pthread_mutex_destroy(&lock);

    /*int used_node=0;
    pthread_t tid;
    int error; 
	
	for(i=0;i<9;i++){
		if (i>used_node) i--;
	
			  if (download<6){
				used_node++;
				error = pthread_create(&tid,
                           NULL, // default attributes please 
                           run_worker,
                           (void *)urls[i]);
				if(0 != error)
					fprintf(stderr, "Couldn't run thread number %d, errno %d\n", i, error);
				download++;
				printf ("I am %d download\n", download);
			}
		
	    }
    
    sleep(2);*/

	return 0;
}
