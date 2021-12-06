target:
	@exit 0

pont1:
	gcc -lpthread -o pont1.o pont1.c && ./pont1.o

pont2:
	gcc -lpthread -o pont2.o pont2.c && ./pont2.o