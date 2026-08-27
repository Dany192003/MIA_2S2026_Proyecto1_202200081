#include "ext2_utils.h"
#include <sstream>
#include <algorithm>
#include <fstream>
#include <cstring>
#include <iostream>

// ============================================================
// SUPERBLOCK
// ============================================================

Superblock Ext2Utils::readSuperblock(const std::string& diskPath, const MBR& mbr, int partitionIndex) {
    Superblock sb;
    memset(&sb, 0, sizeof(Superblock));
    
    std::fstream disk(diskPath, std::ios::in | std::ios::binary);
    if (!disk.is_open()) return sb;
    
    int64_t partitionStart = mbr.mbr_partitions[partitionIndex].part_start;
    disk.seekg(partitionStart, std::ios::beg);
    disk.read(reinterpret_cast<char*>(&sb), sizeof(Superblock));
    disk.close();
    
    return sb;
}

bool Ext2Utils::writeSuperblock(const std::string& diskPath, const Superblock& sb, const MBR& mbr, int partitionIndex) {
    std::fstream disk(diskPath, std::ios::in | std::ios::out | std::ios::binary);
    if (!disk.is_open()) return false;
    
    int64_t partitionStart = mbr.mbr_partitions[partitionIndex].part_start;
    disk.seekp(partitionStart, std::ios::beg);
    disk.write(reinterpret_cast<const char*>(&sb), sizeof(Superblock));
    disk.close();
    
    return true;
}

// ============================================================
// BITMAPS
// ============================================================

int Ext2Utils::findFreeInode(std::fstream& disk, const Superblock& sb) {
    int bmSize = (sb.s_inodes_count + 7) / 8;
    disk.seekg(sb.s_bm_inode_start, std::ios::beg);
    
    for (int i = 0; i < bmSize; i++) {
        char byte;
        disk.read(&byte, 1);
        for (int bit = 0; bit < 8; bit++) {
            int index = i * 8 + bit;
            if (index >= sb.s_inodes_count) return -1;
            if (!(byte & (1 << bit))) {
                return index;
            }
        }
    }
    return -1;
}

int Ext2Utils::findFreeBlock(std::fstream& disk, const Superblock& sb) {
    int bmSize = (sb.s_blocks_count + 7) / 8;
    disk.seekg(sb.s_bm_block_start, std::ios::beg);
    
    for (int i = 0; i < bmSize; i++) {
        char byte;
        disk.read(&byte, 1);
        for (int bit = 0; bit < 8; bit++) {
            int index = i * 8 + bit;
            if (index >= sb.s_blocks_count) return -1;
            if (!(byte & (1 << bit))) {
                return index;
            }
        }
    }
    return -1;
}

void Ext2Utils::markInodeUsed(std::fstream& disk, const Superblock& sb, int inodeIndex) {
    int byteIndex = inodeIndex / 8;
    int bitIndex = inodeIndex % 8;
    
    disk.seekg(sb.s_bm_inode_start + byteIndex, std::ios::beg);
    char byte;
    disk.read(&byte, 1);
    byte |= (1 << bitIndex);
    disk.seekp(sb.s_bm_inode_start + byteIndex, std::ios::beg);
    disk.write(&byte, 1);
}

void Ext2Utils::markInodeFree(std::fstream& disk, const Superblock& sb, int inodeIndex) {
    int byteIndex = inodeIndex / 8;
    int bitIndex = inodeIndex % 8;
    
    disk.seekg(sb.s_bm_inode_start + byteIndex, std::ios::beg);
    char byte;
    disk.read(&byte, 1);
    byte &= ~(1 << bitIndex);
    disk.seekp(sb.s_bm_inode_start + byteIndex, std::ios::beg);
    disk.write(&byte, 1);
}

void Ext2Utils::markBlockUsed(std::fstream& disk, const Superblock& sb, int blockIndex) {
    int byteIndex = blockIndex / 8;
    int bitIndex = blockIndex % 8;
    
    disk.seekg(sb.s_bm_block_start + byteIndex, std::ios::beg);
    char byte;
    disk.read(&byte, 1);
    byte |= (1 << bitIndex);
    disk.seekp(sb.s_bm_block_start + byteIndex, std::ios::beg);
    disk.write(&byte, 1);
}

