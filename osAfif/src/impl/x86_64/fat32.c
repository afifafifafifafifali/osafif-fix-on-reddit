#include "fat32.h"
#include "print.h"
#include "disk.h"
#include "mallocate.h"
#include "str.h"
#include "port.h"
#include "console_input.h"
#define FAT32_PARTITION_OFFSET_LBA 2048 // 1048576, 1MB

FAT32 fat;

// Since getDiskBytes get the shit in human readable format. I mean the format with ABCDEFGHIJKLMNOPQRSTUVWXYZ. 1234567890. & It literrally arrages it in the human readable format like, ive wrote the data AFIF, it displays the data AFIF
// Goofy ahh shits
#define TOUPPER(c) ((c >= 'a' && c <= 'z') ? (c - 'a' + 'A') : c)
#define ISSPACE(c)                                                             \
  (c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == '\v' || c == '\f')
// Goofy ahh shit ends  
//initialise fat32
int initiateFat32() {
    printf("\n[+] FAT32: Initializing...");
  
    printf("\n[+] FAT32: Reading disk0 at lba %d...", FAT32_PARTITION_OFFSET_LBA);
    unsigned char rawArr[SECTOR_SIZE];
    getDiskBytes(rawArr, FAT32_PARTITION_OFFSET_LBA, 1);
  
    printf("\n[+] FAT32: Checking if disk0 at lba %d is FAT32 formatted...",
           FAT32_PARTITION_OFFSET_LBA);
      
       
    // get sector count
    const unsigned char littleSectorCount =
        rawArr[19] | (rawArr[20] << 8); // (two, little endian)
    int bigSectorCount = 0;
    for (int i = 3; i >= 0; i--) {
      bigSectorCount =
          (bigSectorCount << 8) | rawArr[32 + i]; // (four, little endian)
    }
    fat.sector_count =
        littleSectorCount == 0 ? bigSectorCount : littleSectorCount;
  
    // get FAT's
    fat.number_of_fat = rawArr[16];
  
    // get reserved sectors (two, little endian)
    fat.reserved_sectors = rawArr[14] | (rawArr[15] << 8);
  
    // get sectors / track (two, little endian)
    fat.sectors_per_track = rawArr[24] | (rawArr[25] << 8);
  
    // get sectors / cluster
    fat.sectors_per_cluster = rawArr[13];
  
    // get sectors / fat (four, little endian)
    fat.sectors_per_fat = 0;
    for (int i = 3; i >= 0; i--) {
      fat.sectors_per_fat = (fat.sectors_per_fat << 8) | rawArr[36 + i];
    }
  
    // volume id
    fat.volume_id = 0;
    for (int i = 0; i < 4; i++) {
      fat.volume_id = (fat.volume_id << 8) | rawArr[67 + i];
    }
  
    // volume name
    for (int i = 0; i < 11; i++) {
      fat.volume_label[i] = (char)rawArr[71 + i];
    }
    fat.volume_label[11] = '\0';
  
    if (fat.sector_count == 0 || fat.reserved_sectors == 0 ||
        fat.sectors_per_track == 0 || fat.volume_id == 0) {
      printf("\n[+] FAT32: Failed to parse FAT information... This kernel only "
             "supports FAT32!\n");
      return 0;
    }
    if (rawArr[66] != 0x28 && rawArr[66] != 0x29) {
      printf("\n[+] FAT32: Incorrect disk signature! This kernel only supports "
             "FAT32!\n");
      return 0;
    }
  
    fat.fat_begin_lba = FAT32_PARTITION_OFFSET_LBA + fat.reserved_sectors;
    fat.cluster_begin_lba = FAT32_PARTITION_OFFSET_LBA + fat.reserved_sectors +
                            (fat.number_of_fat * fat.sectors_per_fat);
  
    fat.works = 1;
  
    printf("\n[+] FAT32: Valid FAT32 formatted drive: [%X] %s", fat.volume_id,
           fat.volume_label);
    printf("\n    [+] Sector count: %d", fat.sector_count);
    printf("\n    [+] FAT's: %d", fat.number_of_fat);
    printf("\n    [+] Reserved sectors: %d", fat.reserved_sectors);
    printf("\n    [+] Sectors / FAT: %d", fat.sectors_per_fat);
    printf("\n    [+] Sectors / track: %d", fat.sectors_per_track);
    printf("\n    [+] Sectors / cluster: %d", fat.sectors_per_cluster);
    printf("\n");
  }
  

  
