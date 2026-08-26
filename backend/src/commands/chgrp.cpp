#include "command_handler.h"

CommandResult CommandHandler::processChgrp(const json& params) {
    CommandResult result;
    result.success = false;
    
    try {
        // 1. Obtener parámetros
        std::string user = params["user"];
        std::string grp = params["grp"];
        
        // 2. Validar que haya sesión activa
        if (!isLoggedIn()) {
            result.message = "Error: No hay sesión activa. Use LOGIN primero.";
            return result;
        }
        
        // 3. Validar que sea usuario root
        if (!isRoot()) {
            result.message = "Error: Solo el usuario root puede cambiar grupos";
            return result;
        }
        
        // 4. Validar que el usuario exista
        // TODO: Implementar verificación real desde users.txt
        
        // 5. Validar que el grupo exista
        // TODO: Implementar verificación real desde users.txt
        
        // 6. Cambiar el grupo del usuario
        // TODO: Implementar actualización en users.txt
        
        // 7. Éxito
        result.success = true;
        result.message = "Grupo cambiado exitosamente para el usuario: " + user + " (Nuevo grupo: " + grp + ")";
        result.data["change"] = {
            {"user", user},
            {"new_group", grp}
        };
        
    } catch (const std::exception& e) {
        result.message = "Error al procesar CHGRP: " + std::string(e.what());
    }
    
    return result;
}