void Ext2Utils::markBlockFree(std::fstream& disk, const Superblock& sb, int blockIndex) {
    int byteIndex = blockIndex / 8;
    int bitIndex = blockIndex % 8;
    
    disk.seekg(sb.s_bm_block_start + byteIndex, std::ios::beg);
    char byte;
    disk.read(&byte, 1);
    byte &= ~(1 << bitIndex);
    disk.seekp(sb.s_bm_block_start + byteIndex, std::ios::beg);
    disk.write(&byte, 1);
}

// ============================================================
// INODOS
// ============================================================

Inode Ext2Utils::readInode(std::fstream& disk, const Superblock& sb, int inodeIndex) {
    Inode inode;
    memset(&inode, 0, sizeof(Inode));
    
    int64_t offset = sb.s_inode_start + inodeIndex * INODE_SIZE;
    disk.seekg(offset, std::ios::beg);
    disk.read(reinterpret_cast<char*>(&inode), sizeof(Inode));
    
    return inode;
}

bool Ext2Utils::writeInode(std::fstream& disk, const Superblock& sb, int inodeIndex, const Inode& inode) {
    int64_t offset = sb.s_inode_start + inodeIndex * INODE_SIZE;
    disk.seekp(offset, std::ios::beg);
    disk.write(reinterpret_cast<const char*>(&inode), sizeof(Inode));
    return true;
}

// ============================================================
// BLOQUES
// ============================================================

BlockFolder Ext2Utils::readBlockFolder(std::fstream& disk, const Superblock& sb, int blockIndex) {
    BlockFolder block;
    memset(&block, 0, sizeof(BlockFolder));
    
    int64_t offset = sb.s_block_start + blockIndex * BLOCK_SIZE;
    disk.seekg(offset, std::ios::beg);
    disk.read(reinterpret_cast<char*>(&block), sizeof(BlockFolder));
    
    return block;
}

bool Ext2Utils::writeBlockFolder(std::fstream& disk, const Superblock& sb, int blockIndex, const BlockFolder& block) {
    int64_t offset = sb.s_block_start + blockIndex * BLOCK_SIZE;
    disk.seekp(offset, std::ios::beg);
    disk.write(reinterpret_cast<const char*>(&block), sizeof(BlockFolder));
    return true;
}

BlockFile Ext2Utils::readBlockFile(std::fstream& disk, const Superblock& sb, int blockIndex) {
    BlockFile block;
    memset(&block, 0, sizeof(BlockFile));
    
    int64_t offset = sb.s_block_start + blockIndex * BLOCK_SIZE;
    disk.seekg(offset, std::ios::beg);
    disk.read(reinterpret_cast<char*>(&block), sizeof(BlockFile));
    
    return block;
}

bool Ext2Utils::writeBlockFile(std::fstream& disk, const Superblock& sb, int blockIndex, const BlockFile& block) {
    int64_t offset = sb.s_block_start + blockIndex * BLOCK_SIZE;
    disk.seekp(offset, std::ios::beg);
    disk.write(reinterpret_cast<const char*>(&block), sizeof(BlockFile));
    return true;
}

BlockPointer Ext2Utils::readBlockPointer(std::fstream& disk, const Superblock& sb, int blockIndex) {
    BlockPointer block;
    memset(&block, 0, sizeof(BlockPointer));
    
    int64_t offset = sb.s_block_start + blockIndex * BLOCK_SIZE;
    disk.seekg(offset, std::ios::beg);
    disk.read(reinterpret_cast<char*>(&block), sizeof(BlockPointer));
    
    return block;
}

bool Ext2Utils::writeBlockPointer(std::fstream& disk, const Superblock& sb, int blockIndex, const BlockPointer& block) {
    int64_t offset = sb.s_block_start + blockIndex * BLOCK_SIZE;
    disk.seekp(offset, std::ios::beg);
    disk.write(reinterpret_cast<const char*>(&block), sizeof(BlockPointer));
    return true;
}

// ============================================================
// PATH UTILITIES
// ============================================================

int Ext2Utils::getRootInode() {
    return 0;
}

std::vector<std::string> Ext2Utils::splitPath(const std::string& path) {
    std::vector<std::string> parts;
    std::stringstream ss(path);
    std::string part;
    while (std::getline(ss, part, '/')) {
        if (!part.empty()) {
            parts.push_back(part);
        }
    }
    return parts;
}

