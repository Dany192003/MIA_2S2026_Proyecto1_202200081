<template>
  <div class="file-upload">
    <div class="upload-header">
      <span class="icon"> </span>
      <h4>Carga Masiva de Comandos</h4>
    </div>
    
    <div class="upload-area" 
         @dragover.prevent 
         @drop.prevent="handleDrop"
         @click="$refs.fileInput.click()"
         :class="{ 'dragover': isDragover }"
    >
      <input 
        type="file" 
        ref="fileInput" 
        @change="handleFile" 
        accept=".txt"
        style="display: none"
      />
      
      <div v-if="!fileLoaded" class="upload-placeholder">
        <span class="upload-icon">📤</span>
        <p>Arrastra o haz clic para subir un archivo</p>
        <span class="upload-hint"> </span>
      </div>
      
      <div v-else class="file-info">
        <span class="file-icon"></span>
        <div class="file-details">
          <span class="file-name">{{ fileName }}</span>
        </div>
        <button @click.stop="clearFile" class="btn-remove">✕</button>
      </div>
    </div>
  </div>
</template>

<script>
export default {
  name: 'FileUpload',
  data() {
    return {
      fileLoaded: false,
      fileName: '',
      fileSize: 0,
      totalLines: 0,
      commands: [],
      isDragover: false
    }
  },
  methods: {
    handleFile(event) {
      const file = event.target.files[0]
      if (file) {
        this.processFile(file)
      }
    },
    
    handleDrop(event) {
      this.isDragover = false
      const file = event.dataTransfer.files[0]
      if (file) {
        this.processFile(file)
      }
    },
    
    processFile(file) {
      const reader = new FileReader()
      reader.onload = (e) => {
        const content = e.target.result
        const lines = content.split('\n')
          .map(line => line.trim())
          .filter(line => line.length > 0)
          .filter(line => !line.startsWith('#'))
        
        this.commands = lines
        this.totalLines = lines.length
        this.fileName = file.name
        this.fileSize = file.size
        this.fileLoaded = true
        
        this.$emit('file-loaded', { 
          count: this.totalLines,
          commands: this.commands 
        })
      }
      reader.readAsText(file)
    },
    
    clearFile() {
      this.fileLoaded = false
      this.fileName = ''
      this.fileSize = 0
      this.totalLines = 0
      this.commands = []
      this.$refs.fileInput.value = ''
      this.$emit('file-cleared')
    }
  }
}
</script>

<style scoped>
.file-upload {
  background: #f7f9fc;
  border-radius: 12px;
  padding: 12px 14px;
  border: 1px solid #e8edf2;
  margin-top: 10px;
}

.upload-header {
  display: flex;
  align-items: center;
  gap: 6px;
  margin-bottom: 8px;
}

.upload-header .icon {
  font-size: 14px;
  color: #5b7a9a;
}

.upload-header h4 {
  margin: 0;
  font-size: 11px;
  font-weight: 600;
  color: #5b7a9a;
  text-transform: uppercase;
  letter-spacing: 0.3px;
}

.upload-area {
  border: 2px dashed #dce4ec;
  border-radius: 10px;
  padding: 16px;
  text-align: center;
  cursor: pointer;
  transition: all 0.3s ease;
  min-height: 60px;
  display: flex;
  align-items: center;
  justify-content: center;
}

.upload-area:hover {
  border-color: #5b9bd5;
  background: #f0f4f8;
}

.upload-area.dragover {
  border-color: #2e7d32;
  background: #e8f5e9;
}

.upload-placeholder {
  display: flex;
  flex-direction: column;
  align-items: center;
  gap: 4px;
}

.upload-icon {
  font-size: 28px;
  opacity: 0.5;
}

.upload-placeholder p {
  margin: 0;
  font-size: 13px;
  color: #5b7a9a;
}

.upload-hint {
  font-size: 11px;
  color: #a0b4c8;
}

.file-info {
  display: flex;
  align-items: center;
  gap: 12px;
  width: 100%;
}

.file-icon {
  font-size: 24px;
}

.file-details {
  display: flex;
  flex-direction: column;
  align-items: flex-start;
  flex: 1;
}

.file-name {
  font-weight: 600;
  color: #2c3e50;
  font-size: 13px;
}

.file-size, .file-lines {
  font-size: 11px;
  color: #a0b4c8;
}

.btn-remove {
  background: none;
  border: none;
  color: #c62828;
  font-size: 18px;
  cursor: pointer;
  padding: 0 4px;
}

.btn-remove:hover {
  color: #b71c1c;
}

@media (max-width: 480px) {
  .upload-area {
    padding: 12px;
    min-height: 50px;
  }
  .upload-icon {
    font-size: 22px;
  }
  .upload-placeholder p {
    font-size: 12px;
  }
  .file-info {
    gap: 8px;
  }
  .file-name {
    font-size: 12px;
  }
}
</style>