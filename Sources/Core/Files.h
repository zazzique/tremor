
#pragma once

typedef struct _FileHandler
{
    U8 *data;
    I32 size;
    I32 current_pos;
    
    char file_base[128];
	char file_extention[16];
} FileHandler;

void Files_Init();
void Files_Release();
bool Files_GetFilePathByName(char *name, char *result);
bool Files_OpenFile(FileHandler *file, const char *name);
bool Files_OpenFileAltType(FileHandler *file, const char *name, const char *type);
bool Files_OpenFileOfType(FileHandler *file, const char *name, const char *type);
bool Files_GetData(FileHandler *file, void **data, I32 *size);
I32  Files_GetSize(FileHandler *file);
I32  Files_GetCurrentPos(FileHandler *file);
char *Files_GetFileExtension(FileHandler *file);
char *Files_GetFileBaseName(FileHandler *file);
bool Files_Read(FileHandler *file, void *data, int size);
bool Files_ReadCompressed(FileHandler *file, void *data);
void Files_Skip(FileHandler *file, int size);
void Files_SetPos(FileHandler *file, int pos);
void Files_CloseFile(FileHandler *file);



