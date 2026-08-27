#include "command_handler.h"
#include "../utils/ext2_utils.h"
#include <fstream>
#include <sstream>
#include <cstring>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>

// Función auxiliar para crear carpetas recursivas en sistema REAL
static bool createDirectoriesForFile(const std::string& path) {
    size_t pos = 0;
    std::string currentPath;
    
    if (path[0] == '/') {
        currentPath = "/";
        pos = 1;
    }
    
    while (pos < path.length()) {
        size_t nextSlash = path.find('/', pos);
        if (nextSlash == std::string::npos) {
            break;
        }
        
        std::string dir = path.substr(0, nextSlash);
        if (dir.length() > 0 && dir != "/") {
            if (mkdir(dir.c_str(), 0755) != 0 && errno != EEXIST) {
                return false;
            }
        }
        pos = nextSlash + 1;
    }
    return true;
}

// Función para leer contenido de archivo físico
static std::string readFileContent(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        return "";
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

CommandResult CommandHandler::processMkfile(const json& params) {
    CommandResult result;
    result.success = false;
    
    try {
        // 1. Obtener parámetros
        std::string path = params["path"];
        bool recursive = params.contains("r") && std::string(params["r"]) == "true";
        int size = params.contains("size") ? std::stoi(std::string(params["size"])) : 0;
        std::string cont = params.contains("cont") ? std::string(params["cont"]) : "";
        
        // 2. Validar que haya sesión activa
        if (!isLoggedIn()) {
            result.message = "Error: No hay sesión activa. Use LOGIN primero.";
            return result;
        }
        
        // 3. Validar tamaño negativo
        if (size < 0) {
            result.message = "Error: El tamaño no puede ser negativo";
            return result;
        }
        
        // 4. Validar ruta
        if (path.empty()) {
            result.message = "Error: La ruta no puede estar vacía";
            return result;
        }
        
        // 5. Obtener información de la partición
        std::string diskPath = currentSession.diskPath;
        std::string mountId = currentSession.mountId;
        int uid = currentSession.uid;
        int gid = currentSession.gid;
        
        // 6. Leer el MBR
        std::fstream disk(diskPath, std::ios::in | std::ios::out | std::ios::binary);
        if (!disk.is_open()) {
            result.message = "Error: No se pudo abrir el disco: " + diskPath;
            return result;
        }
        
        MBR mbr;
        disk.seekg(0, std::ios::beg);
        disk.read(reinterpret_cast<char*>(&mbr), sizeof(MBR));
        
        // 7. Buscar la partición por ID
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
        
        // 8. Leer el Superblock
        Superblock sb = Ext2Utils::readSuperblock(diskPath, mbr, partitionIndex);
        disk.close();
        
        // 9. Verificar que la carpeta padre exista en EXT2
        size_t lastSlash = path.find_last_of('/');
        if (lastSlash != std::string::npos) {
            std::string parentDir = path.substr(0, lastSlash);
            int parentInode = Ext2Utils::findInodeByPath(diskPath, parentDir, sb, mbr, partitionIndex);
            
            if (parentInode == -1) {
                if (recursive) {
                    // Crear carpetas padre recursivamente
                    // Necesitamos crear desde la raíz hasta el padre
                    std::vector<std::string> parts = Ext2Utils::splitPath(parentDir);
                    std::string currentPath = "/";
                    for (const auto& part : parts) {
                        std::string testPath = currentPath + part;
                        int inode = Ext2Utils::findInodeByPath(diskPath, testPath, sb, mbr, partitionIndex);
                        if (inode == -1) {
                            if (!Ext2Utils::createDirectory(diskPath, testPath, sb, mbr, partitionIndex, uid, gid)) {
                                result.message = "Error: No se pudo crear la carpeta: " + testPath;
                                return result;
                            }
                        }
                        currentPath = testPath + "/";
                    }
                } else {
                    result.message = "Error: La carpeta padre no existe en EXT2. Use -r para crearla automáticamente";
                    return result;
                }
            }
        }
        
        // 10. Generar contenido
        std::string content;
        if (!cont.empty()) {
            content = readFileContent(cont);
            if (content.empty() && size > 0) {
                // Si cont no tiene contenido, usar size
                for (int i = 0; i < size; i++) {
                    content += std::to_string(i % 10);
                }
            }
        } else if (size > 0) {
            for (int i = 0; i < size; i++) {
                content += std::to_string(i % 10);
            }
        }
        
        // 11. Escribir el archivo en EXT2
        if (!Ext2Utils::writeFile(diskPath, path, content, sb, mbr, partitionIndex, uid, gid)) {
            result.message = "Error: No se pudo crear el archivo en EXT2: " + path;
            return result;
        }
        
        // 12. Éxito
        result.success = true;
        result.message = "Archivo creado exitosamente en EXT2: " + path + " (Tamaño: " + std::to_string(content.length()) + " bytes)";
        result.data["file"] = {
            {"path", path},
            {"size", (int)content.length()},
            {"recursive", recursive},
            {"source_file", cont.empty() ? "generated" : cont},
            {"owner", currentSession.user},
            {"permissions", "664"}
        };
        
    } catch (const std::exception& e) {
        result.message = "Error al procesar MKFILE: " + std::string(e.what());
    }
    
    return result;
}