int Ext2Utils::findInodeByPath(const std::string& diskPath, const std::string& path,
                               const Superblock& sb, const MBR& mbr, int partitionIndex) {
    if (path == "/" || path.empty()) return 0;
    
    std::vector<std::string> parts = splitPath(path);
    if (parts.empty()) return -1;
    
    std::fstream disk(diskPath, std::ios::in | std::ios::binary);
    if (!disk.is_open()) return -1;
    
    int currentInode = 0;
    
    for (const auto& part : parts) {
        Inode inode = readInode(disk, sb, currentInode);
        if (inode.i_type != '0') {
            disk.close();
            return -1;
        }
        
        if (inode.i_block[0] == -1) {
            disk.close();
            return -1;
        }
        
        BlockFolder block = readBlockFolder(disk, sb, inode.i_block[0]);
        bool found = false;
        
        for (int i = 0; i < 4; i++) {
            std::string name(block.b_content[i].b_name);
            name = name.c_str();
            if (name == part) {
                currentInode = block.b_content[i].b_inodo;
                found = true;
                break;
            }
        }
        
        if (!found) {
            disk.close();
            return -1;
        }
    }
    
    disk.close();
    return currentInode;
}

// ============================================================
// LEER/ESCRIBIR ARCHIVOS
// ============================================================

std::string Ext2Utils::readFile(const std::string& diskPath, const std::string& filePath, 
                                const Superblock& sb, const MBR& mbr, int partitionIndex) {
    int inodeIndex = findInodeByPath(diskPath, filePath, sb, mbr, partitionIndex);
    if (inodeIndex == -1) return "";
    
    std::fstream disk(diskPath, std::ios::in | std::ios::binary);
    if (!disk.is_open()) return "";
    
    Inode inode = readInode(disk, sb, inodeIndex);
    if (inode.i_type != '1') {
        disk.close();
        return "";
    }
    
    std::string content;
    for (int i = 0; i < 16 && i < 16; i++) {
        if (inode.i_block[i] == -1) break;
        
        if (i < 12) {
            // Bloque directo
            BlockFile block = readBlockFile(disk, sb, inode.i_block[i]);
            content += std::string(block.b_content, 64);
        } else if (i == 12) {
            // Bloque simple indirecto
            BlockPointer pointer = readBlockPointer(disk, sb, inode.i_block[i]);
            for (int j = 0; j < 16; j++) {
                if (pointer.b_pointer[j] == -1) break;
                BlockFile block = readBlockFile(disk, sb, pointer.b_pointer[j]);
                content += std::string(block.b_content, 64);
            }
        } else if (i == 13) {
            // Bloque doble indirecto
            BlockPointer pointer1 = readBlockPointer(disk, sb, inode.i_block[i]);
            for (int j = 0; j < 16; j++) {
                if (pointer1.b_pointer[j] == -1) break;
                BlockPointer pointer2 = readBlockPointer(disk, sb, pointer1.b_pointer[j]);
                for (int k = 0; k < 16; k++) {
                    if (pointer2.b_pointer[k] == -1) break;
                    BlockFile block = readBlockFile(disk, sb, pointer2.b_pointer[k]);
                    content += std::string(block.b_content, 64);
                }
            }
        } else if (i == 14) {
            // Bloque triple indirecto
            BlockPointer pointer1 = readBlockPointer(disk, sb, inode.i_block[i]);
            for (int j = 0; j < 16; j++) {
                if (pointer1.b_pointer[j] == -1) break;
                BlockPointer pointer2 = readBlockPointer(disk, sb, pointer1.b_pointer[j]);
                for (int k = 0; k < 16; k++) {
                    if (pointer2.b_pointer[k] == -1) break;
                    BlockPointer pointer3 = readBlockPointer(disk, sb, pointer2.b_pointer[k]);
                    for (int l = 0; l < 16; l++) {
                        if (pointer3.b_pointer[l] == -1) break;
                        BlockFile block = readBlockFile(disk, sb, pointer3.b_pointer[l]);
                        content += std::string(block.b_content, 64);
                    }
                }
            }
        }
    }
    
    disk.close();
    content = content.c_str();
    return content;
}

