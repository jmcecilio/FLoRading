# dragino lora testing
# Single lora testing app

CC=g++
CFLAGS=-c -lm
LIBS=-lwiringPi -lgps

all: lora_gateway lora_node

lora_gateway: gateway.o draginoLora.o gps_hat.o checksum.o
	$(CC) gateway.o draginoLora.o gps_hat.o checksum.o $(LIBS) -o lora_gateway

gateway.o: gateway.c draginoLora.h lora.h gps_hat.h checksum.h
	$(CC) $(CFLAGS) gateway.c

lora_node: node.o draginoLora.o gps_hat.o checksum.o
	$(CC) node.o draginoLora.o gps_hat.o checksum.o $(LIBS) -o lora_node

node.o: node.c draginoLora.h lora.h gps_hat.h checksum.h
	$(CC) $(CFLAGS) node.c

draginoLora.o: draginoLora.c draginoLora.h lora.h
	$(CC) $(CFLAGS) draginoLora.c

gps_hat.o: gps_hat.c gps_hat.h
	$(CC) $(CFLAGS) gps_hat.c

checksum.o: checksum.c checksum.h
	$(CC) $(CFLAGS) checksum.c

clean:
	rm *.o lora_gateway lora_node	