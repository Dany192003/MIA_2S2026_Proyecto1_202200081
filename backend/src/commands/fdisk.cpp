#include "command_handler.h"
#include <fstream>
#include <cstring>
#include <cmath>
#include <vector>
#include "../structures/ebr.h"

// Función auxiliar para obtener el tamaño del archivo
static int64_t getFileSize(const std::string& path) {
    std::ifstream file(path, std::ios::binary | std::ios::ate);
    if (!file.is_open()) return -1;
    return file.tellg();
}

// Función para leer un EBR en una posición específica
static bool readEBR(std::fstream& disk, int64_t position, EBR& ebr) {
    disk.seekg(position, std::ios::beg);
    disk.read(reinterpret_cast<char*>(&ebr), sizeof(EBR));
    return disk.good();
}

// Función para escribir un EBR en una posición específica
static bool writeEBR(std::fstream& disk, int64_t position, const EBR& ebr) {
    disk.seekp(position, std::ios::beg);
    disk.write(reinterpret_cast<const char*>(&ebr), sizeof(EBR));
    return disk.good();
}

// Función para encontrar el último EBR en una partición extendida
static int64_t findLastEBR(std::fstream& disk, int64_t startPosition) {
    EBR ebr;
    int64_t currentPos = startPosition;
    int64_t lastPos = startPosition;
    
    while (true) {
        if (!readEBR(disk, currentPos, ebr)) break;
        if (ebr.part_next == -1) break;
        currentPos = ebr.part_next;
        lastPos = currentPos;
    }
    
    return lastPos;
}