bool Ext2Utils::writeFile(const std::string& diskPath, const std::string& filePath, 
                          const std::string& content, const Superblock& sb, 
                          const MBR& mbr, int partitionIndex, int uid, int gid) {
    std::fstream disk(diskPath, std::ios::in | std::ios::out | std::ios::binary);
    if (!disk.is_open()) return false;
    
    std::vector<std::string> parts = splitPath(filePath);
    if (parts.empty()) {
        disk.close();
        return false;
    }
    
    std::string fileName = parts.back();
    parts.pop_back();
    
    std::string parentPath = "/";
    if (!parts.empty()) {
        parentPath += parts[0];
        for (size_t i = 1; i < parts.size(); i++) {
            parentPath += "/" + parts[i];
        }
    }
    
    int parentInodeIndex = findInodeByPath(diskPath, parentPath, sb, mbr, partitionIndex);
    if (parentInodeIndex == -1) {
        disk.close();
        return false;
    }
    
    Inode parentInode = readInode(disk, sb, parentInodeIndex);
    if (parentInode.i_type != '0') {
        disk.close();
        return false;
    }
    
    // Verificar si el archivo ya existe
    int existingInode = findInodeByPath(diskPath, filePath, sb, mbr, partitionIndex);
    if (existingInode != -1) {
        // Eliminar el archivo existente
        Inode existing = readInode(disk, sb, existingInode);
        for (int i = 0; i < 16; i++) {
            if (existing.i_block[i] != -1) {
                markBlockFree(disk, sb, existing.i_block[i]);
            }
        }
        markInodeFree(disk, sb, existingInode);
    }
    
    int newInodeIndex = findFreeInode(disk, sb);
    if (newInodeIndex == -1) {
        disk.close();
        return false;
    }
    
    size_t contentSize = content.length();
    int numBlocks = (contentSize + BLOCK_SIZE - 1) / BLOCK_SIZE;
    if (numBlocks < 1) numBlocks = 1;
    
    // Calcular cuántos bloques directos e indirectos necesitamos
    int directBlocks = std::min(numBlocks, 12);
    int remaining = numBlocks - directBlocks;
    int simpleIndirect = 0;
    int doubleIndirect = 0;
    int tripleIndirect = 0;
    
    if (remaining > 0) {
        simpleIndirect = 1;
        remaining -= 16;
    }
    if (remaining > 0) {
        doubleIndirect = 1;
        remaining -= 256;
    }
    if (remaining > 0) {
        tripleIndirect = 1;
    }
    
    int totalBlocks = directBlocks + simpleIndirect + doubleIndirect + tripleIndirect;
    if (totalBlocks > 16) totalBlocks = 16;
    
    int blockIndices[16];
    for (int i = 0; i < 16; i++) blockIndices[i] = -1;
    
    // Asignar bloques directos
    for (int i = 0; i < directBlocks; i++) {
        int blockIndex = findFreeBlock(disk, sb);
        if (blockIndex == -1) {
            disk.close();
            return false;
        }
        blockIndices[i] = blockIndex;
        markBlockUsed(disk, sb, blockIndex);
    }
    
    // Asignar bloque simple indirecto
    if (simpleIndirect > 0) {
        int blockIndex = findFreeBlock(disk, sb);
        if (blockIndex == -1) {
            disk.close();
            return false;
        }
        blockIndices[12] = blockIndex;
        markBlockUsed(disk, sb, blockIndex);
    }
    
    // Asignar bloque doble indirecto
    if (doubleIndirect > 0) {
        int blockIndex = findFreeBlock(disk, sb);
        if (blockIndex == -1) {
            disk.close();
            return false;
        }
        blockIndices[13] = blockIndex;
        markBlockUsed(disk, sb, blockIndex);
    }
    
    // Asignar bloque triple indirecto
    if (tripleIndirect > 0) {
        int blockIndex = findFreeBlock(disk, sb);
        if (blockIndex == -1) {
            disk.close();
            return false;
        }
        blockIndices[14] = blockIndex;
        markBlockUsed(disk, sb, blockIndex);
    }
    
    Inode newInode;
    memset(&newInode, 0, sizeof(Inode));
    newInode.i_uid = uid;
    newInode.i_gid = gid;
    newInode.i_s = contentSize;
    newInode.i_atime = time(nullptr);
    newInode.i_ctime = time(nullptr);
    newInode.i_mtime = time(nullptr);
    for (int i = 0; i < 16; i++) newInode.i_block[i] = blockIndices[i];
    newInode.i_type = '1';
    newInode.i_perm[0] = '6';
    newInode.i_perm[1] = '6';
    newInode.i_perm[2] = '4';
    
    writeInode(disk, sb, newInodeIndex, newInode);
    markInodeUsed(disk, sb, newInodeIndex);
    
    // Escribir datos en bloques directos
    size_t offset = 0;
    for (int i = 0; i < directBlocks && i < 12; i++) {
        BlockFile fileBlock;
        memset(&fileBlock, 0, sizeof(BlockFile));
        size_t toCopy = std::min((size_t)BLOCK_SIZE, contentSize - offset);
        if (toCopy > 0) {
            strncpy(fileBlock.b_content, content.c_str() + offset, toCopy);
        }
        writeBlockFile(disk, sb, blockIndices[i], fileBlock);
        offset += BLOCK_SIZE;
    }
    
    // Escribir datos en bloques indirectos (simplificado)
    // Nota: Esta es una implementación simplificada de indirectos
    
    // Actualizar carpeta padre
    int blockIndex = parentInode.i_block[0];
    if (blockIndex == -1) {
        blockIndex = findFreeBlock(disk, sb);
        if (blockIndex == -1) {
            disk.close();
            return false;
        }
        parentInode.i_block[0] = blockIndex;
        writeInode(disk, sb, parentInodeIndex, parentInode);
        markBlockUsed(disk, sb, blockIndex);
        
        BlockFolder folderBlock;
        memset(&folderBlock, 0, sizeof(BlockFolder));
        for (int i = 0; i < 4; i++) {
            folderBlock.b_content[i].b_inodo = -1;
            memset(folderBlock.b_content[i].b_name, 0, 12);
        }
        writeBlockFolder(disk, sb, blockIndex, folderBlock);
    }
    
    BlockFolder folderBlock = readBlockFolder(disk, sb, blockIndex);
    bool added = false;
    for (int i = 0; i < 4; i++) {
        std::string name(folderBlock.b_content[i].b_name);
        name = name.c_str();
        if (name.empty() || folderBlock.b_content[i].b_inodo == -1) {
            strncpy(folderBlock.b_content[i].b_name, fileName.c_str(), 11);
            folderBlock.b_content[i].b_inodo = newInodeIndex;
            writeBlockFolder(disk, sb, blockIndex, folderBlock);
            added = true;
            break;
        }
    }
    
    if (!added) {
        disk.close();
        return false;
    }
    
    disk.close();
    return true;
}

