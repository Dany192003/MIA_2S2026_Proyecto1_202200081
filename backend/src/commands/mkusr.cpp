#include "command_handler.h"
#include "../utils/ext2_utils.h"

CommandResult CommandHandler::processMkusr(const json& params) {
    CommandResult result;
    result.success = false;
    
    try {
        std::string user = params["user"];
        std::string pass = params["pass"];
        std::string grp = params["grp"];
        
        if (!isLoggedIn()) {
            result.message = "Error: No hay sesión activa. Use LOGIN primero.";
            return result;
        }
        
        if (!isRoot()) {
            result.message = "Error: Solo el usuario root puede crear usuarios";
            return result;
        }
        
        if (user.length() > 10 || pass.length() > 10 || grp.length() > 10) {
            result.message = "Error: Los campos no pueden exceder 10 caracteres";
            return result;
        }
        
        std::string diskPath = currentSession.diskPath;
        std::string mountId = currentSession.mountId;
        
        // Leer el MBR
        std::fstream disk(diskPath, std::ios::in | std::ios::out | std::ios::binary);
        if (!disk.is_open()) {
            result.message = "Error: No se pudo abrir el disco: " + diskPath;
            return result;
        }
        
        MBR mbr;
        disk.seekg(0, std::ios::beg);
        disk.read(reinterpret_cast<char*>(&mbr), sizeof(MBR));
        
        // Buscar la partición
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
            result.message = "Error: No se encontró la partición para el ID: " + mountId;
            return result;
        }
        
        // Leer users.txt
        std::vector<std::string> userLines = Ext2Utils::readUsersFile(diskPath, mountId, mbr, partitionIndex);
        
        // Verificar que el usuario no exista
        for (const auto& line : userLines) {
            if (line.find(", U, ") != std::string::npos) {
                std::stringstream ss(line);
                std::string token;
                std::vector<std::string> parts;
                while (std::getline(ss, token, ',')) {
                    parts.push_back(token);
                }
                if (parts.size() >= 4) {
                    std::string username = parts[3];
                    username.erase(0, username.find_first_not_of(" "));
                    username.erase(username.find_last_not_of(" ") + 1);
                    if (username == user) {
                        result.message = "Error: El usuario ya existe: " + user;
                        return result;
                    }
                }
            }
        }
        
        // Verificar que el grupo exista
        bool groupFound = false;
        int nextUid = 2;
        for (const auto& line : userLines) {
            if (line.find(", G, ") != std::string::npos) {
                std::stringstream ss(line);
                std::string token;
                std::vector<std::string> parts;
                while (std::getline(ss, token, ',')) {
                    parts.push_back(token);
                }
                if (parts.size() >= 3) {
                    std::string groupName = parts[2];
                    groupName.erase(0, groupName.find_first_not_of(" "));
                    groupName.erase(groupName.find_last_not_of(" ") + 1);
                    int gid = std::stoi(parts[0]);
                    if (groupName == grp && gid != 0) {
                        groupFound = true;
                    }
                    if (gid >= nextUid) {
                        nextUid = gid + 1;
                    }
                }
            }
        }
        
        if (!groupFound) {
            result.message = "Error: El grupo no existe: " + grp;
            return result;
        }
        
        // Agregar el usuario
        std::string newUserLine = std::to_string(nextUid) + ", U, " + grp + ", " + user + ", " + pass;
        userLines.push_back(newUserLine);
        
        // Guardar users.txt
        Ext2Utils::writeUsersFile(diskPath, userLines, mbr, partitionIndex);
        disk.close();
        
        result.success = true;
        result.message = "Usuario creado exitosamente: " + user + " (Grupo: " + grp + ")";
        result.data["user"] = {{"user", user}, {"group", grp}};
        
    } catch (const std::exception& e) {
        result.message = "Error al procesar MKUSR: " + std::string(e.what());
    }
    
    return result;
}