#include "command_handler.h"
#include <fstream>
#include <cstring>
#include <cmath>

// Función auxiliar para obtener el tamaño del archivo
static int64_t getFileSize(const std::string& path) {
    std::ifstream file(path, std::ios::binary | std::ios::ate);
    if (!file.is_open()) return -1;
    return file.tellg();
}

CommandResult CommandHandler::processFdisk(const json& params) {
    CommandResult result;
    result.success = false;
    
    try {
        // 1. Obtener parámetros
        std::string path = params["path"];
        std::string name = params["name"];
        int size = std::stoi(std::string(params["size"]));
        std::string unit = params.contains("unit") ? std::string(params["unit"]) : "k";
        std::string type = params.contains("type") ? std::string(params["type"]) : "p";
        std::string fit = params.contains("fit") ? std::string(params["fit"]) : "wf";
        
        // Convertir a minúsculas
        std::transform(unit.begin(), unit.end(), unit.begin(), ::tolower);
        std::transform(type.begin(), type.end(), type.begin(), ::tolower);
        std::transform(fit.begin(), fit.end(), fit.begin(), ::tolower);
        
        // 2. Validar tamaño
        if (size <= 0) {
            result.message = "Error: El tamaño debe ser mayor que cero";
            return result;
        }
        
        // 3. Validar unidad (B, K, M)
        char unitChar;
        if (unit == "b") unitChar = 'B';
        else if (unit == "k") unitChar = 'K';
        else if (unit == "m") unitChar = 'M';
        else {
            result.message = "Error: Unidad inválida. Use B, K o M";
            return result;
        }
        
        // 4. Validar tipo (P, E, L)
        char typeChar;
        if (type == "p") typeChar = 'P';
        else if (type == "e") typeChar = 'E';
        else if (type == "l") typeChar = 'L';
        else {
            result.message = "Error: Tipo inválido. Use P, E o L";
            return result;
        }
        
        // 5. Validar fit (BF, FF, WF)
        char fitChar;
        if (fit == "bf") fitChar = 'B';
        else if (fit == "ff") fitChar = 'F';
        else if (fit == "wf") fitChar = 'W';
        else {
            result.message = "Error: Ajuste inválido. Use BF, FF o WF";
            return result;
        }
        
        // 6. Validar que el disco exista
        if (!validateDiskExists(path)) {
            result.message = "Error: El disco no existe en la ruta: " + path;
            return result;
        }
        
        // 7. Validar que el nombre no esté repetido
        if (!validatePartitionName(path, name)) {
            result.message = "Error: Ya existe una partición con el nombre: " + name;
            return result;
        }
        
        // 8. Abrir el disco
        std::fstream disk(path, std::ios::in | std::ios::out | std::ios::binary);
        if (!disk.is_open()) {
            result.message = "Error: No se pudo abrir el disco: " + path;
            return result;
        }
        
        // 9. Leer el MBR
        MBR mbr;
        disk.seekg(0, std::ios::beg);
        disk.read(reinterpret_cast<char*>(&mbr), sizeof(MBR));
        
        // 10. Verificar límite de particiones primarias/extendidas
        int primaryCount = 0;
        int extendedCount = 0;
        int freeSlot = -1;
        int64_t diskSize = getFileSize(path);
        
        for (int i = 0; i < 4; i++) {
            if (mbr.mbr_partitions[i].part_s > 0) {
                if (mbr.mbr_partitions[i].part_type == 'P') {
                    primaryCount++;
                } else if (mbr.mbr_partitions[i].part_type == 'E') {
                    extendedCount++;
                }
            } else {
                if (freeSlot == -1) {
                    freeSlot = i;
                }
            }
        }
        
        // 11. Validar según el tipo
        if (typeChar == 'P') {
            if (primaryCount + extendedCount >= 4) {
                result.message = "Error: Ya hay 4 particiones en el disco";
                return result;
            }
            if (freeSlot == -1) {
                result.message = "Error: No hay espacio para más particiones";
                return result;
            }
        } else if (typeChar == 'E') {
            if (extendedCount >= 1) {
                result.message = "Error: Ya existe una partición extendida en el disco";
                return result;
            }
            if (primaryCount + extendedCount >= 4) {
                result.message = "Error: No se puede crear partición extendida, ya hay 4 particiones";
                return result;
            }
            if (freeSlot == -1) {
                result.message = "Error: No hay espacio para más particiones";
                return result;
            }
        } else if (typeChar == 'L') {
            // Buscar partición extendida
            int extendedSlot = -1;
            for (int i = 0; i < 4; i++) {
                if (mbr.mbr_partitions[i].part_type == 'E' && mbr.mbr_partitions[i].part_s > 0) {
                    extendedSlot = i;
                    break;
                }
            }
            if (extendedSlot == -1) {
                result.message = "Error: No existe partición extendida para crear una lógica";
                return result;
            }
        }
        
        // 12. Convertir tamaño a bytes
        int64_t sizeInBytes;
        if (unitChar == 'B') sizeInBytes = size;
        else if (unitChar == 'K') sizeInBytes = (int64_t)size * 1024;
        else sizeInBytes = (int64_t)size * 1024 * 1024;
        
        // 13. Calcular espacio disponible
        int64_t usedSpace = 0;
        for (int i = 0; i < 4; i++) {
            usedSpace += mbr.mbr_partitions[i].part_s;
        }
        
        // Espacio libre desde el final del MBR
        int64_t freeSpace = diskSize - sizeof(MBR) - usedSpace;
        
        // Ajuste según fit
        int64_t startPosition = sizeof(MBR) + usedSpace;
        
        if (fitChar == 'F') {
            // First Fit: usar el primer espacio disponible
            startPosition = sizeof(MBR) + usedSpace;
        } else if (fitChar == 'B') {
            // Best Fit: buscar el mejor espacio (para simplificar, usamos el mismo)
            startPosition = sizeof(MBR) + usedSpace;
        } else if (fitChar == 'W') {
            // Worst Fit: usar el espacio más grande (para simplificar, usamos el mismo)
            startPosition = sizeof(MBR) + usedSpace;
        }
        
        if (sizeInBytes > freeSpace) {
            result.message = "Error: No hay suficiente espacio libre en el disco";
            return result;
        }
        
        // 14. Crear la partición
        Partition newPartition;
        memset(&newPartition, 0, sizeof(Partition));
        newPartition.part_status = '0';
        newPartition.part_type = typeChar;
        newPartition.part_fit = fitChar;
        newPartition.part_start = startPosition;
        newPartition.part_s = sizeInBytes;
        strncpy(newPartition.part_name, name.c_str(), 15);
        newPartition.part_correlative = -1;
        memset(newPartition.part_id, 0, 4);
        
        // 15. Escribir la partición en el MBR
        int slot = freeSlot;
        if (typeChar == 'L') {
            // Para lógicas, se escriben en el EBR
            // TODO: Implementar EBR para particiones lógicas
            result.message = "Error: Particiones lógicas aún no implementadas";
            return result;
        }
        
        mbr.mbr_partitions[slot] = newPartition;
        
        // 16. Escribir el MBR actualizado
        disk.seekp(0, std::ios::beg);
        disk.write(reinterpret_cast<const char*>(&mbr), sizeof(MBR));
        disk.close();
        
        // 17. Éxito
        result.success = true;
        result.message = "Partición creada exitosamente: " + name + " (Tamaño: " + std::to_string(size) + unit + ", Tipo: " + type + ")";
        result.data["partition"] = {
            {"name", name},
            {"size", size},
            {"unit", unit},
            {"type", type},
            {"fit", fit},
            {"start", startPosition},
            {"bytes", sizeInBytes}
        };
        
    } catch (const std::exception& e) {
        result.message = "Error al procesar FDISK: " + std::string(e.what());
    }
    
    return result;
}