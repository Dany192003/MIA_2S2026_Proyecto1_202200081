#include "command_handler.h"
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <cstring>

// Función auxiliar para crear carpetas recursivamente
static bool createDirectoriesRecursive(const std::string& path) {
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
        
        // 4. Verificar si la carpeta ya existe
        struct stat buffer;
        if (stat(path.c_str(), &buffer) == 0) {
            if (S_ISDIR(buffer.st_mode)) {
                result.message = "Error: La carpeta ya existe: " + path;
            } else {
                result.message = "Error: Ya existe un archivo con ese nombre: " + path;
            }
            return result;
        }
        
        // 5. Crear la carpeta
        if (recursive) {
            if (!createDirectoriesRecursive(path)) {
                result.message = "Error: No se pudieron crear las carpetas para: " + path;
                return result;
            }
        } else {
            // Verificar que la carpeta padre exista
            size_t lastSlash = path.find_last_of('/');
            if (lastSlash != std::string::npos) {
                std::string parentDir = path.substr(0, lastSlash);
                if (stat(parentDir.c_str(), &buffer) != 0) {
                    result.message = "Error: La carpeta padre no existe. Use -p para crearla automáticamente";
                    return result;
                }
            }
            
            // Crear la carpeta
            if (mkdir(path.c_str(), 0755) != 0) {
                result.message = "Error: No se pudo crear la carpeta: " + path;
                return result;
            }
        }
        
        // 6. TODO: Crear inodo y bloques en el sistema EXT2
        
        // 7. Éxito
        result.success = true;
        result.message = "Carpeta creada exitosamente: " + path;
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