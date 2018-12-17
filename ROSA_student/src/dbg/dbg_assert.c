/*
 * dbg_assert.c
 *
 * Created: 13.12.2018 0:43:26
 *  Author: ant
 */ 

#include "dbg/dbg_assert.h"
#include "rosa_config.h"

//#include "drivers/usart.h"

void printd(int n)
{
	if (n / 10)
	{
		printd(n / 10);
	}
	
	usartWriteChar(USART, n % 10 + '0');
}

void aFailed(char *file, int line)
{	
	usartWriteLine(USART, "Error in file: ");
	usartWriteLine(USART, file);
	usartWriteLine(USART, " at line ");
	printd(line);
	usartWriteLine(USART, "\r\n");
	usartWriteLine(USART, "Program execuion has been stopped!");
	
	while (1);
}
