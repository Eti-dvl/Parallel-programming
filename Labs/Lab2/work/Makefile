PROJECTS_LIST= 1-* 2-* 3-*

all: 
	for project in $(PROJECTS_LIST); do echo "Compiling $$project"; (cd $$project; make); done
 
sum: 
	(cd 1-*; make)
 
run_sum: 
	(cd 1-*; make run)
 
primes: 
	(cd 2-*; make)
 
run_primes: 
	(cd 2-*; make run)
 
time_primes: 
	(cd 2-*; make time)
 
prod_cons: 
	(cd 3-*; make)
 
run_prod_cons: 
	(cd 3-*; make run)
 
time_prod_cons: 
	(cd 3-*; make time)
  
clean: 
	for project in $(PROJECTS_LIST); do echo "Compiling $$project"; (cd $$project; make clean); done
