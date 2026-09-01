#include "command_handler.h"
#include <iostream>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <cerrno>
#include <cstring>

CommandHandler::CommandHandler() {
    // Inicializar sesión
    currentSession.active = false;
    currentSession.user = "";
    currentSession.mountId = "";
    currentSession.diskPath = "";
    currentSession.uid = -1;
    currentSession.gid = -1;
    currentSession.group = "";
}

CommandResult CommandHandler::processCommand(const std::string& command) {
    // Primero, parsear el comando
    CommandResult result = parser.parse(command);
    
    // Si hay errores de sintaxis o léxicos, retornar
    if (!result.success) {
        return result;
    }
    
    // Validar estructura del comando
    if (!validateCommandStructure(result)) {
        result.success = false;
        result.message = "Estructura del comando inválida";
        return result;
    }
    
    // Obtener el comando y parámetros
    std::string cmd = result.data["command"];
    json params = result.data["parameters"];
    
    // Procesar según el comando
    CommandResult processedResult;
    
    if (cmd == "mkdisk") {
        processedResult = processMkdisk(params);
    } else if (cmd == "rmdisk") {
        processedResult = processRmdisk(params);
    } else if (cmd == "fdisk") {
        processedResult = processFdisk(params);
    } else if (cmd == "mount") {
        processedResult = processMount(params);
    } else if (cmd == "mounted") {
        processedResult = processMounted(params);
    } else if (cmd == "mkfs") {
        processedResult = processMkfs(params);
    } else if (cmd == "login") {
        processedResult = processLogin(params);
    } else if (cmd == "logout") {
        processedResult = processLogout(params);
    } else if (cmd == "mkgrp") {
        processedResult = processMkgrp(params);
    } else if (cmd == "rmgrp") {
        processedResult = processRmgrp(params);
    } else if (cmd == "mkusr") {
        processedResult = processMkusr(params);
    } else if (cmd == "rmusr") {
        processedResult = processRmusr(params);
    } else if (cmd == "chgrp") {
        processedResult = processChgrp(params);
    } else if (cmd == "mkfile") {
        processedResult = processMkfile(params);
    } else if (cmd == "mkdir") {
        processedResult = processMkdir(params);
    } else if (cmd == "cat") {
        processedResult = processCat(params);
    } else if (cmd == "rep") {
        processedResult = processRep(params);
    } else {
        processedResult.success = false;
        processedResult.message = "Comando no implementado: " + cmd;
    }
    
    // 🔥 CORREGIDO: Fusionar datos en lugar de sobrescribir
    // Combinar resultados
    processedResult.command = command;
    processedResult.tokens = result.tokens;
    
    // 1. Si processedResult ya tiene data (ej: mounted, cat), conservarla
    // 2. Agregar command y parameters del parser para depuración
    json mergedData = processedResult.data;
    
    // Si no hay data del handler, inicializar vacío
    if (mergedData.empty()) {
        mergedData = json::object();
    }
    
    // Agregar command y parameters del parser (NO sobrescribir)
    mergedData["_command"] = cmd;
    mergedData["_parameters"] = params;
    
    // Asignar el data fusionado
    processedResult.data = mergedData;
    
    return processedResult;
}

bool CommandHandler::validateCommandStructure(const CommandResult& result) {
    if (!result.data.contains("parameters")) {
        return false;
    }
    if (!result.data["parameters"].is_object()) {
        return false;
    }
    return true;
}

bool CommandHandler::validateDiskExists(const std::string& path) {
    struct stat buffer;
    return (stat(path.c_str(), &buffer) == 0);
}

bool CommandHandler::validatePartitionName(const std::string& path, const std::string& name) {
    // TODO: Implementar validación de nombre de partición
    return true;
}

bool CommandHandler::validateMountId(const std::string& id) {
    return mountedDisks.find(id) != mountedDisks.end();
}

bool CommandHandler::isLoggedIn() {
    return currentSession.active;
}

bool CommandHandler::isRoot() {
    return currentSession.active && currentSession.user == "root";
}