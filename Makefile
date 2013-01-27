all: bomba centro

#bomba (cliente)
bomba: bomba.o errores.o
	gcc -pedantic -pthread bomba.o errores.o -o bomba

bomba.o: bomba.c errores.h
	gcc -c -g -pedantic -pthread bomba.c

#centro (servidor)
centro: centro.o errores.o
	gcc -pedantic -pthread centro.o errores.o -o centro

centro.o: centro.c errores.h
	gcc -c -g -pedantic -pthread centro.c

#manejo de errores
errores.o: errores.c errores.h
	gcc -c -g -pedantic errores.c

clean:
	rm *.o bomba centro