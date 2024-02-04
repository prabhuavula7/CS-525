test_assign1: test_assign1_1.o test.o dberror.o
	gcc -o test_assign1 test_assign1_1.o test.o dberror.o
	
test_assign1_1.o: test_assign1_1.c
	gcc -c test_assign1_1.c
	
storage_mgr.o: test.c
	gcc -c test.c

dberror.o: dberror.c
	gcc -c dberror.c
	
run: test_assign1
	./test_assign1
	
clean:
	-rm test_assign1 test_assign1_1.o test.o dberror.o