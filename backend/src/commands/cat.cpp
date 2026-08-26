#include "command_handler.h"
#include <fstream>
#include <sstream>

CommandResult CommandHandler::processCat(const json& params) {
    CommandResult result;
    result.success = false;
    
    try {
        // 1. Obtener parámetros (file1, file2, file3, ...)
        std::vector<std::string> files;
        for (int i = 1; ; i++) {
            std::string key = "file" + std::to_string(i);
            if (params.contains(key)) {
                files.push_back(params[key]);
            } else {
                break;
            }
        }
        
        // 2. Validar que haya sesión activa
        if (!isLoggedIn()) {
            result.message = "Error: No hay sesión activa. Use LOGIN primero.";
            return result;
        }
        
        // 3. Validar que haya al menos un archivo
        if (files.empty()) {
            result.message = "Error: Se requiere al menos un archivo (file1, file2, ...)";
            return result;
        }
        
        // 4. Leer cada archivo
        std::string output;
        for (const auto& filePath : files) {
            // Verificar que el archivo exista
            if (!validateDiskExists(filePath)) {
                result.message = "Error: El archivo no existe: " + filePath;
                return result;
            }
            
            // TODO: Validar permisos de lectura según el usuario actual
            
            // Leer el archivo
            std::ifstream file(filePath);
            if (!file.is_open()) {
                result.message = "Error: No se pudo leer el archivo: " + filePath;
                return result;
            }
            
            std::stringstream buffer;
            buffer << file.rdbuf();
            if (!output.empty()) {
                output += "\n";
            }
            output += buffer.str();
            file.close();
        }
        
        // 5. Éxito
        result.success = true;
        result.message = "Contenido de " + std::to_string(files.size()) + " archivo(s)";
        result.data["content"] = output;
        result.data["files"] = files;
        
    } catch (const std::exception& e) {
        result.message = "Error al procesar CAT: " + std::string(e.what());
    }
    
    return result;
}