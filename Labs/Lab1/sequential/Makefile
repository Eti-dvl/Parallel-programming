PROJECTS_LIST= 1-memory_organization 2-integrate 3-fractals

all: 
	for project in $(PROJECTS_LIST); do echo "Compiling $$project"; (cd $$project; make); done
 
mem: 
	(cd 1-memory_organization; make)
 
run_mem: 
	(cd 1-memory_organization; make run)
 
time_mem: 
	(cd 1-memory_organization; make time)
 
integrate: 
	(cd 2-integrate; make)
 
run_integrate: 
	(cd 2-integrate; make run)
 
time_integrate: 
	(cd 2-integrate; make time)
 
fractals: 
	(cd 3-fractals; make)
 
run_fractals: 
	(cd 3-fractals; make run)
 
time_fractals: 
	(cd 3-fractals; make time)
  
clean: 
	for project in $(PROJECTS_LIST); do echo "Compiling $$project"; (cd $$project; make clean); done