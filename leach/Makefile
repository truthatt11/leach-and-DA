TARGET=leach-fixed leach-txman leach-txman-multi leach-proposal
FLAG=-g

all: mt19937-64.o leach.o fixed.o txman.o txman-multi.o proposal.o
	gcc leach.o mt19937-64.o fixed.o       -o leach-fixed -lm
	gcc leach.o mt19937-64.o txman.o       -o leach-txman -lm
	gcc leach.o mt19937-64.o txman-multi.o -o leach-txman-multi -lm
	gcc leach.o mt19937-64.o proposal.o    -o leach-proposal -lm
leach.o: leach.c
	gcc leach.c $(FLAG) -c -o leach.o
mt19937-64.o: ../mt19937-64.c
	gcc ../mt19937-64.c $(FLAG) -c -o mt19937-64.o
fixed.o: fixed.c
	gcc fixed.c $(FLAG) -c -o fixed.o
txman.o: txman.c
	gcc txman.c $(FLAG) -c -o txman.o
txman-multi.o: txman-multi.c
	gcc txman-multi.c $(FLAG) -c -o txman-multi.o
proposal.o: proposal.c
	gcc proposal.c $(FLAG) -c -o proposal.o

.PHONY: clean
clean:
	rm *.o $(TARGET)
