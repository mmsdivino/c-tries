# Regra default para criar o programa
all: tries Makefile.uptodate

# Compilando o programa
tries.o: tries.c
	gcc -g -Wall -c tries.c

# Ligando o programa
tries: tries.o
	gcc -g tries.o -o a.out -ggdb

# Limpando tudo o que foi gerado automaticamente pelo comando clean
Makefile.uptodate: Makefile
	make clean
clean:
	rm -rf *.o
