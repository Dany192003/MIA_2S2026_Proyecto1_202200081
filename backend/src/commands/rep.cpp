#include "command_handler.h"
#include <fstream>
#include <sstream>
#include <cstring>
#include <iomanip>
#include <sys/stat.h>    
#include <sys/types.h>   
#include <errno.h>

// Función auxiliar para crear carpetas
static bool createReportDirectories(const std::string& path) {
    size_t pos = 0;
    std::string currentPath;
    
    if (path[0] == '/') {
        currentPath = "/";
        pos = 1;
    }
    
    while (pos < path.length()) {
        size_t nextSlash = path.find('/', pos);
        if (nextSlash == std::string::npos) {
            break;
        }
        
        std::string dir = path.substr(0, nextSlash);
        if (dir.length() > 0 && dir != "/") {
            if (mkdir(dir.c_str(), 0755) != 0 && errno != EEXIST) {
                return false;
            }
        }
        pos = nextSlash + 1;
    }
    return true;
}

// Función para generar reporte MBR
static std::string generateMBRReport(const MBR& mbr, const std::string& diskPath) {
    std::stringstream ss;
    
    ss << "========================================\n";
    ss << "          REPORTE MBR - DISCO\n";
    ss << "========================================\n\n";
    
    ss << "Información del Disco:\n";
    ss << "  Ruta: " << diskPath << "\n";
    ss << "  Tamaño: " << mbr.mbr_tamano << " bytes\n";
    ss << "  Fecha creación: " << ctime(&mbr.mbr_fecha_creacion);
    ss << "  Signature: " << mbr.mbr_dsk_signature << "\n";
    ss << "  Ajuste: " << mbr.dsk_fit << "\n\n";
    
    ss << "----------------------------------------\n";
    ss << "          PARTICIONES\n";
    ss << "----------------------------------------\n\n";
    
    for (int i = 0; i < 4; i++) {
        const auto& p = mbr.mbr_partitions[i];
        ss << "Partición " << (i + 1) << ":\n";
        ss << "  Estado: " << (p.part_status == '1' ? "Montada" : "Libre") << "\n";
        ss << "  Tipo: " << (p.part_type == 'P' ? "Primaria" : p.part_type == 'E' ? "Extendida" : "Desconocido") << "\n";
        ss << "  Ajuste: " << p.part_fit << "\n";
        ss << "  Inicio: " << p.part_start << " bytes\n";
        ss << "  Tamaño: " << p.part_s << " bytes\n";
        ss << "  Nombre: " << p.part_name << "\n";
        ss << "  Correlativo: " << p.part_correlative << "\n";
        ss << "  ID: " << p.part_id << "\n\n";
    }
    
    return ss.str();
}

// Función para generar reporte DISK
static std::string generateDiskReport(const MBR& mbr, const std::string& diskPath) {
    std::stringstream ss;
    
    ss << "========================================\n";
    ss << "          REPORTE DISK - DISCO\n";
    ss << "========================================\n\n";
    
    int64_t totalSize = mbr.mbr_tamano;
    int64_t usedSize = 0;
    
    for (int i = 0; i < 4; i++) {
        usedSize += mbr.mbr_partitions[i].part_s;
    }
    
    int64_t freeSize = totalSize - usedSize - sizeof(MBR);
    
    ss << "Resumen del Disco:\n";
    ss << "  Tamaño total: " << totalSize << " bytes\n";
    ss << "  Espacio usado: " << usedSize << " bytes\n";
    ss << "  Espacio libre: " << freeSize << " bytes\n\n";
    
    ss << "Distribución de Espacio:\n";
    ss << "  MBR: " << sizeof(MBR) << " bytes\n";
    
    for (int i = 0; i < 4; i++) {
        const auto& p = mbr.mbr_partitions[i];
        if (p.part_s > 0) {
            double percent = (double)p.part_s / totalSize * 100;
            ss << "  Partición " << (i + 1) << " (" << p.part_name << "): " 
               << p.part_s << " bytes (" << std::fixed << std::setprecision(2) << percent << "%)\n";
        }
    }
    
    double freePercent = (double)freeSize / totalSize * 100;
    ss << "  Espacio libre: " << freeSize << " bytes (" << std::fixed << std::setprecision(2) << freePercent << "%)\n";
    
    return ss.str();
}

