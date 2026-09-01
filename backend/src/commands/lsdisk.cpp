#include "command_handler.h"
#include <filesystem>
#include <fstream>
#include <cstring>

namespace fs = std::filesystem;

CommandResult CommandHandler::processLsdisk(const json& params) {
    CommandResult result;
    result.success = false;
    
    try {
        // 1. Escanear la carpeta "discos/" en el directorio del proyecto
        std::string diskDir = "/home/Edwin/Desktop/MIA_2S2026_Proyecto1_202200081/discos/";
        
        json diskList = json::array();
        
        // 2. Recorrer archivos .mia en la carpeta
        for (const auto& entry : fs::directory_iterator(diskDir)) {
            if (entry.path().extension() == ".mia") {
                json diskInfo;
                diskInfo["name"] = entry.path().filename().string();
                diskInfo["path"] = entry.path().string();
                diskInfo["size"] = entry.file_size();
                
                // 3. Leer el MBR del disco
                std::ifstream diskFile(entry.path(), std::ios::binary);
                if (diskFile.is_open()) {
                    MBR mbr;
                    diskFile.read(reinterpret_cast<char*>(&mbr), sizeof(MBR));
                    diskFile.close();
                    
                    json partitions = json::array();
                    int partCount = 0;
                    
                    for (int i = 0; i < 4; i++) {
                        if (mbr.mbr_partitions[i].part_s > 0) {
                            json part;
                            part["name"] = std::string(mbr.mbr_partitions[i].part_name, 16).c_str();
                            part["type"] = std::string(1, mbr.mbr_partitions[i].part_type);
                            part["size"] = mbr.mbr_partitions[i].part_s;
                            part["start"] = mbr.mbr_partitions[i].part_start;
                            part["status"] = std::string(1, mbr.mbr_partitions[i].part_status);
                            part["id"] = std::string(mbr.mbr_partitions[i].part_id, 4).c_str();
                            partitions.push_back(part);
                            partCount++;
                        }
                    }
                    
                    diskInfo["partitions"] = partitions;
                    diskInfo["partition_count"] = partCount;
                } else {
                    diskInfo["partitions"] = json::array();
                    diskInfo["partition_count"] = 0;
                }
                
                diskList.push_back(diskInfo);
            }
        }
        
        result.data["disks"] = diskList;
        result.data["total"] = diskList.size();
        result.message = "Discos encontrados: " + std::to_string(diskList.size());
        result.success = true;
        
    } catch (const std::exception& e) {
        result.message = "Error al procesar LSDISK: " + std::string(e.what());
    }
    
    return result;
}
