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
        rows="6"
        class="command-textarea"
        @keydown.ctrl.enter="submitCommand"
        @input="autoResize"
        ref="textarea"
      ></textarea>
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
        🗑 Limpiar
      </button>
    </div>
    
    <FileUpload @file-loaded="onFileLoaded" />
    
    <div class="status-bar" v-if="loading">
      <div class="status-indicator">
        <div class="spinner"></div>
        <span>Enviando comando al servidor...</span>
      </div>
    </div>
    
    <div class="shortcuts">
      <span><kbd>Ctrl</kbd> + <kbd>Enter</kbd> para ejecutar</span>
    </div>
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
      if (!this.command.trim()) {
        this.$emit('command-submitted', {
          success: false,
          message: '⚠️ Por favor ingresa un comando',
          errors: [{ message: 'Comando vacío' }]
        })
        return
      }
      
      if (!this.backendOnline) {
        this.$emit('command-submitted', {
          success: false,
          message: '❌ Backend no disponible',
          errors: [{ message: 'Servidor no está corriendo en http://localhost:8080' }]
        })
        return
      }
      
      this.loading = true
      
      const result = await analyzeCommand(this.command)
      
      if (result.success) {
        this.$emit('command-submitted', result.data)
      } else {
        this.$emit('command-submitted', {
          success: false,
          message: '❌ Error',
          errors: [{ message: result.error || 'Error desconocido' }]
        })
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
      if (data.commands && data.commands.length > 0) {
        this.command = data.commands.join('\n')
        this.autoResize()
      }
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
  gap: 10px;
  height: 100%;
}

.input-header {
  display: flex;
  justify-content: space-between;
  align-items: center;
  padding-bottom: 8px;
  border-bottom: 1px solid #30363d;
  flex-wrap: wrap;
  gap: 6px;
}

.header-left {
  display: flex;
  align-items: center;
  gap: 6px;
}

.icon {
  font-size: 16px;
  color: #8b949e;
}

.input-header h3 {
  margin: 0;
  font-size: 13px;
  font-weight: 600;
  color: #e6edf3;
}

.connection-status {
  display: flex;
  align-items: center;
  gap: 4px;
  background: #0d1117;
  padding: 2px 10px 2px 6px;
  border-radius: 12px;
  border: 1px solid #30363d;
  font-size: 11px;
}

.status-dot {
  width: 6px;
  height: 6px;
  border-radius: 50%;
  display: inline-block;
}

.status-dot.online {
  background: #3fb950;
}

.status-dot.offline {
  background: #f85149;
}

.status-text.online {
  color: #3fb950;
}

.status-text.offline {
  color: #f85149;
}

.input-group {
  flex: 1;
  min-height: 120px;
}

.command-textarea {
  width: 100%;
  height: 100%;
  min-height: 120px;
  padding: 10px 14px;
  background: #0d1117;
  color: #e6edf3;
  border: 1px solid #30363d;
  border-radius: 8px;
  font-family: 'Courier New', monospace;
  font-size: 14px;
  resize: vertical;
  transition: border-color 0.3s ease;
  line-height: 1.6;
}

.command-textarea:focus {
  outline: none;
  border-color: #58a6ff;
}

.command-textarea::placeholder {
  color: #30363d;
}

.button-group {
  display: flex;
  gap: 8px;
}

.btn-primary {
  flex: 1;
  padding: 8px 20px;
  background: #58a6ff;
  color: #0d1117;
  border: none;
  border-radius: 6px;
  font-weight: 600;
  font-size: 13px;
  cursor: pointer;
  transition: all 0.3s ease;
}

.btn-primary:hover:not(:disabled) {
  background: #79c0ff;
}

.btn-primary:disabled {
  opacity: 0.5;
  cursor: not-allowed;
}

.btn-secondary {
  padding: 8px 16px;
  background: transparent;
  color: #8b949e;
  border: 1px solid #30363d;
  border-radius: 6px;
  font-weight: 500;
  font-size: 13px;
  cursor: pointer;
  transition: all 0.3s ease;
}

.btn-secondary:hover {
  border-color: #58a6ff;
  color: #e6edf3;
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
  border: 2px solid rgba(13, 17, 23, 0.3);
  border-top-color: #0d1117;
  border-radius: 50%;
  animation: spin 0.8s linear infinite;
}

@keyframes spin {
  to { transform: rotate(360deg); }
}

.status-bar {
  background: #161b22;
  border-radius: 6px;
  padding: 6px 12px;
  border-left: 3px solid #58a6ff;
}

.status-indicator {
  display: flex;
  align-items: center;
  gap: 8px;
  color: #8b949e;
  font-size: 12px;
}

.spinner {
  width: 14px;
  height: 14px;
  border: 2px solid #30363d;
  border-top-color: #58a6ff;
  border-radius: 50%;
  animation: spin 0.8s linear infinite;
  flex-shrink: 0;
}

.shortcuts {
  text-align: center;
  font-size: 10px;
  color: #30363d;
}

.shortcuts kbd {
  display: inline-block;
  padding: 0 6px;
  background: #161b22;
  border: 1px solid #30363d;
  border-radius: 4px;
  font-size: 9px;
  font-family: monospace;
  color: #8b949e;
}

@media (max-width: 600px) {
  .input-header {
    flex-direction: column;
    align-items: stretch;
  }
  .button-group {
    flex-direction: column;
  }
  .btn-primary, .btn-secondary {
    padding: 6px 12px;
    font-size: 12px;
  }
  .command-textarea {
    font-size: 13px;
    padding: 8px 12px;
    min-height: 100px;
  }
}
</style>