
#ifndef _TEXTURE_MANAGER_H_
#define _TEXTURE_MANAGER_H_

#define TEXTURE_FLAG_CLAMPED 0x80000000
#define TEXTURE_FLAG_NEAREST 0x40000000

void TexManager_Init();
void TexManager_GetTextureResolutionByName(char *name, I32 *width, I32 *height);
int	 TexManager_AddTexture(char *filename, U32 user_flags);
int	 TexManager_AddTextureFromIndexedData(const U8* data, I32 width, I32 height, U32 user_flags, bool save = false);
bool TexManager_LoadTexture(int index, char *filename, bool clamped, bool nearest);
bool TexManager_UnloadTexture(int index);
void TexManager_LoadAll();
void TexManager_UnloadAll();
int  TexManager_GetTextureIndexByName(char *name);
void TexManager_SetTextureByIndex(int index);
void TexManager_SetTextureByName(char *name);
void TexManager_RemoveTextureByIndex(int index);
void TexManager_RemoveTextureByName(char *name);
void TexManager_RemoveTexturesByFlag(U32 user_flag);
void TexManager_RemoveAllTextures();
void TexManager_Release();

#endif /* _TEXTURE_MANAGER_H_ */
