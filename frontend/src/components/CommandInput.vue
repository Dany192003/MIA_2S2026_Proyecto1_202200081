<template>
  <div class="command-input">
    <div class="input-header">
      <div class="header-left">
        <span class="icon">⌨</span>
        <h3>Entrada de Comandos</h3>
      </div>
      <div class="connection-status">
        <span class="status-dot" :class="backendOnline ? 'online' : 'offline'"></span>
        <span class="status-text" :class="backendOnline ? 'online' : 'offline'">
          {{ backendOnline ? 'Conectado' : 'Desconectado' }}
        </span>
      </div>
    </div>
    
    <div class="input-group">
      <textarea
        v-model="command"
        placeholder="Ej: mkdisk -size=100 -path=/home/disk.dk"
        rows="4"
        class="command-textarea"
        @keydown.ctrl.enter="submitCommand"
        @input="autoResize"
        ref="textarea"
      ></textarea>
      <div class="char-count" v-if="command.length > 0">
        {{ command.length }} caracteres
      </div>
    </div>
    
    <div class="button-group">
      <button @click="submitCommand" class="btn-primary" :disabled="loading || !backendOnline">
        <span v-if="!loading">▶ Ejecutar</span>
        <span v-else class="loading-text">
          <span class="spinner-small"></span>
          Procesando...
        </span>
      </button>
      <button @click="clearCommand" class="btn-secondary">
        Limpiar
      </button>
    </div>
    
    <div class="status-bar" v-if="loading">
      <div class="status-indicator">
        <div class="spinner"></div>
        <span>Enviando comando al servidor...</span>
      </div>
    </div>
    
    <div class="shortcuts">
      <span><kbd>Ctrl</kbd> + <kbd>Enter</kbd> para ejecutar</span>
    </div>
    
    <!-- Carga masiva -->
    <FileUpload 
      @file-loaded="onFileLoaded"
      @execution-complete="onExecutionComplete"
      @file-cleared="onFileCleared"
    />
  </div>
</template>

<script>
import { analyzeCommand, checkBackendStatus } from '../services/api.js'
import FileUpload from './FileUpload.vue'

export default {
  name: 'CommandInput',
  components: {
    FileUpload
  },
  data() {
    return {
      command: '',
      loading: false,
      backendOnline: false
    }
  },
  mounted() {
    this.checkConnection()
    setInterval(this.checkConnection, 10000)
  },
  methods: {
    async checkConnection() {
      const status = await checkBackendStatus()
      this.backendOnline = status.online
    },
    
    async submitCommand() {
      const trimmedCommand = this.command.trim()
      
      if (!trimmedCommand) {
        this.$emit('command-submitted', {
          success: false,
          message: 'Por favor ingresa un comando',
          errors: [{ message: 'Comando vacío' }]
        })
        return
      }
      
      if (!this.backendOnline) {
        this.$emit('command-submitted', {
          success: false,
          message: 'Backend no disponible',
          errors: [{ message: 'El servidor no está corriendo en http://localhost:8080' }]
        })
        return
      }
      
      this.loading = true
      
      // Dividir por líneas y filtrar vacías
      const lines = trimmedCommand.split('\n')
        .map(line => line.trim())
        .filter(line => line.length > 0)
        .filter(line => !line.startsWith('#'))
      
      // Si es un solo comando, ejecutarlo directamente
      if (lines.length === 1) {
        const result = await analyzeCommand(lines[0])
        if (result.success) {
          this.$emit('command-submitted', result.data)
        } else {
          this.$emit('command-submitted', {
            success: false,
            message: 'Error de conexión',
            errors: [{ message: result.error }]
          })
        }
      } 
      // Si son múltiples comandos, ejecutar en lote
      else if (lines.length > 1) {
        for (let i = 0; i < lines.length; i++) {
          const result = await analyzeCommand(lines[i])
          this.$emit('command-submitted', {
            ...result.data,
            _batch: {
              index: i + 1,
              total: lines.length,
              command: lines[i]
            }
          })
        }
      }
      
      this.loading = false
    },
    
    clearCommand() {
      this.command = ''
      this.$emit('command-submitted', null)
      this.autoResize()
    },
    
    autoResize() {
      const textarea = this.$refs.textarea
      if (textarea) {
        textarea.style.height = 'auto'
        textarea.style.height = Math.min(textarea.scrollHeight, 200) + 'px'
      }
    },
    
    onFileLoaded(data) {
      console.log(`Archivo cargado: ${data.count} comandos`)
      if (data.commands && data.commands.length > 0) {
        this.command = data.commands.join('\n')
        this.autoResize()
      }
    },
    
    onExecutionComplete(data) {
      console.log(`Completado: ${data.success} éxitos, ${data.failed} fallos`)
    },
    
    onFileCleared() {
      this.command = ''
      this.$emit('command-submitted', null)
      this.autoResize()
    }
  },
  watch: {
    command() {
      this.autoResize()
    }
  }
}
</script>

