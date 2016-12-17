
#pragma once

#include "Common.h"

bool  Parser_OpenFile(const char* filename);
bool  Parser_OpenFile(const char* filename, const char *separators);
bool  Parser_ReadToken();
bool  Parser_ReadToken(const char *separators);
bool  Parser_ReadLine();
bool  Parser_IsEqual(const char *token);
char  *Parser_GetString();
int   Parser_GetInteger();
float Parser_GetFloat();
float Parser_GetDouble();
void  Parser_CloseFile();



