#include "lexer.h"
#include <cctype>
#include <algorithm>

Lexer::Lexer() : currentLine(1), currentColumn(1), position(0) {}

std::vector<Token> Lexer::tokenize(const std::string& input) {
    tokens.clear();
    errors.clear();
    source = input;
    position = 0;
    currentLine = 1;
    currentColumn = 1;
    
    while (!isAtEnd()) {
        char c = peek();
        
        if (std::isspace(c)) {
            if (c == '\n') {
                currentLine++;
                currentColumn = 1;
            } else {
                currentColumn++;
            }
            position++;
            continue;
        }
        
        if (c == '#') {
            while (!isAtEnd() && peek() != '\n') {
                position++;
                currentColumn++;
            }
            continue;
        }
        
        if (c == '-') {
            if (position + 1 < (int)source.length() && std::isdigit(source[position + 1])) {
                identifyNumber();
            } else {
                identifyParameter();
            }
            continue;
        }
        
        if (c == '"') {
            identifyString();
            continue;
        }
        
        if (c == '=') {
            advance();
            tokens.push_back(makeToken("EQUALS", "="));
            
            while (!isAtEnd() && std::isspace(peek())) {
                advance();
            }
            
            if (!isAtEnd() && peek() == '"') {
                identifyString();
            } else {
                identifyValue();
            }
            continue;
        }
        
        if (std::isdigit(c)) {
            identifyNumber();
            continue;
        }
        
        if (std::isalpha(c)) {
            identifyCommand();
            continue;
        }
        
        addError("Caracter no reconocido: '" + std::string(1, c) + "'");
        position++;
        currentColumn++;
    }
    
    return tokens;
}

char Lexer::peek() {
    if (isAtEnd()) return '\0';
    return source[position];
}

bool Lexer::isAtEnd() {
    return position >= (int)source.length();
}

void Lexer::addError(const std::string& message) {
    errors.push_back({"lexical", message, currentLine, currentColumn});
}

bool Lexer::hasErrors() const {
    return !errors.empty();
}

std::vector<ErrorInfo> Lexer::getErrors() const {
    return errors;
}

void Lexer::advance() {
    if (!isAtEnd()) {
        if (source[position] == '\n') {
            currentLine++;
            currentColumn = 1;
        } else {
            currentColumn++;
        }
        position++;
    }
}

Token Lexer::makeToken(const std::string& type, const std::string& value) {
    return {type, value, currentLine, currentColumn};
}

void Lexer::identifyCommand() {
    std::string command;
    while (!isAtEnd() && std::isalnum(peek())) {
        command += peek();
        advance();
    }
    
    std::string lowerCommand = command;
    std::transform(lowerCommand.begin(), lowerCommand.end(), lowerCommand.begin(), ::tolower);
    
    // ============================================================
    // TODOS LOS COMANDOS DEL PROYECTO 1
    // ============================================================
    std::vector<std::string> validCommands = {
        // Administración de discos
        "mkdisk", "rmdisk", "fdisk", "mount", "mounted",
        // Administración del sistema de archivos
        "mkfs", "login", "logout",
        // Administración de usuarios y grupos
        "mkgrp", "rmgrp", "mkusr", "rmusr", "chgrp",
        // Administración de carpetas y archivos
        "mkfile", "mkdir", "cat",
        // Reportes
        "rep", "lsdisk"

    };
    
    if (std::find(validCommands.begin(), validCommands.end(), lowerCommand) != validCommands.end()) {
        tokens.push_back(makeToken("COMMAND", lowerCommand));
    } else {
        tokens.push_back(makeToken("IDENTIFIER", command));
    }
}

void Lexer::identifyParameter() {
    if (peek() != '-') {
        addError("Se esperaba '-' para un parametro");
        return;
    }
    
    advance(); 
    std::string paramName;
    // Permitir letras, números y guiones bajos en nombres de parámetros
    while (!isAtEnd() && (std::isalnum(peek()) || peek() == '_')) {
        paramName += peek();
        advance();
    }
    
    // Convertir parametro a minusculas
    std::transform(paramName.begin(), paramName.end(), paramName.begin(), ::tolower);
    
    if (!isAtEnd() && peek() == '=') {
        advance(); 
        
        while (!isAtEnd() && std::isspace(peek())) {
            advance();
        }
        
        tokens.push_back(makeToken("PARAMETER", paramName));
        tokens.push_back(makeToken("EQUALS", "="));
        
        if (!isAtEnd() && peek() == '"') {
            identifyString(); 
        } else {
            identifyValue(); 
        }
        return;
    }
    
    tokens.push_back(makeToken("FLAG", paramName));
}

void Lexer::identifyValue() {
    std::string value;
    int startLine = currentLine;
    int startColumn = currentColumn;
    
    if (peek() == '-') {
        value += peek();
        advance();
    }
    
    while (!isAtEnd() && !std::isspace(peek()) && peek() != '=' && peek() != '-') {
        value += peek();
        advance();
    }
    
    if (value.empty()) {
        addError("Valor vacio");
        return;
    }
    
    bool isNumber = true;
    int startIdx = 0;
    if (value[0] == '-') {
        startIdx = 1;
    }
    for (int i = startIdx; i < (int)value.length(); i++) {
        if (!std::isdigit(value[i])) {
            isNumber = false;
            break;
        }
    }
    
    if (isNumber) {
        tokens.push_back({"NUMBER", value, startLine, startColumn});
    } else {
        tokens.push_back({"VALUE", value, startLine, startColumn});
    }
}

void Lexer::identifyString() {
    if (peek() != '"') {
        addError("Se esperaba '\"' para iniciar una cadena");
        return;
    }
    
    advance(); 
    std::string value;
    int startLine = currentLine;
    int startColumn = currentColumn;
    
    while (!isAtEnd() && peek() != '"') {
        if (peek() == '\n') {
            addError("Cadena no cerrada");
            return;
        }
        value += peek();
        advance();
    }
    
    if (isAtEnd()) {
        addError("Cadena no cerrada");
        return;
    }
    
    advance();
    tokens.push_back({"STRING", value, startLine, startColumn});
}

void Lexer::identifyNumber() {
    std::string number;
    int startLine = currentLine;
    int startColumn = currentColumn;

    if (peek() == '-') {
        number += peek();
        advance();
    }
    
    while (!isAtEnd() && std::isdigit(peek())) {
        number += peek();
        advance();
    }
    
    tokens.push_back({"NUMBER", number, startLine, startColumn});
}