// Función para generar reporte SB (Superbloque)
static std::string generateSBReport(const Superblock& sb) {
    std::stringstream ss;
    
    ss << "========================================\n";
    ss << "       REPORTE SUPERBLOQUE\n";
    ss << "========================================\n\n";
    
    ss << "Información del Superbloque:\n";
    ss << "  Tipo de sistema: " << sb.s_filesystem_type << " (EXT2)\n";
    ss << "  Inodos totales: " << sb.s_inodes_count << "\n";
    ss << "  Bloques totales: " << sb.s_blocks_count << "\n";
    ss << "  Inodos libres: " << sb.s_free_inodes_count << "\n";
    ss << "  Bloques libres: " << sb.s_free_blocks_count << "\n";
    ss << "  Magic: 0x" << std::hex << sb.s_magic << std::dec << "\n";
    ss << "  Tamaño inodo: " << sb.s_inode_s << " bytes\n";
    ss << "  Tamaño bloque: " << sb.s_block_s << " bytes\n";
    ss << "  Primer inodo: " << sb.s_first_ino << "\n";
    ss << "  Primer bloque: " << sb.s_first_blo << "\n\n";
    
    ss << "Posiciones:\n";
    ss << "  Bitmap inodos: " << sb.s_bm_inode_start << "\n";
    ss << "  Bitmap bloques: " << sb.s_bm_block_start << "\n";
    ss << "  Tabla inodos: " << sb.s_inode_start << "\n";
    ss << "  Tabla bloques: " << sb.s_block_start << "\n";
    
    return ss.str();
}

CommandResult CommandHandler::processRep(const json& params) {
    CommandResult result;
    result.success = false;
    
    try {
        // 1. Obtener parámetros
        std::string name = params["name"];
        std::string path = params["path"];
        std::string id = params["id"];
        
        // 2. Validar que el ID exista
        if (mountedDisks.find(id) == mountedDisks.end()) {
            result.message = "Error: El ID de montaje no existe: " + id;
            return result;
        }
        
        // 3. Obtener la ruta del disco
        std::string diskPath = mountedDisks[id];
        
        // 4. Abrir el disco
        std::fstream disk(diskPath, std::ios::in | std::ios::binary);
        if (!disk.is_open()) {
            result.message = "Error: No se pudo abrir el disco: " + diskPath;
            return result;
        }
        
        // 5. Leer el MBR
        MBR mbr;
        disk.seekg(0, std::ios::beg);
        disk.read(reinterpret_cast<char*>(&mbr), sizeof(MBR));
        disk.close();
        
        // 6. Crear carpetas para el reporte
        size_t lastSlash = path.find_last_of('/');
        if (lastSlash != std::string::npos) {
            std::string dirPath = path.substr(0, lastSlash);
            if (!createReportDirectories(dirPath)) {
                result.message = "Error: No se pudo crear la carpeta para el reporte";
                return result;
            }
        }
        
        // 7. Generar el reporte según el tipo
        std::string reportContent;
        
        if (name == "mbr") {
            reportContent = generateMBRReport(mbr, diskPath);
        } else if (name == "disk") {
            reportContent = generateDiskReport(mbr, diskPath);
        } else if (name == "sb") {
            // Leer el Superbloque de la primera partición
            for (int i = 0; i < 4; i++) {
                if (mbr.mbr_partitions[i].part_s > 0) {
                    Superblock sb;
                    std::fstream disk2(diskPath, std::ios::in | std::ios::binary);
                    disk2.seekg(mbr.mbr_partitions[i].part_start, std::ios::beg);
                    disk2.read(reinterpret_cast<char*>(&sb), sizeof(Superblock));
                    disk2.close();
                    reportContent = generateSBReport(sb);
                    break;
                }
            }
        } else if (name == "inode") {
            reportContent = "Reporte INODE - Pendiente de implementación\n";
        } else if (name == "block") {
            reportContent = "Reporte BLOCK - Pendiente de implementación\n";
        } else if (name == "bm_inode") {
            reportContent = "Reporte BM_INODE - Pendiente de implementación\n";
        } else if (name == "bm_block") {
            reportContent = "Reporte BM_BLOCK - Pendiente de implementación\n";
        } else if (name == "tree") {
            reportContent = "Reporte TREE - Pendiente de implementación\n";
        } else if (name == "file") {
            reportContent = "Reporte FILE - Pendiente de implementación\n";
        } else if (name == "ls") {
            reportContent = "Reporte LS - Pendiente de implementación\n";
        } else {
            result.message = "Error: Tipo de reporte inválido: " + name;
            return result;
        }
        
        // 8. Escribir el reporte en el archivo
        std::ofstream reportFile(path);
        if (!reportFile.is_open()) {
            result.message = "Error: No se pudo crear el archivo de reporte: " + path;
            return result;
        }
        reportFile << reportContent;
        reportFile.close();
        
        // 9. Éxito
        result.success = true;
        result.message = "Reporte generado exitosamente: " + path;
        result.data["report"] = {
            {"name", name},
            {"path", path},
            {"id", id}
        };
        
    } catch (const std::exception& e) {
        result.message = "Error al procesar REP: " + std::string(e.what());
    }
    
    return result;
}