// ============================================================
// CREAR CARPETAS (COMPLETO)
// ============================================================

bool Ext2Utils::createDirectory(const std::string& diskPath, const std::string& dirPath,
                                const Superblock& sb, const MBR& mbr, 
                                int partitionIndex, int uid, int gid) {
    std::fstream disk(diskPath, std::ios::in | std::ios::out | std::ios::binary);
    if (!disk.is_open()) return false;
    
    std::vector<std::string> parts = splitPath(dirPath);
    if (parts.empty()) {
        disk.close();
        return false;
    }
    
    std::string dirName = parts.back();
    parts.pop_back();
    
    std::string parentPath = "/";
    if (!parts.empty()) {
        parentPath += parts[0];
        for (size_t i = 1; i < parts.size(); i++) {
            parentPath += "/" + parts[i];
        }
    }
    
    int parentInodeIndex = findInodeByPath(diskPath, parentPath, sb, mbr, partitionIndex);
    if (parentInodeIndex == -1) {
        disk.close();
        return false;
    }
    
    Inode parentInode = readInode(disk, sb, parentInodeIndex);
    if (parentInode.i_type != '0') {
        disk.close();
        return false;
    }
    
    // Verificar si ya existe
    int existingInode = findInodeByPath(diskPath, dirPath, sb, mbr, partitionIndex);
    if (existingInode != -1) {
        disk.close();
        return false;
    }
    
    int newInodeIndex = findFreeInode(disk, sb);
    if (newInodeIndex == -1) {
        disk.close();
        return false;
    }
    
    // Crear bloque de carpeta
    int blockIndex = findFreeBlock(disk, sb);
    if (blockIndex == -1) {
        disk.close();
        return false;
    }
    markBlockUsed(disk, sb, blockIndex);
    
    // Inicializar bloque de carpeta con "." y ".."
    BlockFolder folderBlock;
    memset(&folderBlock, 0, sizeof(BlockFolder));
    
    // Entrada "." (apunta a sí mismo)
    strncpy(folderBlock.b_content[0].b_name, ".", 1);
    folderBlock.b_content[0].b_inodo = newInodeIndex;
    
    // Entrada ".." (apunta al padre)
    strncpy(folderBlock.b_content[1].b_name, "..", 2);
    folderBlock.b_content[1].b_inodo = parentInodeIndex;
    
    // Las demás entradas vacías
    for (int i = 2; i < 4; i++) {
        folderBlock.b_content[i].b_inodo = -1;
        memset(folderBlock.b_content[i].b_name, 0, 12);
    }
    
    writeBlockFolder(disk, sb, blockIndex, folderBlock);
    
    // Crear inodo para la carpeta
    Inode newInode;
    memset(&newInode, 0, sizeof(Inode));
    newInode.i_uid = uid;
    newInode.i_gid = gid;
    newInode.i_s = 0;
    newInode.i_atime = time(nullptr);
    newInode.i_ctime = time(nullptr);
    newInode.i_mtime = time(nullptr);
    for (int i = 0; i < 16; i++) newInode.i_block[i] = -1;
    newInode.i_block[0] = blockIndex;
    newInode.i_type = '0';
    newInode.i_perm[0] = '6';
    newInode.i_perm[1] = '6';
    newInode.i_perm[2] = '4';
    
    writeInode(disk, sb, newInodeIndex, newInode);
    markInodeUsed(disk, sb, newInodeIndex);
    
    // Agregar entrada en la carpeta padre
    int parentBlockIndex = parentInode.i_block[0];
    if (parentBlockIndex == -1) {
        parentBlockIndex = findFreeBlock(disk, sb);
        if (parentBlockIndex == -1) {
            disk.close();
            return false;
        }
        parentInode.i_block[0] = parentBlockIndex;
        writeInode(disk, sb, parentInodeIndex, parentInode);
        markBlockUsed(disk, sb, parentBlockIndex);
        
        BlockFolder emptyFolder;
        memset(&emptyFolder, 0, sizeof(BlockFolder));
        for (int i = 0; i < 4; i++) {
            emptyFolder.b_content[i].b_inodo = -1;
            memset(emptyFolder.b_content[i].b_name, 0, 12);
        }
        writeBlockFolder(disk, sb, parentBlockIndex, emptyFolder);
    }
    
    BlockFolder parentFolder = readBlockFolder(disk, sb, parentBlockIndex);
    bool added = false;
    for (int i = 0; i < 4; i++) {
        std::string name(parentFolder.b_content[i].b_name);
        name = name.c_str();
        if (name.empty() || parentFolder.b_content[i].b_inodo == -1) {
            strncpy(parentFolder.b_content[i].b_name, dirName.c_str(), 11);
            parentFolder.b_content[i].b_inodo = newInodeIndex;
            writeBlockFolder(disk, sb, parentBlockIndex, parentFolder);
            added = true;
            break;
        }
    }
    
    if (!added) {
        disk.close();
        return false;
    }
    
    disk.close();
    return true;
}

