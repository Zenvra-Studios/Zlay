#include <os/system/zlay_file.h>

#include <stdio.h>
#include <stdlib.h>

bool ZLay_ReadFile(const char* path, ZLay_FileData* out_file) {
  FILE* file;
  long size;
  void* data;

  if (path == NULL || out_file == NULL) {
    return false;
  }

  *out_file = (ZLay_FileData){0};
  file = fopen(path, "rb");
  if (file == NULL) {
    return false;
  }

  if (fseek(file, 0, SEEK_END) != 0) {
    fclose(file);
    return false;
  }

  size = ftell(file);
  if (size < 0 || fseek(file, 0, SEEK_SET) != 0) {
    fclose(file);
    return false;
  }

  data = malloc((size_t)size + 1u);
  if (data == NULL) {
    fclose(file);
    return false;
  }

  if ((size_t)size > 0u && fread(data, 1u, (size_t)size, file) != (size_t)size) {
    free(data);
    fclose(file);
    return false;
  }

  ((unsigned char*)data)[size] = 0u;
  fclose(file);

  out_file->data = data;
  out_file->size = (size_t)size;
  return true;
}

bool ZLay_WriteFile(const char* path, const void* data, size_t size) {
  FILE* file;

  if (path == NULL || (data == NULL && size > 0u)) {
    return false;
  }

  file = fopen(path, "wb");
  if (file == NULL) {
    return false;
  }

  if (size > 0u && fwrite(data, 1u, size, file) != size) {
    fclose(file);
    return false;
  }

  return fclose(file) == 0;
}

void ZLay_FreeFileData(ZLay_FileData* file) {
  if (file == NULL) {
    return;
  }

  free(file->data);
  *file = (ZLay_FileData){0};
}
