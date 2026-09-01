<template>
  <div class="file-explorer">
    <div class="explorer-header">
      <span class="explorer-title">Archivos</span>
      <span class="mount-info" v-if="mountId">ID: {{ mountId }}</span>
      <span class="mount-info" v-else>sin montar</span>
    </div>

    <div class="explorer-path">
      <button class="btn-up" @click="goUp" :disabled="currentPath === '/'">↑</button>
      <span class="path-text">{{ currentPath }}</span>
      <button class="btn-refresh" @click="refresh" title="Actualizar">↻</button>
    </div>

    <div class="explorer-content">
      <div v-if="loading" class="loading-state">
        <span>Cargando...</span>
      </div>
      <div v-else-if="files.length === 0" class="empty-state">
        <p>Vacio</p>
      </div>
      <div v-for="file in files" :key="file.name" class="file-item" 
           :class="{ 'is-folder': file.isFolder }" 
           @dblclick="openFolder(file)">
        <span class="file-icon">{{ file.isFolder ? '📁' : '📄' }}</span>
        <span class="file-name">{{ file.name }}</span>
        <span class="file-size">{{ file.isFolder ? '-' : formatSize(file.size) }}</span>
        <span class="file-perms">{{ file.perms }}</span>
      </div>
    </div>
  </div>
</template>

<script>
import { analyzeCommand } from '../../services/api.js'

export default {
  name: 'FileExplorer',
  data() {
    return {
      currentPath: '/',
      mountId: '',
      files: [],
      loading: false
    }
  },
  mounted() {
    // Intentar obtener el ID de montaje
    this.getMountId()
  },
  methods: {
    async getMountId() {
      try {
        const result = await analyzeCommand('mounted')
        if (result.success && result.data?.data?.mounted) {
          const mounted = result.data.data.mounted
          if (mounted.length > 0) {
            this.mountId = mounted[0].id || ''
            this.refresh()
          }
        }
      } catch (error) {
        console.error('Error obteniendo ID de montaje:', error)
      }
    },
    async refresh() {
      if (!this.mountId) {
        await this.getMountId()
        if (!this.mountId) return
      }

      this.loading = true
      try {
        const result = await analyzeCommand(`lsjson -path=${this.currentPath} -id=${this.mountId}`)
        console.log('📂 lsjson result:', result)
        
        if (result.success && result.data?.data?.files) {
          this.files = result.data.data.files
        } else {
          this.files = []
        }
      } catch (error) {
        console.error('Error listando archivos:', error)
        this.files = []
      }
      this.loading = false
    },
    goUp() {
      if (this.currentPath !== '/') {
        const parts = this.currentPath.split('/').filter(p => p)
        parts.pop()
        this.currentPath = '/' + parts.join('/') || '/'
        this.refresh()
      }
    },
    openFolder(file) {
      if (file.isFolder) {
        this.currentPath = this.currentPath === '/' ? '/' + file.name : this.currentPath + '/' + file.name
        this.refresh()
      }
    },
    formatSize(bytes) {
      if (!bytes || bytes === 0) return '0 B'
      const k = 1024
      const sizes = ['B', 'KB', 'MB', 'GB']
      const i = Math.floor(Math.log(bytes) / Math.log(k))
      return parseFloat((bytes / Math.pow(k, i)).toFixed(2)) + ' ' + sizes[i]
    }
  }
}
</script>

<style scoped>
.file-explorer {
  background: #161b22;
  border-radius: 8px;
  border: 1px solid #30363d;
  padding: 8px 12px;
  height: 100%;
  display: flex;
  flex-direction: column;
  overflow: hidden;
}

.explorer-header {
  display: flex;
  justify-content: space-between;
  align-items: center;
  margin-bottom: 4px;
  flex-shrink: 0;
}

.explorer-title {
  font-size: 10px;
  font-weight: 600;
  color: #8b949e;
  text-transform: uppercase;
  letter-spacing: 0.5px;
}

.mount-info {
  font-size: 9px;
  color: #3fb950;
  background: rgba(63, 185, 80, 0.1);
  padding: 0 8px;
  border-radius: 8px;
  border: 1px solid rgba(63, 185, 80, 0.2);
}

.explorer-path {
  display: flex;
  align-items: center;
  gap: 4px;
  margin-bottom: 4px;
  flex-shrink: 0;
}

.btn-up, .btn-refresh {
  background: #0d1117;
  border: 1px solid #30363d;
  color: #8b949e;
  border-radius: 4px;
  padding: 0 6px;
  font-size: 12px;
  cursor: pointer;
  transition: all 0.3s ease;
}

.btn-up:hover:not(:disabled), .btn-refresh:hover {
  border-color: #58a6ff;
  color: #e6edf3;
}

.btn-up:disabled {
  opacity: 0.3;
  cursor: not-allowed;
}

.path-text {
  font-size: 10px;
  color: #8b949e;
  font-family: 'Courier New', monospace;
  background: #0d1117;
  padding: 0 6px;
  border-radius: 3px;
  border: 1px solid #30363d;
  flex: 1;
  overflow: hidden;
  text-overflow: ellipsis;
  white-space: nowrap;
}

.explorer-content {
  flex: 1;
  overflow-y: auto;
  display: flex;
  flex-direction: column;
  gap: 2px;
}

.loading-state, .empty-state {
  flex: 1;
  display: flex;
  align-items: center;
  justify-content: center;
  color: #8b949e;
  font-size: 12px;
}

.file-item {
  display: flex;
  align-items: center;
  gap: 6px;
  padding: 2px 6px;
  border-radius: 4px;
  font-size: 11px;
  color: #8b949e;
  transition: background 0.2s ease;
}

.file-item:hover {
  background: #21262d;
}

.file-item.is-folder {
  cursor: pointer;
  color: #e6edf3;
}

.file-item.is-folder:hover {
  background: #1c2333;
}

.file-icon {
  font-size: 12px;
}

.file-name {
  flex: 1;
  color: #e6edf3;
}

.file-size {
  font-size: 9px;
  color: #8b949e;
  min-width: 50px;
  text-align: right;
}

.file-perms {
  font-size: 8px;
  color: #30363d;
  font-family: monospace;
  min-width: 30px;
  text-align: center;
}

::-webkit-scrollbar {
  width: 3px;
}

::-webkit-scrollbar-track {
  background: #0d1117;
}

::-webkit-scrollbar-thumb {
  background: #30363d;
  border-radius: 2px;
}
</style>