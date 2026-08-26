#include "command_handler.h"
#include <fstream>
#include <sstream>
#include <cstring>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>

// Función auxiliar para crear carpetas recursivamente
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
        
        // 5. Verificar si el archivo ya existe
        struct stat buffer;
        bool fileExists = (stat(path.c_str(), &buffer) == 0);
        
        if (fileExists) {
            result.message = "Error: El archivo ya existe. Use -f para sobrescribir";
            result.data["fileExists"] = true;
            return result;
        }
        
        // 6. Crear carpetas si es necesario
        if (recursive) {
            if (!createDirectoriesForFile(path)) {
                result.message = "Error: No se pudieron crear las carpetas para: " + path;
                return result;
            }
        } else {
            // Verificar que la carpeta padre exista
            size_t lastSlash = path.find_last_of('/');
            if (lastSlash != std::string::npos) {
                std::string parentDir = path.substr(0, lastSlash);
                if (stat(parentDir.c_str(), &buffer) != 0) {
                    result.message = "Error: La carpeta padre no existe. Use -r para crearla automáticamente";
                    return result;
                }
            }
        }
        
        // 7. Obtener contenido del archivo
        std::string content;
        
        if (!cont.empty()) {
            // Leer contenido de archivo físico
            if (!validateDiskExists(cont)) {
                result.message = "Error: El archivo de origen no existe: " + cont;
                return result;
            }
            content = readFileContent(cont);
            if (size == 0 && !content.empty()) {
                size = content.length();
            }
        } else if (size > 0) {
            // Generar números 0-9
            for (int i = 0; i < size; i++) {
                content += std::to_string(i % 10);
            }
        } else {
            content = "";
        }
        
        // 8. Escribir el archivo
        std::ofstream file(path);
        if (!file.is_open()) {
            result.message = "Error: No se pudo crear el archivo: " + path;
            return result;
        }
        file << content;
        file.close();
        
        // 9. TODO: Crear inodo y bloques en el sistema EXT2
        
        // 10. Éxito
        result.success = true;
        result.message = "Archivo creado exitosamente: " + path;
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