#include "command_handler.h"
#include "../utils/ext2_utils.h"
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <cstring>

CommandResult CommandHandler::processMkdir(const json& params) {
    CommandResult result;
    result.success = false;
    
    try {
        // 1. Obtener parámetros
        std::string path = params["path"];
        bool recursive = params.contains("p") && std::string(params["p"]) == "true";
        
        // 2. Validar que haya sesión activa
        if (!isLoggedIn()) {
            result.message = "Error: No hay sesión activa. Use LOGIN primero.";
            return result;
        }
        
        // 3. Validar ruta
        if (path.empty()) {
            result.message = "Error: La ruta no puede estar vacía";
            return result;
        }
        
        // 4. Obtener información de la partición
        std::string diskPath = currentSession.diskPath;
        std::string mountId = currentSession.mountId;
        int uid = currentSession.uid;
        int gid = currentSession.gid;
        
        // 5. Leer el MBR
        std::fstream disk(diskPath, std::ios::in | std::ios::out | std::ios::binary);
        if (!disk.is_open()) {
            result.message = "Error: No se pudo abrir el disco: " + diskPath;
            return result;
        }
        
        MBR mbr;
        disk.seekg(0, std::ios::beg);
        disk.read(reinterpret_cast<char*>(&mbr), sizeof(MBR));
        
        // 6. Buscar la partición por ID
        int partitionIndex = -1;
        for (int i = 0; i < 4; i++) {
            char partId[5] = {0};
            memcpy(partId, mbr.mbr_partitions[i].part_id, 4);
            partId[4] = '\0';
            std::string partIdStr(partId);
            partIdStr = partIdStr.c_str();
            
            if (partIdStr == mountId && mbr.mbr_partitions[i].part_s > 0) {
                partitionIndex = i;
                break;
            }
        }
        
        if (partitionIndex == -1) {
            disk.close();
            result.message = "Error: No se encontró la partición para el ID: " + mountId;
            return result;
        }
        
        // 7. Leer el Superblock
        Superblock sb = Ext2Utils::readSuperblock(diskPath, mbr, partitionIndex);
        disk.close();
        
        // 8. Verificar si la carpeta ya existe en EXT2
        int existingInode = Ext2Utils::findInodeByPath(diskPath, path, sb, mbr, partitionIndex);
        if (existingInode != -1) {
            result.message = "Error: La carpeta ya existe en EXT2: " + path;
            return result;
        }
        
        // 9. Verificar que la carpeta padre exista
        size_t lastSlash = path.find_last_of('/');
        if (lastSlash != std::string::npos) {
            std::string parentDir = path.substr(0, lastSlash);
            int parentInode = Ext2Utils::findInodeByPath(diskPath, parentDir, sb, mbr, partitionIndex);
            
            if (parentInode == -1) {
                if (recursive) {
                    // Crear carpetas padre recursivamente en EXT2
                    std::vector<std::string> parts = Ext2Utils::splitPath(parentDir);
                    std::string currentPath = "/";
                    for (const auto& part : parts) {
                        std::string testPath = currentPath + part;
                        int inode = Ext2Utils::findInodeByPath(diskPath, testPath, sb, mbr, partitionIndex);
                        if (inode == -1) {
                            if (!Ext2Utils::createDirectory(diskPath, testPath, sb, mbr, partitionIndex, uid, gid)) {
                                result.message = "Error: No se pudo crear la carpeta en EXT2: " + testPath;
                                return result;
                            }
                        }
                        currentPath = testPath + "/";
                    }
                } else {
                    result.message = "Error: La carpeta padre no existe en EXT2. Use -p para crearla automáticamente";
                    return result;
                }
            }
        }
        
        // 10. Crear la carpeta en EXT2
        if (!Ext2Utils::createDirectory(diskPath, path, sb, mbr, partitionIndex, uid, gid)) {
            result.message = "Error: No se pudo crear la carpeta en EXT2: " + path;
            return result;
        }
        
        // 11. Éxito
        result.success = true;
        result.message = "Carpeta creada exitosamente en EXT2: " + path;
        result.data["directory"] = {
            {"path", path},
            {"recursive", recursive},
            {"owner", currentSession.user},
            {"permissions", "664"}
        };
        
    } catch (const std::exception& e) {
        result.message = "Error al procesar MKDIR: " + std::string(e.what());
    }
    
    return result;
}