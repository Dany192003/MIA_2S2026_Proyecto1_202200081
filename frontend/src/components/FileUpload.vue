<template>
  <div class="file-upload">
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
        accept=".txt,.smia"
        style="display: none"
      />
      
      <div v-if="!fileLoaded" class="upload-placeholder">
        <span class="upload-icon">📤</span>
        <p>Arrastra o haz clic para subir un script</p>
        <span class="upload-hint">Archivos .txt o .smia con comandos EXT2</span>
      </div>
      
      <div v-else class="file-info">
        <span class="file-icon">📄</span>
        <div class="file-details">
          <span class="file-name">{{ fileName }}</span>
          <span class="file-size">{{ fileSize }} bytes</span>
          <span class="file-lines">{{ totalLines }} comandos</span>
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
  margin-top: 4px;
}

.upload-area {
  border: 2px dashed #30363d;
  border-radius: 8px;
  padding: 12px;
  text-align: center;
  cursor: pointer;
  transition: all 0.3s ease;
  min-height: 50px;
  display: flex;
  align-items: center;
  justify-content: center;
  background: #0d1117;
}

.upload-area:hover {
  border-color: #58a6ff;
  background: #161b22;
}

.upload-area.dragover {
  border-color: #3fb950;
  background: rgba(63, 185, 80, 0.05);
}

.upload-placeholder {
  display: flex;
  flex-direction: column;
  align-items: center;
  gap: 2px;
}

.upload-icon {
  font-size: 24px;
  opacity: 0.5;
}

.upload-placeholder p {
  margin: 0;
  font-size: 12px;
  color: #8b949e;
}

.upload-hint {
  font-size: 10px;
  color: #30363d;
}

.file-info {
  display: flex;
  align-items: center;
  gap: 10px;
  width: 100%;
}

.file-icon {
  font-size: 20px;
}

.file-details {
  display: flex;
  flex-direction: column;
  align-items: flex-start;
  flex: 1;
}

.file-name {
  font-weight: 600;
  color: #e6edf3;
  font-size: 13px;
}

.file-size, .file-lines {
  font-size: 10px;
  color: #8b949e;
}

.btn-remove {
  background: transparent;
  border: none;
  color: #8b949e;
  font-size: 16px;
  cursor: pointer;
  padding: 0 4px;
  transition: color 0.3s ease;
}

.btn-remove:hover {
  color: #f85149;
}

@media (max-width: 600px) {
  .upload-area {
    padding: 8px;
    min-height: 40px;
  }
  .upload-placeholder p {
    font-size: 11px;
  }
  .file-name {
    font-size: 11px;
  }
}
</style>