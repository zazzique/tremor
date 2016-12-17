

#include "windows.h"
#include "Common.h"
#include "FastMath.h"
#include "Utils.h"
#include "Files.h"

#define MAX_FILES 8192

char file_pathes[MAX_FILES][256]; // TODO: use strings
char file_names[MAX_FILES][64];
int files_count = 0;

int GetFileIndex(char *name)
{
	for (int i = 0; i < files_count; i ++)
	{
		if (strcicmp(file_names[i], name) == 0)
		{
			return i;
		}
	}

	return -1;
}

int AddFileToList(char *path, char *name)
{
	int current_file_index = files_count;

	char current_path[256];

	if (files_count >= MAX_FILES - 1)
	{
		Log::Print("Error! Too many files");
		return -1;
	}

	sprintf(current_path, "%s\\%s", path, name);

	for (int i = 0; i < files_count; i ++)
	{
		if (strcicmp(file_pathes[i], current_path) == 0) // TODO: CompareStrings everywhere
		{
			return i;
		}
	}

	strcpy(file_pathes[current_file_index], current_path);
	strcpy(file_names[current_file_index], name);

	files_count ++;

	return current_file_index;
}

void ScanDir(char *dir)
{
	char filter[MAX_PATH];
	char sub_dir[MAX_PATH];

	HANDLE hFind;
	WIN32_FIND_DATA data;

	sprintf(filter, "%s\\*", dir);

	hFind = FindFirstFile(filter, &data);
	if (hFind != INVALID_HANDLE_VALUE)
	{
		do
		{
			if (data.cFileName[0] != '.')
			{
				if (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
				{
				
					sprintf(sub_dir, "%s\\%s", dir, data.cFileName);
					ScanDir(sub_dir);

				}
				else
				{
					AddFileToList(dir, data.cFileName);
				}
			}
		}
		while (FindNextFile(hFind, &data));

		FindClose(hFind);
	}
}

bool Files_GetFilePathByName(char *name, char *result)
{
	int index = GetFileIndex(name);

	if (index < 0)
		return false;

	strcpy(result, &file_pathes[index][0]);

	return true;
}

void Files_Init()
{
	char dir[MAX_PATH];
	char current_dir[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, current_dir);

	sprintf(dir, "%s%s", current_dir, "\\Assets");

	ScanDir(dir);
}

bool Files_OpenFile(FileHandler *file, const char *name)
{
	const char *type = strrchr(name,'.');
	
	if (type == nullptr)
		return false;
	
	char base[128];
	
	strncpy(base, name, (type - name));
	base[type - name] = '\0';

	
	return Files_OpenFileOfType(file, base, type + 1);
}

bool Files_OpenFileAltType(FileHandler *file, const char *name, const char *type)
{
	const char *main_type = strrchr(name,'.');
	
	if (main_type == nullptr)
		return false;
	
	char base[128];
    
    strncpy(base, name, (main_type - name));
	base[main_type - name] = '\0';
    
    if (strlen(base) == 0)
        return false;
		
	if (Files_OpenFileOfType(file, base, type) == false)
	{
		return Files_OpenFileOfType(file, base, main_type + 1);
	}
	
	return true;
}

bool Files_OpenFileOfType(FileHandler *file, const char *name, const char *type)
{
	char full_name[64];
	sprintf(full_name, "%s.%s", name, type);
	int index = GetFileIndex(full_name);

	if (index < 0)
		return false;

    FILE *fp;
    
	fp = fopen(file_pathes[index], "rb");
	if (fp == nullptr)
		return false;
    
	fseek(fp, 0, SEEK_END);
	file->size = ftell(fp);
	rewind(fp);

	if (file->size <= 0)
		return false;
    
	file->data = (U8 *)malloc(file->size * sizeof(U8));
    
	if (file->data == nullptr)
		return false;

	int result = fread(file->data, sizeof(U8), file->size, fp);
	if (file->size != result)
	{
		free(file->data);
        file->data = nullptr;
		return false;
	}
	
	fclose(fp);
    
	file->current_pos = 0;
    
    strcpy(file->file_base, name);
	strcpy(file->file_extention, type);
    
	return true;
}

bool Files_GetData(FileHandler *file, void **data, I32 *size)
{
	if (file->data == nullptr)
		return false;

	if (data != nullptr)
		*data = file->data;

	if (size != nullptr)
		*size = file->size;

	return true;
}

I32 Files_GetSize(FileHandler *file)
{
	if (file->data == nullptr)
		return -1;
	
	return file->size;
}

I32 Files_GetCurrentPos(FileHandler *file)
{
	if (file->data == nullptr)
		return -1;
	
	return file->current_pos;
}

char *Files_GetFileBaseName(FileHandler *file)
{
	return file->file_base;
}

char *Files_GetFileExtension(FileHandler *file)
{
	return file->file_extention;
}

bool Files_Read(FileHandler *file, void *data, int size)
{
	if (file->current_pos + size > file->size)
		return false;
    
	U8 *data_src = &file->data[file->current_pos];
    
	memcpy(data, data_src, size);
    
	file->current_pos += size;
    
	return true;
}

bool Files_ReadCompressed(FileHandler *file, void *data)
{
	int i, j;
	I32 size, result_size;
	U8 count, data_byte;
	
	U8 *out = (U8*)data;
	
	if (file->data == nullptr)
		return false;
	
	memcpy(&size, file->data + file->current_pos, sizeof(I32));
    
	if (size == 0)
		return false;
	
	file->current_pos += sizeof(I32);
	
	result_size = 0;
	
	for (i = 0; i < size; i += 2)
	{
        data_byte = file->data[file->current_pos];
		file->current_pos ++;
        
        count = file->data[file->current_pos];
        file->current_pos ++;
		
		for (j = 0; j < count; j++)
		{
			*out = data_byte;
			out ++;
		}
		
		result_size += count;
	}
	
	return true;
}

void Files_Skip(FileHandler *file, int size)
{
	file->current_pos += size;
    
	if (file->current_pos > file->size - 1)
		file->current_pos = file->size - 1;
}

void Files_SetPos(FileHandler *file, int pos)
{
	file->current_pos = pos;
    
	if (file->current_pos > file->size - 1)
		file->current_pos = file->size - 1;
}

void Files_CloseFile(FileHandler *file)
{
    if (file->data != nullptr)
    {
        free(file->data);
        file->data = nullptr;
    }
}

void Files_Release()
{
	//
}


