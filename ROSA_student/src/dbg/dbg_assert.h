/*
 * dbg_assert.h
 *
 * Created: 13.12.2018 0:43:05
 *  Author: ant
 */ 

#ifndef DBG_ASSERT_H_
#define DBG_ASSERT_H_

#ifdef DBG_MODE
	#define ASSERT(expr) if (!(expr)) aFailed(__FILE__, __LINE__)
	#define STATIC_ASSERT(expr) typedef int foo[(expr) ? 1 : -1]
#else
	#define ASSERT(expr) ;
	#define STATIC_ASSERT(expr) ; 
#endif

void aFailed(char *file, int line);

#endif /* DBG_ASSERT_H_ */