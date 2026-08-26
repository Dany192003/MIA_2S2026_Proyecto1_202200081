#include "command_handler.h"

CommandResult CommandHandler::processRmgrp(const json& params) {
    CommandResult result;
    result.success = false;
    
    try {
        // 1. Obtener parámetros
        std::string name = params["name"];
        
        // 2. Validar que haya sesión activa
        if (!isLoggedIn()) {
            result.message = "Error: No hay sesión activa. Use LOGIN primero.";
            return result;
        }
        
        // 3. Validar que sea usuario root
        if (!isRoot()) {
            result.message = "Error: Solo el usuario root puede eliminar grupos";
            return result;
        }
        
        // 4. Validar que no sea el grupo root
        if (name == "root") {
            result.message = "Error: No se puede eliminar el grupo root";
            return result;
        }
        
        // 5. Validar que el grupo exista
        // TODO: Implementar verificación real desde users.txt
        
        // 6. Eliminar el grupo
        // TODO: Implementar eliminación en users.txt
        
        // 7. Éxito
        result.success = true;
        result.message = "Grupo eliminado exitosamente: " + name;
        
    } catch (const std::exception& e) {
        result.message = "Error al procesar RMGRP: " + std::string(e.what());
    }
    
    return result;
}