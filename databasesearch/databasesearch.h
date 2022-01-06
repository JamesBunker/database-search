/*
Student ID: bunkerj
Student Number: 1130246
Class: CIS2520 F21
Assignment 6
 */


#ifndef A6_H
#define A6_H
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "hash.h"

int getIdx(char*name, char*type);
char* query(char*code, int i1, char*type, int i2);
int entries(char *filebase);
char * intersection(char*fileBuidlingSet, char*fileRoomSet, char*name, char*number);
void setToIdx(char* filenameSet);
int setToIdxCopy(char* filenameSet);
void finalSetDisplay(int index);
char * getString(char * basename, int index);
#endif
