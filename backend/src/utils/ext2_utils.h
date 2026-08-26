#ifndef EXT2_UTILS_H
#define EXT2_UTILS_H

#include <string>
#include <vector>
#include <fstream>
#include <cstring>
#include "../structures/mbr.h"
#include "../structures/superblock.h"
#include "../structures/inode.h"
#include "../structures/block.h"

#define INODE_SIZE 128
#define BLOCK_SIZE 64
#define NUM_DIRECT_BLOCKS 12

class Ext2Utils {
public:
    // ===== USERS.TXT =====
    static std::vector<std::string> readUsersFile(const std::string& diskPath, 
                                                  const std::string& mountId,
                                                  const MBR& mbr, int partitionIndex);
    
    static bool writeUsersFile(const std::string& diskPath, 
                               const std::vector<std::string>& lines,
                               const MBR& mbr, int partitionIndex);
    
    // ===== ARCHIVOS EXT2 =====
    static std::string readFile(const std::string& diskPath, const std::string& filePath, 
                                const Superblock& sb, const MBR& mbr, int partitionIndex);
    
    static bool writeFile(const std::string& diskPath, const std::string& filePath, 
                          const std::string& content, const Superblock& sb, 
                          const MBR& mbr, int partitionIndex, int uid, int gid);
    
    static bool createDirectory(const std::string& diskPath, const std::string& dirPath,
                                const Superblock& sb, const MBR& mbr, 
                                int partitionIndex, int uid, int gid);
    
    // ===== UTILIDADES EXT2 =====
    static Superblock readSuperblock(const std::string& diskPath, const MBR& mbr, int partitionIndex);
    static bool writeSuperblock(const std::string& diskPath, const Superblock& sb, const MBR& mbr, int partitionIndex);
    
    static int findFreeInode(std::fstream& disk, const Superblock& sb);
    static int findFreeBlock(std::fstream& disk, const Superblock& sb);
    static void markInodeUsed(std::fstream& disk, const Superblock& sb, int inodeIndex);
    static void markBlockUsed(std::fstream& disk, const Superblock& sb, int blockIndex);
    
    static int getRootInode();
    static std::vector<std::string> splitPath(const std::string& path);
    static int findInodeByPath(const std::string& diskPath, const std::string& path,
                               const Superblock& sb, const MBR& mbr, int partitionIndex);
    
    static Inode readInode(std::fstream& disk, const Superblock& sb, int inodeIndex);
    static bool writeInode(std::fstream& disk, const Superblock& sb, int inodeIndex, const Inode& inode);
    
    static BlockFolder readBlockFolder(std::fstream& disk, const Superblock& sb, int blockIndex);
    static bool writeBlockFolder(std::fstream& disk, const Superblock& sb, int blockIndex, const BlockFolder& block);
    
    static BlockFile readBlockFile(std::fstream& disk, const Superblock& sb, int blockIndex);
    static bool writeBlockFile(std::fstream& disk, const Superblock& sb, int blockIndex, const BlockFile& block);
};

#endif