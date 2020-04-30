#pragma once
#include<windows.h>
#include<TlHelp32.h>
struct List
{
	PROCESSENTRY32 Data;
	List* Next;
}*Processes = NULL;


void init();
