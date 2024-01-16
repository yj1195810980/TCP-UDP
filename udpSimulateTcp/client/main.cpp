#include<iostream>
#include"../common/CUMT.h"
int main(int argn, char** argc) {

	CUMT umt;
	umt.Connect("127.0.0.1", 5566);

	return 0;
}