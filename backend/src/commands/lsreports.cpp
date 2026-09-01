#include "command_handler.h"
#include <filesystem>
#include <fstream>
#include <cstring>

namespace fs = std::filesystem;

CommandResult CommandHandler::processLsreports(const json& params) {
    CommandResult result;
    result.success = false;
    
    try {
        std::string reportsDir = "/home/Edwin/Desktop/MIA_2S2026_Proyecto1_202200081/reports/";
        
        json reportList = json::array();
        
        if (!fs::exists(reportsDir)) {
            result.message = "La carpeta reports/ no existe";
            result.data["reports"] = json::array();
            result.data["total"] = 0;
            result.success = true;
            return result;
        }
        
        for (const auto& entry : fs::directory_iterator(reportsDir)) {
            if (entry.is_regular_file()) {
                std::string filename = entry.path().filename().string();
                std::string ext = entry.path().extension().string();
                
                // Solo incluir .png, .txt y .dot
                if (ext == ".png" || ext == ".txt" || ext == ".dot") {
                    json item;
                    item["name"] = filename;
                    item["path"] = entry.path().string();
                    item["size"] = entry.file_size();
                    item["extension"] = ext;
                    
                    // Determinar tipo
                    if (ext == ".png") item["type"] = "image";
                    else if (ext == ".txt") item["type"] = "text";
                    else if (ext == ".dot") item["type"] = "dot";
                    
                    reportList.push_back(item);
                }
            }
        }
        
        result.data["reports"] = reportList;
        result.data["total"] = reportList.size();
        result.message = "Reportes encontrados: " + std::to_string(reportList.size());
        result.success = true;
        
    } catch (const std::exception& e) {
        result.message = "Error al procesar LSREPORTS: " + std::string(e.what());
    }
    
    return result;
}