<style scoped>
.command-input {
  display: flex;
  flex-direction: column;
  gap: 14px;
  height: 100%;
}

.input-header {
  display: flex;
  justify-content: space-between;
  align-items: center;
  padding-bottom: 10px;
  border-bottom: 1px solid #e8edf2;
  flex-wrap: wrap;
  gap: 8px;
}

.header-left {
  display: flex;
  align-items: center;
  gap: 8px;
}

.icon {
  font-size: 18px;
  color: #5b7a9a;
}

.input-header h3 {
  margin: 0;
  font-size: 15px;
  font-weight: 600;
  color: #2c3e50;
  letter-spacing: 0.3px;
}

.connection-status {
  display: flex;
  align-items: center;
  gap: 6px;
  background: #f5f7fa;
  padding: 3px 12px 3px 8px;
  border-radius: 20px;
  border: 1px solid #e8edf2;
  font-size: 12px;
}

.status-dot {
  width: 8px;
  height: 8px;
  border-radius: 50%;
  display: inline-block;
  transition: all 0.3s ease;
}

.status-dot.online {
  background: #4caf50;
}

.status-dot.offline {
  background: #f44336;
}

.status-text.online {
  color: #2c3e50;
}

.status-text.offline {
  color: #c62828;
}

.input-group {
  position: relative;
  flex: 1;
}

.command-textarea {
  width: 100%;
  padding: 12px 14px;
  background: #f7f9fc;
  color: #1a2a3a;
  border: 1px solid #dce4ec;
  border-radius: 12px;
  font-family: 'Courier New', monospace;
  font-size: 13px;
  resize: none;
  min-height: 70px;
  max-height: 200px;
  transition: all 0.3s ease;
  line-height: 1.6;
}

.command-textarea:focus {
  outline: none;
  border-color: #5b9bd5;
  box-shadow: 0 0 0 3px rgba(91, 155, 213, 0.12);
  background: #ffffff;
}

.command-textarea::placeholder {
  color: #a0b4c8;
}

.char-count {
  position: absolute;
  bottom: 8px;
  right: 10px;
  font-size: 10px;
  color: #a0b4c8;
  background: rgba(255, 255, 255, 0.9);
  padding: 1px 8px;
  border-radius: 10px;
}

.button-group {
  display: flex;
  gap: 10px;
}

.btn-primary {
  flex: 1;
  padding: 10px 20px;
  background: #2c3e50;
  color: white;
  border: none;
  border-radius: 12px;
  font-weight: 500;
  font-size: 13px;
  cursor: pointer;
  transition: all 0.3s ease;
  letter-spacing: 0.3px;
}

.btn-primary:hover:not(:disabled) {
  background: #1a2a3a;
  transform: translateY(-1px);
  box-shadow: 0 4px 12px rgba(44, 62, 80, 0.25);
}

.btn-primary:disabled {
  opacity: 0.5;
  cursor: not-allowed;
  transform: none;
}

.loading-text {
  display: flex;
  align-items: center;
  justify-content: center;
  gap: 8px;
}

.spinner-small {
  width: 14px;
  height: 14px;
  border: 2px solid rgba(255, 255, 255, 0.3);
  border-top-color: white;
  border-radius: 50%;
  animation: spin 0.8s linear infinite;
}

@keyframes spin {
  to { transform: rotate(360deg); }
}

.btn-secondary {
  padding: 10px 18px;
  background: transparent;
  color: #2c3e50;
  border: 1px solid #dce4ec;
  border-radius: 12px;
  font-weight: 500;
  font-size: 13px;
  cursor: pointer;
  transition: all 0.3s ease;
}

.btn-secondary:hover {
  background: #f0f4f8;
  border-color: #b0c4d8;
}

.status-bar {
  background: #eef3f8;
  border-radius: 12px;
  padding: 8px 14px;
  border-left: 3px solid #5b9bd5;
}

.status-indicator {
  display: flex;
  align-items: center;
  gap: 10px;
  color: #2c3e50;
  font-size: 12px;
  font-weight: 500;
}

.spinner {
  width: 16px;
  height: 16px;
  border: 2px solid #dce4ec;
  border-top-color: #5b9bd5;
  border-radius: 50%;
  animation: spin 0.8s linear infinite;
  flex-shrink: 0;
}

.shortcuts {
  margin-top: 2px;
  text-align: center;
  font-size: 11px;
  color: #a0b4c8;
}

.shortcuts kbd {
  display: inline-block;
  padding: 0 6px;
  background: #f0f4f8;
  border: 1px solid #dce4ec;
  border-radius: 6px;
  font-size: 10px;
  font-family: monospace;
  color: #2c3e50;
}

@media (max-width: 480px) {
  .input-header {
    flex-direction: column;
    align-items: stretch;
  }
  .button-group {
    flex-direction: column;
  }
  .btn-primary, .btn-secondary {
    padding: 9px 14px;
    font-size: 12px;
  }
  .command-textarea {
    font-size: 12px;
    padding: 10px;
    min-height: 55px;
  }
}
</style>