// Función para contar cuántas particiones lógicas hay en una extendida
static int countLogicalPartitions(std::fstream& disk, int64_t startPosition) {
    int count = 0;
    EBR ebr;
    int64_t currentPos = startPosition;
    
    while (true) {
        if (!readEBR(disk, currentPos, ebr)) break;
        if (ebr.part_s == 0) break;
        count++;
        if (ebr.part_next == -1) break;
        currentPos = ebr.part_next;
    }
    
    return count;
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
        
        // 7. Abrir el disco
        std::fstream disk(path, std::ios::in | std::ios::out | std::ios::binary);
        if (!disk.is_open()) {
            result.message = "Error: No se pudo abrir el disco: " + path;
            return result;
        }
        
        // 8. Leer el MBR
        MBR mbr;
        disk.seekg(0, std::ios::beg);
        disk.read(reinterpret_cast<char*>(&mbr), sizeof(MBR));
        
        // 9. Convertir tamaño a bytes
        int64_t sizeInBytes;
        if (unitChar == 'B') sizeInBytes = size;
        else if (unitChar == 'K') sizeInBytes = (int64_t)size * 1024;
        else sizeInBytes = (int64_t)size * 1024 * 1024;
        
        int64_t diskSize = getFileSize(path);
        if (diskSize == -1) {
            result.message = "Error: No se pudo obtener el tamaño del disco";
            return result;
        }
        
        // 10. Buscar espacios libres en el MBR
        int freeSlot = -1;
        int primaryCount = 0;
        int extendedCount = 0;
        int64_t usedSpace = sizeof(MBR);
        
        for (int i = 0; i < 4; i++) {
            if (mbr.mbr_partitions[i].part_s > 0) {
                usedSpace += mbr.mbr_partitions[i].part_s;
                if (mbr.mbr_partitions[i].part_type == 'P') primaryCount++;
                else if (mbr.mbr_partitions[i].part_type == 'E') extendedCount++;
            } else {
                if (freeSlot == -1) freeSlot = i;
            }
        }
        
        // 11. Validar según el tipo de partición
        if (typeChar == 'P') {
            // Partición primaria
            if (primaryCount + extendedCount >= 4) {
                result.message = "Error: Ya hay 4 particiones en el disco";
                return result;
            }
            if (freeSlot == -1) {
                result.message = "Error: No hay espacio para más particiones";
                return result;
            }
            
            // Verificar espacio libre
            int64_t freeSpace = diskSize - usedSpace;
            if (sizeInBytes > freeSpace) {
                result.message = "Error: No hay suficiente espacio libre en el disco";
                return result;
            }
            
            // Crear la partición primaria
            Partition newPartition;
            memset(&newPartition, 0, sizeof(Partition));
            newPartition.part_status = '0';
            newPartition.part_type = typeChar;
            newPartition.part_fit = fitChar;
            newPartition.part_start = usedSpace;
            newPartition.part_s = sizeInBytes;
            strncpy(newPartition.part_name, name.c_str(), 15);
            newPartition.part_correlative = -1;
            memset(newPartition.part_id, 0, 4);
            
            mbr.mbr_partitions[freeSlot] = newPartition;
            
            // Escribir el MBR actualizado
            disk.seekp(0, std::ios::beg);
            disk.write(reinterpret_cast<const char*>(&mbr), sizeof(MBR));
            disk.close();
            
            result.success = true;
            result.message = "Partición creada exitosamente: " + name + " (Tamaño: " + std::to_string(size) + unit + ", Tipo: p)";
            result.data["partition"] = {
                {"name", name},
                {"size", size},
                {"unit", unit},
                {"type", "p"},
                {"fit", fit},
                {"start", usedSpace},
                {"bytes", sizeInBytes}
            };
            return result;
            
        } else if (typeChar == 'E') {
            // Partición extendida
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
            
            int64_t freeSpace = diskSize - usedSpace;
            if (sizeInBytes > freeSpace) {
                result.message = "Error: No hay suficiente espacio libre en el disco";
                return result;
            }
            
            // Crear la partición extendida en el MBR
            Partition newPartition;
            memset(&newPartition, 0, sizeof(Partition));
            newPartition.part_status = '0';
            newPartition.part_type = typeChar;
            newPartition.part_fit = fitChar;
            newPartition.part_start = usedSpace;
            newPartition.part_s = sizeInBytes;
            strncpy(newPartition.part_name, name.c_str(), 15);
            newPartition.part_correlative = -1;
            memset(newPartition.part_id, 0, 4);
            
            mbr.mbr_partitions[freeSlot] = newPartition;
            
            // Crear el primer EBR en el inicio de la partición extendida
            EBR firstEbr;
            memset(&firstEbr, 0, sizeof(EBR));
            firstEbr.part_mount = '0';
            firstEbr.part_fit = fitChar;
            firstEbr.part_start = usedSpace + sizeof(EBR);
            firstEbr.part_s = 0;  // Aún no hay lógicas
            firstEbr.part_next = -1;
            strncpy(firstEbr.part_name, name.c_str(), 15);
            
            disk.seekp(usedSpace, std::ios::beg);
            disk.write(reinterpret_cast<const char*>(&firstEbr), sizeof(EBR));
            
            // Escribir el MBR actualizado
            disk.seekp(0, std::ios::beg);
            disk.write(reinterpret_cast<const char*>(&mbr), sizeof(MBR));
            disk.close();
            
            result.success = true;
            result.message = "Partición extendida creada exitosamente: " + name + " (Tamaño: " + std::to_string(size) + unit + ", Tipo: e)";
            result.data["partition"] = {
                {"name", name},
                {"size", size},
                {"unit", unit},
                {"type", "e"},
                {"fit", fit},
                {"start", usedSpace},
                {"bytes", sizeInBytes}
            };
            return result;
            
        } else if (typeChar == 'L') {
            // Partición lógica
            // Buscar la partición extendida
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
            
            int64_t extendedStart = mbr.mbr_partitions[extendedSlot].part_start;
            int64_t extendedSize = mbr.mbr_partitions[extendedSlot].part_s;
            int64_t extendedEnd = extendedStart + extendedSize;
            
            // Leer el primer EBR
            EBR currentEbr;
            if (!readEBR(disk, extendedStart, currentEbr)) {
                result.message = "Error: No se pudo leer el EBR de la partición extendida";
                return result;
            }
            
            // Encontrar el último EBR y el espacio disponible
            int64_t lastEbrPos = extendedStart;
            int64_t nextFreePos = extendedStart + sizeof(EBR);
            int logicalCount = 0;
            
            if (currentEbr.part_s == 0) {
                // No hay lógicas aún
                lastEbrPos = extendedStart;
                nextFreePos = extendedStart + sizeof(EBR);
                logicalCount = 0;
            } else {
                // Recorrer los EBRs
                int64_t currentPos = extendedStart;
                while (true) {
                    EBR tempEbr;
                    if (!readEBR(disk, currentPos, tempEbr)) break;
                    if (tempEbr.part_s == 0) break;
                    
                    logicalCount++;
                    lastEbrPos = currentPos;
                    nextFreePos = tempEbr.part_start + tempEbr.part_s;
                    
                    if (tempEbr.part_next == -1) break;
                    currentPos = tempEbr.part_next;
                }
            }
            
            // Verificar que no se exceda el tamaño de la extendida
            int64_t ebrSize = sizeof(EBR);
            int64_t totalNeeded = ebrSize + sizeInBytes;
            if (nextFreePos + totalNeeded > extendedEnd) {
                result.message = "Error: No hay suficiente espacio en la partición extendida";
                return result;
            }
            
            // Crear el nuevo EBR
            EBR newEbr;
            memset(&newEbr, 0, sizeof(EBR));
            newEbr.part_mount = '0';
            newEbr.part_fit = fitChar;
            newEbr.part_start = nextFreePos + ebrSize;
            newEbr.part_s = sizeInBytes;
            newEbr.part_next = -1;
            strncpy(newEbr.part_name, name.c_str(), 15);
            
            // Si hay EBRs previos, actualizar el part_next del último
            if (logicalCount > 0) {
                EBR lastEbr;
                if (readEBR(disk, lastEbrPos, lastEbr)) {
                    lastEbr.part_next = nextFreePos;
                    writeEBR(disk, lastEbrPos, lastEbr);
                }
            }
            
            // Escribir el nuevo EBR
            disk.seekp(nextFreePos, std::ios::beg);
            disk.write(reinterpret_cast<const char*>(&newEbr), sizeof(EBR));
            
            // Si es la primera lógica, actualizar el EBR de la extendida
            if (logicalCount == 0) {
                EBR firstEbr;
                if (readEBR(disk, extendedStart, firstEbr)) {
                    firstEbr.part_s = sizeInBytes;
                    firstEbr.part_next = nextFreePos;
                    writeEBR(disk, extendedStart, firstEbr);
                }
            }
            
            disk.close();
            
            result.success = true;
            result.message = "Partición lógica creada exitosamente: " + name + " (Tamaño: " + std::to_string(size) + unit + ", Tipo: l)";
            result.data["partition"] = {
                {"name", name},
                {"size", size},
                {"unit", unit},
                {"type", "l"},
                {"fit", fit},
                {"start", nextFreePos + ebrSize},
                {"bytes", sizeInBytes}
            };
            return result;
        }
        
        result.message = "Error: Tipo de partición no reconocido";
        return result;
        
    } catch (const std::exception& e) {
        result.message = "Error al procesar FDISK: " + std::string(e.what());
        return result;
    }
}