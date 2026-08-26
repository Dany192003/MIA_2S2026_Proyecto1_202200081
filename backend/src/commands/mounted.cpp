#include "command_handler.h"

CommandResult CommandHandler::processMounted(const json& params) {
    CommandResult result;
    result.success = false;
    
    try {
        // 1. Verificar que haya particiones montadas
        if (mountedDisks.empty()) {
            result.message = "No hay particiones montadas";
            result.success = true;
            result.data["mounted"] = json::array();
            return result;
        }
        
        // 2. Crear lista de particiones montadas
        json mountedList = json::array();
        for (const auto& mount : mountedDisks) {
            json item;
            item["id"] = mount.first;
            item["disk"] = mount.second;
            mountedList.push_back(item);
        }
        
        // 3. Éxito
        result.success = true;
        result.message = "Particiones montadas: " + std::to_string(mountedDisks.size());
        result.data["mounted"] = mountedList;
        
    } catch (const std::exception& e) {
        result.message = "Error al procesar MOUNTED: " + std::string(e.what());
    }
    
    return result;
}