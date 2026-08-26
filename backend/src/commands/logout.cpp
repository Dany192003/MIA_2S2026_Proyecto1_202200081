#include "command_handler.h"

CommandResult CommandHandler::processLogout(const json& params) {
    CommandResult result;
    result.success = false;
    
    try {
        // 1. Validar que haya sesión activa
        if (!currentSession.active) {
            result.message = "Error: No hay sesión activa";
            return result;
        }
        
        // 2. Cerrar sesión
        std::string user = currentSession.user;
        currentSession.active = false;
        currentSession.user = "";
        currentSession.mountId = "";
        currentSession.diskPath = "";
        currentSession.uid = -1;
        currentSession.gid = -1;
        
        // 3. Éxito
        result.success = true;
        result.message = "Sesión cerrada exitosamente. Usuario: " + user;
        
    } catch (const std::exception& e) {
        result.message = "Error al procesar LOGOUT: " + std::string(e.what());
    }
    
    return result;
}