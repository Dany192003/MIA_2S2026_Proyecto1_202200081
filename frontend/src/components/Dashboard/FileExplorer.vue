<template>
  <div class="file-explorer">
    <div class="explorer-header">
      <div class="header-left">
        <h3>📁 Sistema de Archivos</h3>
        <span class="mount-info">{{ currentMount || 'Ninguna partición montada' }}</span>
      </div>
      <div class="header-right">
        <button class="btn-path" @click="goUp" :disabled="currentPath === '/'">
          📤 Subir
        </button>
        <span class="current-path">{{ currentPath }}</span>
      </div>
    </div>

    <div class="explorer-content">
      <table class="file-table">
        <thead>
          <tr>
            <th>Nombre</th>
            <th>Tamaño</th>
            <th>Permisos</th>
            <th>Propietario</th>
            <th>Grupo</th>
            <th>Fecha</th>
          </tr>
        </thead>
        <tbody>
          <tr v-if="files.length === 0" class="empty-row">
            <td colspan="6">
              <span class="empty-folder">📂</span>
              <p>No hay archivos o carpetas</p>
            </td>
          </tr>
          <tr 
            v-for="file in files" 
            :key="file.name" 
            class="file-row"
            :class="{ 'is-folder': file.isFolder }"
            @dblclick="openFolder(file)"
          >
            <td class="file-name">
              <span class="file-icon">{{ file.isFolder ? '📁' : '📄' }}</span>
              <span>{{ file.name }}</span>
            </td>
            <td class="file-size">{{ file.isFolder ? '-' : file.size }}</td>
            <td class="file-perms">{{ file.perms }}</td>
            <td class="file-owner">{{ file.owner }}</td>
            <td class="file-group">{{ file.group }}</td>
            <td class="file-date">{{ file.date }}</td>
          </tr>
        </tbody>
      </table>
    </div>
  </div>
</template>

<script>
export default {
  name: 'FileExplorer',
  data() {
    return {
      currentPath: '/',
      currentMount: '811A',
      files: [
        { name: 'users.txt', isFolder: false, size: '1.2 KB', perms: '664', owner: 'root', group: 'root', date: '21/08/2026' },
        { name: 'home', isFolder: true, size: '-', perms: '755', owner: 'root', group: 'root', date: '27/08/2026' },
        { name: 'archivos', isFolder: true, size: '-', perms: '755', owner: 'root', group: 'root', date: '27/08/2026' },
        { name: 'test.txt', isFolder: false, size: '15 B', perms: '664', owner: 'user1', group: 'usuarios', date: '27/08/2026' },
        { name: 'mi_carpeta', isFolder: true, size: '-', perms: '664', owner: 'user1', group: 'usuarios', date: '27/08/2026' }
      ]
    }
  },
  methods: {
    refresh() {
      console.log('🔄 Actualizando explorador de archivos...')
    },
    goUp() {
      if (this.currentPath !== '/') {
        const parts = this.currentPath.split('/').filter(p => p)
        parts.pop()
        this.currentPath = '/' + parts.join('/')
        if (this.currentPath === '') this.currentPath = '/'
        console.log('📂 Subiendo a:', this.currentPath)
      }
    },
    openFolder(file) {
      if (file.isFolder) {
        this.currentPath = this.currentPath === '/' ? '/' + file.name : this.currentPath + '/' + file.name
        console.log('📂 Abriendo:', this.currentPath)
      }
    }
  }
}
</script>

<style scoped>
.file-explorer {
  background: #161b22;
  border-radius: 12px;
  border: 1px solid #30363d;
  overflow: hidden;
  display: flex;
  flex-direction: column;
}

.explorer-header {
  display: flex;
  justify-content: space-between;
  align-items: center;
  padding: 12px 16px;
  border-bottom: 1px solid #30363d;
  flex-wrap: wrap;
  gap: 8px;
}

.header-left {
  display: flex;
  align-items: center;
  gap: 12px;
}

.header-left h3 {
  margin: 0;
  font-size: 14px;
  font-weight: 600;
  color: #e6edf3;
}

.mount-info {
  font-size: 11px;
  color: #3fb950;
  background: rgba(63, 185, 80, 0.12);
  padding: 2px 10px;
  border-radius: 12px;
  border: 1px solid rgba(63, 185, 80, 0.2);
}

.header-right {
  display: flex;
  align-items: center;
  gap: 10px;
}

.btn-path {
  background: transparent;
  border: 1px solid #30363d;
  border-radius: 6px;
  color: #8b949e;
  padding: 4px 12px;
  font-size: 12px;
  cursor: pointer;
  transition: all 0.3s ease;
}

.btn-path:hover:not(:disabled) {
  border-color: #58a6ff;
  color: #e6edf3;
}

.btn-path:disabled {
  opacity: 0.3;
  cursor: not-allowed;
}

.current-path {
  font-size: 12px;
  color: #8b949e;
  font-family: 'Courier New', monospace;
  background: #0d1117;
  padding: 2px 10px;
  border-radius: 4px;
  border: 1px solid #30363d;
}

.explorer-content {
  flex: 1;
  overflow-x: auto;
  padding: 0 4px 4px 4px;
}

.file-table {
  width: 100%;
  border-collapse: collapse;
  font-size: 13px;
}

.file-table thead {
  background: #0d1117;
}

.file-table th {
  text-align: left;
  padding: 8px 12px;
  font-size: 11px;
  font-weight: 600;
  color: #8b949e;
  text-transform: uppercase;
  letter-spacing: 0.5px;
  border-bottom: 1px solid #30363d;
}

.file-table td {
  padding: 6px 12px;
  border-bottom: 1px solid #21262d;
  color: #e6edf3;
}

.file-row {
  cursor: default;
  transition: background 0.15s ease;
}

.file-row:hover {
  background: #21262d;
}

.file-row.is-folder {
  cursor: pointer;
}

.file-row.is-folder:hover {
  background: #1c2333;
}

.file-name {
  display: flex;
  align-items: center;
  gap: 8px;
}

.file-icon {
  font-size: 16px;
}

.file-size {
  color: #8b949e;
  font-size: 12px;
}

.file-perms {
  font-family: 'Courier New', monospace;
  font-size: 12px;
  color: #8b949e;
}

.file-owner, .file-group {
  font-size: 12px;
  color: #8b949e;
}

.file-date {
  font-size: 12px;
  color: #8b949e;
}

.empty-row td {
  text-align: center;
  padding: 30px 20px;
}

.empty-folder {
  font-size: 32px;
  display: block;
  opacity: 0.4;
}

.empty-row p {
  margin: 8px 0 0 0;
  color: #8b949e;
  font-size: 14px;
}

@media (max-width: 600px) {
  .explorer-header {
    flex-direction: column;
    align-items: stretch;
  }
  .header-right {
    flex-wrap: wrap;
  }
  .file-table {
    font-size: 11px;
  }
  .file-table th, .file-table td {
    padding: 4px 6px;
  }
  .file-owner, .file-group, .file-date {
    display: none;
  }
}
</style>