// ============================================================
// USERS.TXT
// ============================================================

std::vector<std::string> Ext2Utils::readUsersFile(const std::string& diskPath, 
                                                  const std::string& mountId,
                                                  const MBR& mbr, int partitionIndex) {
    std::vector<std::string> lines;
    
    Superblock sb = readSuperblock(diskPath, mbr, partitionIndex);
    
    int inodeIndex = findInodeByPath(diskPath, "/users.txt", sb, mbr, partitionIndex);
    
    if (inodeIndex != -1) {
        std::string content = readFile(diskPath, "/users.txt", sb, mbr, partitionIndex);
        std::stringstream ss(content);
        std::string line;
        while (std::getline(ss, line)) {
            if (!line.empty()) {
                lines.push_back(line);
            }
        }
    }
    
    if (lines.empty()) {
        lines.push_back("1, G, root");
        lines.push_back("1, U, root, root, 123");
    }
    
    return lines;
}

bool Ext2Utils::writeUsersFile(const std::string& diskPath, 
                               const std::vector<std::string>& lines,
                               const MBR& mbr, int partitionIndex) {
    std::string content;
    for (const auto& line : lines) {
        content += line + "\n";
    }
    
    Superblock sb = readSuperblock(diskPath, mbr, partitionIndex);
    return writeFile(diskPath, "/users.txt", content, sb, mbr, partitionIndex, 1, 1);
}