#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <windows.h>
#include <dirent.h>

char* getDirExeRuns(char* currentDirectoryPtr, int size){
  if (getcwd(currentDirectoryPtr, size) != NULL){
    return currentDirectoryPtr;
  } else{
    perror("getcwd() error");
    return NULL;
  }
}


FILE* openFileReadM(int argc, char* snipsPath){
  if (argc != 2 && argc != 1){
    printf("This command needs 1 or 2 arguments, you provided: %d\n", argc - 1);
    exit(EXIT_FAILURE);
  }

  FILE* fptr = fopen(snipsPath, "r");
  if (fptr == NULL){
    perror("Error opening the file");
    exit(EXIT_FAILURE);
  }
  return fptr;
}

long getFileSize(FILE* fptr){
  fseek(fptr, 0, SEEK_END); // seek to end of file
  long size = ftell(fptr); // get current file pointer
  fseek(fptr, 0, SEEK_SET); // seek back to beginning of file
  return size;
}

void printAllSnips(char *directory_path){
  WIN32_FIND_DATA findFileData;
  HANDLE hFind = FindFirstFile(strcat(directory_path, "\\*"), &findFileData);
  if (hFind == INVALID_HANDLE_VALUE) {
      perror("Error finding files");
  }
  do {
    if (findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) { continue; }
    printf("File: %s\n", findFileData.cFileName);
  } 
  while (FindNextFile(hFind, &findFileData) != 0);
  FindClose(hFind);
}

int main(int argc, char *argv[]){  
  FILE* fptr;

  char snipsDirPath[1024];
  char snipsDirPathCopy[1024];
  strcpy(snipsDirPath, ".\\codesnip\\");
  strcpy(snipsDirPathCopy, snipsDirPath);
  printAllSnips(snipsDirPathCopy);

  if(argc==2){ strcat(snipsDirPath, argv[1]); }
  else{
    char inputRM[1024]={0};
    printf("inputRM: %s\n", inputRM);
    scanf("%s", inputRM);
    memcpy(snipsDirPath, snipsDirPath, sizeof(snipsDirPath)/sizeof(char)-2);
    printf("snipsDirPath: %s\n", snipsDirPath);
    strcat(snipsDirPath, inputRM);
    printf("inputRM: %s\n", inputRM);
  }

  char snipsFilePath[1024]="";
  strcpy(snipsFilePath, snipsDirPath);
  printf("snipsFilePath: %s\n", snipsFilePath);
  fptr = openFileReadM(argc, snipsFilePath);
  
  long size = getFileSize(fptr);
  char buffer[size];
  for (int i=0; i<size+1; i++){ if(i==size){buffer[i]='\0';} else{buffer[i]=' ';} }
  fread(buffer, 1, size, fptr);
  printf("File content:\n%s\n", buffer);

  fclose(fptr);

  size_t len = strlen(buffer) + 1;
  HGLOBAL hMem =  GlobalAlloc(GMEM_MOVEABLE, len);
  memcpy(GlobalLock(hMem), buffer, len);
  GlobalUnlock(hMem);
  OpenClipboard(0);
  EmptyClipboard();
  SetClipboardData(CF_TEXT, hMem);
  CloseClipboard();
  return 0;
}
