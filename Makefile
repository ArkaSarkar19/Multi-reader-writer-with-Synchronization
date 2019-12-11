all : main.c 
	gcc -o main main.c -lpthread && gcc -o rw rw.c -lpthread
clean :
	
	rm main rw

