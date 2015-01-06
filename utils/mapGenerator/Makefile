CC = g++
OBJECTS = main.o MapFileIO.o MapGenerator.o Map.o MapFileHeader.o


mapGenerator:	$(OBJECTS)
	$(CC) $(OBJECTS) -o mapGenerator

%.o:	%.c
	$(CC) -c $<

clean:
	rm mapGenerator *.o
