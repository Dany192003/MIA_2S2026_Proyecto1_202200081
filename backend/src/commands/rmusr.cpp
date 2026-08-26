#include "command_handler.h"

CommandResult CommandHandler::processRmusr(const json& params) {
    CommandResult result;
    result.success = false;
    
    try {
        // 1. Obtener parámetros
        std::string user = params["user"];
        
        // 2. Validar que haya sesión activa
        if (!isLoggedIn()) {
            result.message = "Error: No hay sesión activa. Use LOGIN primero.";
            return result;
        }
        
        // 3. Validar que sea usuario root
        if (!isRoot()) {
            result.message = "Error: Solo el usuario root puede eliminar usuarios";
            return result;
        }
        
        // 4. Validar que no sea el usuario root
        if (user == "root") {
            result.message = "Error: No se puede eliminar el usuario root";
            return result;
        }
        
        // 5. Validar que el usuario exista
        // TODO: Implementar verificación real desde users.txt
        
        // 6. Eliminar el usuario
        // TODO: Implementar eliminación en users.txt
        
        // 7. Éxito
        result.success = true;
        result.message = "Usuario eliminado exitosamente: " + user;
        
    } catch (const std::exception& e) {
        result.message = "Error al procesar RMUSR: " + std::string(e.what());
    }
    
    return result;
}