all:
	g++ dn.cpp -o testoutput `pkg-config --cflags --libs opencv`
disarium:
	g++ dn.cpp -o dn -lgomp -fopenmp
ds:
	g++ dn.cpp -o dn -lgomp
bradley:
	g++ main.cpp -o bradley `pkg-config --cflags --libs opencv`
bradley1:
	g++ main.cpp -o bradley -lgomp -fopenmp `pkg-config --cflags --libs opencv`
bradley2:
	g++ main.cpp -o bradley -lgomp


