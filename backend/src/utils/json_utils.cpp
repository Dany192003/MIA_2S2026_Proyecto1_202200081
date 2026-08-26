#include "json_utils.h"

json tokenToJson(const Token& token) {
    json j;
    j["type"] = token.type;
    j["value"] = token.value;
    j["line"] = token.line;
    j["column"] = token.column;
    return j;
}

json errorToJson(const ErrorInfo& error) {
    json j;
    j["type"] = error.type;
    j["message"] = error.message;
    j["line"] = error.line;
    j["column"] = error.column;
    return j;
}

std::string resultToJson(const CommandResult& result) {
    json j;
    j["success"] = result.success;
    j["command"] = result.command;
    j["message"] = result.message;
    
    // Convertir tokens a JSON
    json tokensArray = json::array();
    for (const auto& token : result.tokens) {
        tokensArray.push_back(tokenToJson(token));
    }
    j["tokens"] = tokensArray;
    
    // Convertir errores a JSON
    json errorsArray = json::array();
    for (const auto& error : result.errors) {
        errorsArray.push_back(errorToJson(error));
    }
    j["errors"] = errorsArray;
    
    // Datos adicionales
    if (!result.data.empty()) {
        j["data"] = result.data;
    }
    
    return j.dump(4); 
}