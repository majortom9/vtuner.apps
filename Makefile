CFLAGS += -fPIC -DHAVE_DVB_API_VERSION=5 $(DBGFLAGS)
LDFLAGS += -lpthread -lrt
DRIVER = vtuner-dvb-3

default: vtunerd vtunerc

vtuner-dvb-3.o: vtuner-dvb-3.c vtuner-dvb-3.h
	$(CC) $(CFLAGS) -c -o vtuner-dvb-3.o vtuner-dvb-3.c

vtunerd: vtunerd.c vtunerd-service.o vtuner-network.o vtuner-utils.o $(DRIVER).o
	$(CC) $(CFLAGS) -DBUILDVER="\"$(LOCVER)\"" -DMODFLAG=\"$(MODFLAG)\" -o vtunerd vtuner-network.o vtunerd-service.o $(DRIVER).o vtuner-utils.o vtunerd.c $(LDFLAGS)

vtunerc: vtunerc.c vtuner-network.o vtuner-utils.o
	$(CC) $(CFLAGS) -DBUILDVER="\"$(LOCVER)\"" -DMODFLAG=\"$(MODFLAG)\" -o vtunerc vtuner-network.o vtuner-utils.o vtunerc.c $(LDFLAGS)

vtunerd-service.o: vtunerd-service.c vtunerd-service.h
	$(CC) $(CFLAGS) -c -o vtunerd-service.o vtunerd-service.c

vtuner-network.o: vtuner-network.c vtuner-network.h
	$(CC) $(CFLAGS) -c -o vtuner-network.o vtuner-network.c

vtuner-utils.o: vtuner-utils.c vtuner-utils.h
	$(CC) $(CFLAGS) -c -o vtuner-utils.o vtuner-utils.c

clean:
	rm -rf *.o vtunerd vtunerc
