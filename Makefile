all: clean bomba centro

#bomba (cliente)
bomba: bomba.o errores.o extra.o
	-gcc -pedantic -pthread bomba.o errores.o extra.o -o bomba
	echo ""

bomba.o: bomba.c errores.h extra.h
	gcc -c -g -pedantic -pthread bomba.c

#centro (servidor)
centro: centro.o errores.o extra.o
	-gcc -pedantic -pthread centro.o errores.o extra.o -o centro
	echo ""

centro.o: centro.c errores.h extra.h
	gcc -c -g -pedantic -pthread centro.c

#procedimientos extra
extra.o: extra.c extra.h
	gcc -c -g -pedantic extra.c

#manejo de errores
errores.o: errores.c errores.h
	gcc -c -g -pedantic errores.c

clean:
	-rm *.o bomba centro
	echo ""
	clear