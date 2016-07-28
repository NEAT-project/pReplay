SRC=parseDependencyGraph.c cJSON.c
HDRS=cJSON.h
CPLR=cc
CPFL=-std=c99 -pedantic -Wall -Wextra -Werror -Wno-unused-function -Wno-unused-variable -D_POSIX_C_SOURCE=199309L
BNAME=pReplay

pReplay: $(SRC) Makefile
	$(CPLR) $(CPFL) $(SRC) -lm -lpthread -lcurl -o $(BNAME)
debug: $(SRC) Makefile
	$(CPLR) $(CPFL) -g -O0 $(SRC) -lm -lpthread -lcurl -o $(BNAME)
clean:
	rm -rf $(BNAME)