unsigned int getFatEntry(int cluster) {
    int lba = fat.fat_begin_lba + (cluster * 4 / SECTOR_SIZE);
    int entryOffset = (cluster * 4) % SECTOR_SIZE;
  
    unsigned char rawArr[SECTOR_SIZE];
    getDiskBytes(rawArr, lba, 1);
    // unsigned int c = *(uint32 *)&rawArr[entryOffset] & 0x0FFFFFFF; //
    // 0x0FFFFFFF mask to keep lower 28 bits valid, nah fuck this ima do it
    // manually, watch & learn
    if (rawArr[entryOffset] >= 0xFFF8)
      return 0;
  
    unsigned int result = 0;
    for (int i = 3; i >= 0; i--) {
      result = (result << 8) | rawArr[entryOffset + i];
    }
  
    // printf("\n[%d] %x %x %x %x {Binary: %d Hexadecimal: %x}\n", entryOffset,
    //        rawArr[entryOffset], rawArr[entryOffset + 1], rawArr[entryOffset +
    //        2], rawArr[entryOffset + 3], result, result);
  
    return result;
  }

 
char *formatFilename(char **rawOriginal) {
    char *original = *rawOriginal;
    char *modifiable;
    int   i = 0;
    int   j = 0;
  
    while (original[i] != '\0' && j < 11) {
      if (original[i] != '.' && !ISSPACE(original[i])) {
        modifiable[j] = TOUPPER(original[i]);
        j++;
      }
      i++;
    }
  
    while (j < 11) {
      modifiable[j] = ' ';
      j++;
    }
  
    // modifiable[11] = '\0';
  
    return modifiable;
  }

  
  
