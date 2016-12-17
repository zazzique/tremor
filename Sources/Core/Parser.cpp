
#include "Common.h"
#include "Files.h"
#include "Utils.h"
#include "Parser.h"


FileHandler parsing_file;
char* parsing_content;

char* current_token;
char  parsing_separators[256];


bool Parser_OpenFile(const char *filename)
{
	return Parser_OpenFile(filename, CONTENT_PARSE_SEPARATORS);
}

bool Parser_OpenFile(const char *filename, const char *separators)
{
	strcpy(parsing_separators, separators);

	if (!Files_OpenFile(&parsing_file, filename))
	{
		LogPrint("Error: file '%s' not found!\n", filename);
		return false;
	}

	int content_size = Files_GetSize(&parsing_file) + 1;
	parsing_content = (char *)malloc(content_size);

	if (parsing_content == NULL)
	{
		Files_CloseFile(&parsing_file);

		LogPrint("Error: couldn't allocate memory!\n");
		return false;
	}
	
	Files_Read(&parsing_file, parsing_content, content_size - 1);
	
	parsing_content[content_size - 1] = '\0';

	current_token = strtok(parsing_content, parsing_separators);

	return true;
}

bool Parser_ReadToken()
{
	current_token = strtok(0, parsing_separators);

	return (current_token != NULL);
}

bool Parser_ReadToken(const char *separators)
{
	current_token = strtok(0, separators);

	return (current_token != NULL);
}

bool Parser_ReadLine()
{
	current_token = strtok(0, "\n\r");

	return (current_token != NULL);
}

bool Parser_IsEqual(const char *token)
{
	return (strcicmp(current_token, token) == 0);
}

char *Parser_GetString()
{
	return current_token;
}

int Parser_GetInteger()
{
	return atoi(current_token);
}

float Parser_GetFloat()
{
	return (float)atof(current_token);
}

float Parser_GetDouble()
{
	return atof(current_token);
}

void Parser_CloseFile()
{
	free(parsing_content);
	Files_CloseFile(&parsing_file);
}

