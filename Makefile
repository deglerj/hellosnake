CC = /opt/gbdk/bin/lcc -Wa-l -Wl-m -Wl-j

default:
	$(CC) -c -o hellosnake.o hellosnake.c
	$(CC) -o hellosnake.gb hellosnake.o

clean:
	rm -f *.o *.lst *.map *.gb *~ *.rel *.cdb *.ihx *.lnk *.sym *.asm
