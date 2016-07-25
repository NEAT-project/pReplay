SRC=  http_traces_replay_max_con6.c cJSON.c
HDRS=cJSON.h
CPLR=gcc

all:pReplay6

pReplay6: $(SRC) Makefile
	$(CPLR) $(SRC) -lm -lpthread -lcurl -o $@
debug: $(SRC) Makefile
	$(CPLR) -g $(SRC) -lm -lpthread -lcurl -o $@
	

clean:
	rm -rf pReplay6	 
