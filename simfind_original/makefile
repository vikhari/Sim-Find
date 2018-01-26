PROJECT_SRC = ./src/main.c ./src/fnv.c src/hashing.c src/bloomfilter.c src/fingerprint.c src/fingerprintList.c src/helper.c


NAME=mrsh

all: debug

debug: ${PROJECT_SRC} ${PROJECT_HDR}
	gcc -w -ggdb -std=c99 -D_BSD_SOURCE -lcrypto -o ${NAME} ${PROJECT_SRC} -lm

mrsh: ${PROJECT_SRC} ${PROJECT_HDR}
	gcc -w -std=c99 -O3 -D_BSD_SOURCE -lcrypto -o ${NAME} ${PROJECT_SRC} -lm

#pg for profiler, gprof. 

clean :  
	rm -f mrsh *.o 

#for DT_DIR feature to work, need to have the _BSD_SOURCE  feature test macro defined. THese are not standard, and GCC does not define the macro when compiling for C99
# -lm: -l means link a library and -m means a math library. Without this option 


