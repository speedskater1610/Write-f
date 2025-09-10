#ifndef WRITE_F_H
#define WRITE_F_H


/**
 * RULES 
 * 
 * main is your main peice of text
 * 
 * vars must be passed in as a array through the void * type
 * 
 * formatting goes like this
 * 
 * {i} - int
 * {d} - double
 * {f} - float
 * {c} - char
 * {s} - char*
 * {&} - void* 
*/
void writef(char *main, void **vars);

#endif