int findFile(pFAT32_Directory fatdir, int initialCluster, char *filename) {
    int     clusterNum = initialCluster;
    uint8_t rawArr[SECTOR_SIZE];
    while (1) {
      const int lba =
          fat.cluster_begin_lba + (clusterNum - 2) * fat.sectors_per_cluster;
      getDiskBytes(rawArr, lba, 1);
  
      for (int i = 0; i < (SECTOR_SIZE / 32); i++) {
        if (memcmp(rawArr + (32 * i), filename, 11) == 0) { // fatdir->filename
          *fatdir = *(pFAT32_Directory)(&rawArr[32 * i]);
          printf("\n[search] filename: %s\n", filename);
          printf("\n[search] low=%d low1=%x low2=%x\n", fatdir->firstClusterLow,
                 rawArr[32 * i + 26], rawArr[32 * i + 27]);
          for (int o = 0; o < 32; o++) {
            printf("%x ", rawArr[32 * i + o]);
          }
          printf("\n");
          return 1;
        }
      }
  
      if (rawArr[SECTOR_SIZE - 32] != 0) {
        unsigned int nextCluster = getFatEntry(clusterNum);
        if (nextCluster == 0)
          return 0;
        clusterNum = nextCluster;
      } else
        return 0;
    }
  }
  
  int followConventionalDirectoryLoop(string outStr, string directory,
                                      int levelDeep) {
    int currLevelDeep = -1;
    int compIng = 0;
    int len = strlen(directory);
    for (int i = 0; i < len; i++) {
      if (compIng == 0) {
        if (directory[i] == '/')
          currLevelDeep++;
        if (currLevelDeep == levelDeep)
          compIng = 1;
      } else {
        if (directory[i] == '/') {
          outStr[compIng - 1] = '\0';
          return 1;
        } else if ((i + 1) == len) {
          outStr[compIng - 1] = directory[i];
          compIng++;
          outStr[compIng - 1] = '\0';
          return 1;
        } else {
          outStr[compIng - 1] = directory[i];
          compIng++;
        }
      }
    }
  
    return 0;
  }
  
  char *formatToShort8_3Format(char *directory) {
    static char out[12]; // 8 characters + dot + 3 characters + null terminator
    int         i;
  
    for (i = 0; i < 11; i++) {
      out[i] = ' ';
    }
    out[11] = '\0';
  
    int len = strlen(directory);
    int dotIndex = -1;
  
    for (i = len - 1; i >= 0; i--) {
      if (directory[i] == '.') {
        dotIndex = i;
        break;
      }
    }
  
    int nameLength = (dotIndex == -1) ? len : dotIndex;
    for (i = 0; i < nameLength && i < 8; i++) {
      char c = directory[i];
      if (c >= 'a' && c <= 'z') {
        c -= 'a' - 'A';
      }
      out[i] = c;
    }
  
    if (dotIndex != -1) {
      int extensionLength = len - (dotIndex + 1);
      for (i = 0; i < extensionLength && i < 3; i++) {
        char c = directory[dotIndex + 1 + i];
        if (c >= 'a' && c <= 'z') {
          c -= 'a' - 'A';
        }
        out[8 + i] = c;
      }
    }
  
    return out;
  }
  
  int charPrintNoFF(char *target) {
    if (*target != 0xFFFFFFFF)
      printf("%c", *target);
  }
  
  int calcLfn(int clusterNum, int nthOf32) {
    if (clusterNum < 2)
      return 0;
  
    unsigned char rawArr[SECTOR_SIZE];
    int           curr = 0;
  
    if (nthOf32 == 0) {
      curr = 1;
      nthOf32 = 33;
    }
    int checksum = 0x0;
    while (1) {
      const int lba = fat.cluster_begin_lba +
                      (clusterNum - 2 - curr) * fat.sectors_per_cluster;
      getDiskBytes(rawArr, lba, 1);
      for (int i = (nthOf32 - 1); i >= 0; i--) {
        if (rawArr[32 * i + 11] != 0x0F) {
          // printf("end of long \n", i);
          printf(" | checksum: 0x%x\n", checksum);
          return 1;
        }
  
        if (checksum == 0x0)
          printf("    [-1] ");
  
        for (int j = 0; j < 5; j++) {
          charPrintNoFF(&rawArr[32 * i + 1 + (j * 2)]);
        }
  
        for (int j = 0; j < 6; j++) {
          charPrintNoFF(&rawArr[32 * i + 14 + (j * 2)]);
        }
  
        for (int j = 0; j < 2; j++) {
          charPrintNoFF(&rawArr[32 * i + 28 + (j * 2)]);
        }
  
        checksum = rawArr[32 * i + 13];
  
        // if ((rawArr[32 * i] & 0x40) != 0) { // last long
        //   printf(" last long detected [index=%d] \n", i);
        // } else { // any long
        //   printf(" nth long detected [index=%d] \n", i);
        // }
      }
    }
  
    return 0;
  }
  
  int showCluster(int clusterNum, int attrLimitation) // NOT 0, NOT 1
  {
    if (clusterNum < 2)
      return 0;
  
    const int lba =
        fat.cluster_begin_lba + (clusterNum - 2) * fat.sectors_per_cluster;
    unsigned char rawArr[SECTOR_SIZE];
    getDiskBytes(rawArr, lba, 1);
  
    for (int i = 0; i < (SECTOR_SIZE / 32); i++) {
      if (rawArr[32 * i] == 0)
        break;
  
      pFAT32_Directory directory = (pFAT32_Directory)(&rawArr[32 * i]);
  
      if ((directory->attributes == 0x0F) || (directory->attributes == 0x08) ||
          (attrLimitation != NULL && directory->attributes != attrLimitation))
        continue;
  
      uint16_t createdDate = directory->creationDate;
      int      createdDay = createdDate & 0x1F;
      int      createdMonth = (createdDate >> 5) & 0xF;
      int      createdYear = ((createdDate >> 9) & 0x7F) + 1980;
  
      printf("[%d] attr: 0x%02X | created: %02d/%02d/%04d | ",
             directory->ntReserved, directory->attributes, createdDay,
             createdMonth, createdYear);
      int lfn = 0;
      // char *out;
      for (int o = 0; o < 11; o++) {
        if (rawArr[32 * i + o] == '~')
          lfn = 1;
        // out[o] = rawArr[32 * i + o];
        // printf("%c", rawArr[32 * i + o]);
      }
      // out[11] = '\0';
      // formatFilename(&directory->filename);
      printf("%.11s", directory->filename);
      printf("\n");
      if (lfn)
        calcLfn(clusterNum, i);
    }
  
    if (rawArr[SECTOR_SIZE - 32] != 0) {
      unsigned int nextCluster = getFatEntry(clusterNum);
      if (nextCluster == 0)
        return 1;
      showCluster(nextCluster, attrLimitation);
    }
  
    return 1;
  }
  
  int findExtensionIndex(char **filename) {
    char *filenameAcc = *filename;
    for (int i = 10; i >= 0; i--)
      if (filenameAcc[i] == 0x20)
        return (i + 1);
  
    return 10;
  }
  
  int findPaddingIndex(char **filename) {
    char *filenameAcc = *filename;
    int   ext = findExtensionIndex(filename);
    for (int i = (ext - 1); i >= 0; i--)
      if (filenameAcc[i] != 0x20)
        return (i + 1);
  
    return ext;
  }
  
  // int formatFilename(char **filename) {
  //   char *filenameAcc = *filename;
  
  //   int startPos = 8; // would be 7 if last was not getting cut off
  //   for (int i = 7; i >= 0; i--) {
  //     if (filenameAcc[i] != 0x20)
  //       break;
  //     startPos = i;
  //   }
  
  //   // filenameAcc[startPos++] = '.';
  //   for (int i = startPos; i < (startPos + 3); i++) {
  //     filenameAcc[i] = filenameAcc[8 + (i - startPos)];
  //   }
  
  //   startPos += 3;
  //   for (int i = startPos; i < 11; i++) {
  //     filenameAcc[i] = 0x20;
  //   }
  
  //   filename[startPos] = '\0';
  
  //   return 1;
  // }
  
  int divisionRoundUp(int a, int b) { return (a + (b - 1)) / b; }
  
  char *readFileContents(pFAT32_Directory dir) {
    printf("\n[read] filesize=%d cluster=%d\n", dir->filesize,
           dir->firstClusterLow);
    char *out;
    int   curr = 0;
    for (int i = 0; i < divisionRoundUp(dir->filesize, SECTOR_SIZE);
         i++) { // DIV_ROUND_CLOSEST(dir->filesize, SECTOR_SIZE)
      unsigned char rawArr[SECTOR_SIZE];
      const int     lba = fat.cluster_begin_lba +
                      (dir->firstClusterLow - 2) * fat.sectors_per_cluster + i;
      getDiskBytes(rawArr, lba, 1);
      for (int j = 0; j < SECTOR_SIZE; j++) {
        out[curr] = rawArr[j];
        curr++;
      }
    }
    return out;
  }
  
  int showFile(pFAT32_Directory dir) {
    printf("\n[read] filesize=%d cluster=%d\n", dir->filesize,
           dir->firstClusterLow);
    for (int i = 0; i < divisionRoundUp(dir->filesize, SECTOR_SIZE);
         i++) { // DIV_ROUND_CLOSEST(dir->filesize, SECTOR_SIZE)
      unsigned char rawArr[SECTOR_SIZE];
      const int     lba = fat.cluster_begin_lba +
                      (dir->firstClusterLow - 2) * fat.sectors_per_cluster + i;
      getDiskBytes(rawArr, lba, 1);
      for (int j = 0; j < SECTOR_SIZE; j++)
        printf("%c", rawArr[j]);
    }
  }
  
  char *empty;
  
  int openFile(pFAT32_Directory dir, char *filename) {
    if (filename[0] != '/')
      return 0;
  
    char *tmpBuff;
    int   index = 0;
    int   len = strlen(filename);
    dir->firstClusterLow = 2;
    memset(tmpBuff, '\0', len);
  
    for (int i = 1; i < len; i++) { // skip index 0
      if (filename[i] == '/' || (i + 1) == len) {
        if ((i + 1) == len)
          tmpBuff[index++] = filename[i];
        findFile(dir, dir->firstClusterLow, formatToShort8_3Format(tmpBuff));
        if (dir->filename[0] == 0x10)
          return 0;
  
        // cleanup
        memset(tmpBuff, '\0', len);
        index = 0;
      } else
        tmpBuff[index++] = filename[i];
    }
  
    return 1;
  }
  
  int fileReaderTest() {
    print_clear();
    printf("=========================================\n");
    printf("====      cavOS file reader 1.0      ====\n");
    printf("====    Copyright MalwarePad 2023    ====\n");
    printf("=========================================\n");
  
    printf("\nEnter cluster choice (2 -> root directory):\n> ");
    char choice[50];
    readStr(choice);
    int cluster = atoi(choice);
    printf("\n\nCluster information:\n");
    showCluster(cluster, 0);
    printf("\n");
  
    while (1) {
      printf("Note: } can be used to exit...\nEnter target filename (8.3 "
             "short):\n> ");
      char cnt[200];
      readStr(cnt);
      printf("\n");
  
      if (cnt[0] == '}')
        return 1;
  
      char *res = &cnt;
      printf("[input]: %s\n", res);
      char *modifiable = formatToShort8_3Format(res);
      // for (int i = 0; i < 11; i++) {
      //   // printf("%2x ", modifiable[i]);
      // }
      printf("\n[parse] FAT32-compatible filename: %s\n", modifiable);
  
      FAT32_Directory dir;
      findFile(&dir, cluster, modifiable);
      if (dir.filename[0] == 0x10) {
        printf("\nNo such file can be found!\n[input: %s] [prased: %s]\n\n", res,
               modifiable);
        continue;
      }
  
      printf("\n[search res] filename=%.11s attr=0x%x low=%d\n", dir.filename,
             dir.attributes, dir.firstClusterLow);
  
      // showFile(&dir);
      char *out = readFileContents(&dir);
      printf("%s", out);
      printf("\n\n");
    }
  }

  // ATOI
  int atoi(char *str) {
    int result = 0;
    int sign = 1;
  
    // Skip leading whitespace
    while (*str == ' ') {
      str++;
    }
  
    // Check for optional sign
    if (*str == '-') {
      sign = -1;
      str++;
    } else if (*str == '+') {
      str++;
    }
  
    // Convert characters to integer
    while (*str >= '0' && *str <= '9') {
      result = result * 10 + (*str - '0');
      str++;
    }
  
    return result * sign;
  }