<template>
  <div class="file-explorer">
    <div class="explorer-header">
      <span class="explorer-title">Archivos</span>
      <span class="mount-info">{{ currentMount || 'sin montar' }}</span>
    </div>

    <div class="explorer-path">
      <button class="btn-up" @click="goUp" :disabled="currentPath === '/'">↑</button>
      <span class="path-text">{{ currentPath }}</span>
    </div>

    <div class="explorer-content">
      <div v-if="files.length === 0" class="empty-state">
        <p>Vacio</p>
      </div>
      <div v-for="file in files" :key="file.name" class="file-item" :class="{ 'is-folder': file.isFolder }" @dblclick="openFolder(file)">
        <span class="file-icon">{{ file.isFolder ? '📁' : '📄' }}</span>
        <span class="file-name">{{ file.name }}</span>
        <span class="file-size">{{ file.isFolder ? '-' : file.size }}</span>
      </div>
    </div>
  </div>
</template>

<script>
export default {
  name: 'FileExplorer',
  data() {
    return {
      currentPath: '/',
      currentMount: '',
      files: []
    }
  },
  methods: {
    refresh() {
      console.log('Actualizando explorador de archivos...')
    },
    goUp() {
      if (this.currentPath !== '/') {
        const parts = this.currentPath.split('/').filter(p => p)
        parts.pop()
        this.currentPath = '/' + parts.join('/') || '/'
      }
    },
    openFolder(file) {
      if (file.isFolder) {
        this.currentPath = this.currentPath === '/' ? '/' + file.name : this.currentPath + '/' + file.name
      }
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

.btn-up {
  background: #0d1117;
  border: 1px solid #30363d;
  color: #8b949e;
  border-radius: 4px;
  padding: 0 6px;
  font-size: 12px;
  cursor: pointer;
  transition: all 0.3s ease;
}

.btn-up:hover:not(:disabled) {
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

.empty-state {
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