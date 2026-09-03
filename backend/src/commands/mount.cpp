#include "command_handler.h"
#include <fstream>
#include <cstring>
#include <map>

// Función para generar ID de montaje según el PDF
static std::string generateMountId(const std::string& carnet, const std::string& diskPath, int partitionNumber) {
    // Últimos dos dígitos del carnet: 202200081 -> 81
    std::string lastTwo = carnet.substr(carnet.length() - 2);
    
    // Letra según el disco (A, B, C, ...)
    static std::map<std::string, char> diskLetter;
    if (diskLetter.find(diskPath) == diskLetter.end()) {
        char nextLetter = 'A' + diskLetter.size();
        diskLetter[diskPath] = nextLetter;
    }
    
    std::string id = lastTwo + std::to_string(partitionNumber) + diskLetter[diskPath];
    return id;
}

CommandResult CommandHandler::processMount(const json& params) {
    CommandResult result;
    result.success = false;
    
    try {
        // 1. Obtener parámetros
        std::string path = params["path"];
        std::string name = params["name"];
        
        // 2. Validar que el disco exista
        if (!validateDiskExists(path)) {
            result.message = "Error: El disco no existe en la ruta: " + path;
            return result;
        }
        
        // 3. Abrir el disco y leer el MBR
        std::fstream disk(path, std::ios::in | std::ios::out | std::ios::binary);
        if (!disk.is_open()) {
            result.message = "Error: No se pudo abrir el disco: " + path;
            return result;
        }
        
        MBR mbr;
        disk.seekg(0, std::ios::beg);
        disk.read(reinterpret_cast<char*>(&mbr), sizeof(MBR));
        
        // 4. Buscar la partición por nombre
        int partitionIndex = -1;
        for (int i = 0; i < 4; i++) {
            std::string partName(mbr.mbr_partitions[i].part_name);
            partName = partName.c_str();
            if (partName == name && mbr.mbr_partitions[i].part_s > 0) {
                partitionIndex = i;
                break;
            }
        }
        
        if (partitionIndex == -1) {
            disk.close();
            result.message = "Error: No existe la partición: " + name + " en el disco: " + path;
            return result;
        }
        
        // 5. Verificar que no sea una partición extendida
        if (mbr.mbr_partitions[partitionIndex].part_type == 'E') {
            disk.close();
            result.message = "Error: No se puede montar una partición extendida";
            return result;
        }
        
        // 6. Calcular número de partición (correlative)
        int partitionNumber = 1;
        for (int i = 0; i < 4; i++) {
            if (mbr.mbr_partitions[i].part_correlative > 0) {
                partitionNumber++;
            }
        }
        
        // 7. Generar ID con el carnet
        std::string carnet = "202200081";
        std::string mountId = generateMountId(carnet, path, partitionNumber);
        
        // 8. Actualizar la partición en el MBR
        mbr.mbr_partitions[partitionIndex].part_status = '1';
        mbr.mbr_partitions[partitionIndex].part_correlative = partitionNumber;
        memset(mbr.mbr_partitions[partitionIndex].part_id, 0, 4);
        strncpy(mbr.mbr_partitions[partitionIndex].part_id, mountId.c_str(), 4);
        
        // 9. Escribir el MBR actualizado
        disk.seekp(0, std::ios::beg);
        disk.write(reinterpret_cast<const char*>(&mbr), sizeof(MBR));
        disk.close();
        
        // 10. Guardar en memoria
        mountedDisks[mountId] = path;
        
        // 11. Éxito
        result.success = true;
        result.message = "Partición montada exitosamente. ID: " + mountId;
        result.data["mount"] = {
            {"id", mountId},
            {"path", path},
            {"name", name},
            {"partition", partitionIndex},
            {"correlative", partitionNumber}
        };
        
    } catch (const std::exception& e) {
        result.message = "Error al procesar MOUNT: " + std::string(e.what());
    }
    
    return result;
}