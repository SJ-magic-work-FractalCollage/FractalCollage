/************************************************************
************************************************************/
#pragma once

/************************************************************
************************************************************/
#include "stdio.h"

#include "ofMain.h"


/************************************************************
************************************************************/
#define ERROR_MSG(); printf("Error in %s:%d\n", __FILE__, __LINE__);


/************************************************************
************************************************************/

/**************************************************
Derivation
	class MyClass : private Noncopyable {
	private:
	public:
	};
**************************************************/
class Noncopyable{
protected:
	Noncopyable() {}
	~Noncopyable() {}

private:
	void operator =(const Noncopyable& src);
	Noncopyable(const Noncopyable& src);
};


/************************************************************
************************************************************/
extern FILE* fp_Log;

extern int GPIO_0;
extern int GPIO_1;

