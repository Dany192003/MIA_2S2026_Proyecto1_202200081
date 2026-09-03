<template>
  <div class="reports-panel">
    <div class="reports-header">
      <span class="reports-title">Reportes</span>
      <span class="reports-badge">{{ reportFiles.length }}</span>
      <button class="btn-refresh" @click="refresh" title="Actualizar">↻</button>
    </div>

    <div class="reports-tabs">
      <button v-for="tab in reportTabs" :key="tab.key" 
              class="tab-btn" :class="{ active: activeTab === tab.key }" 
              @click="activeTab = tab.key">
        {{ tab.label }}
      </button>
    </div>

    <div class="reports-list">
      <div v-if="filteredReports.length === 0" class="empty-state">
        <p>No hay reportes generados</p>
        <span class="hint">Usa el comando <kbd>rep</kbd> para generar reportes</span>
      </div>
      <div v-for="report in filteredReports" :key="report.path" 
           class="report-item"
           @click="viewReport(report)">
        <div class="report-info">
          <span class="report-icon">{{ report.type === 'image' ? '🖼️' : '📄' }}</span>
          <span class="report-name">{{ report.displayName }}</span>
          <span class="report-size">{{ formatSize(report.size) }}</span>
        </div>
        <div class="report-status">
          <span class="status-ok">✅ {{ report.path }}</span>
        </div>
      </div>
    </div>

    <!-- Vista previa -->
    <div v-if="selectedReport" class="report-preview">
      <div class="preview-header">
        <span class="preview-title">{{ selectedReport.displayName }}</span>
        <span class="preview-path">{{ selectedReport.path }}</span>
        <button class="preview-close" @click="closePreview">×</button>
      </div>
      <div class="preview-content">
        <img v-if="selectedReport.type === 'image'" 
             :src="selectedReport.preview || selectedReport.path" 
             alt="Reporte" 
             @error="handleImageError" />
        <pre v-else-if="selectedReport.type === 'text'" class="text-preview">{{ previewContent || 'Contenido del reporte...' }}</pre>
        <span v-else class="preview-info">📊 Archivo .dot (Graphviz)</span>
      </div>
      <div class="preview-actions">
        <button class="btn-download" @click="downloadReport">📥 Descargar</button>
        <button class="btn-copy" @click="copyPath">📋 Copiar ruta</button>
      </div>
    </div>
  </div>
</template>

<script>
import { analyzeCommand } from '../../services/api.js'

export default {
  name: 'ReportsPanel',
  data() {
    return {
      activeTab: 'sistema',
      selectedReport: null,
      previewContent: '',
      reportFiles: [],
      reportTabs: [
        { key: 'sistema', label: 'Sistema' },
        { key: 'archivos', label: 'Archivos' },
        { key: 'bitmaps', label: 'Bitmaps' }
      ],
      reportTypes: {
        'mbr': 'sistema',
        'disk': 'sistema',
        'sb': 'sistema',
        'inode': 'archivos',
        'block': 'archivos',
        'tree': 'archivos',
        'ls': 'archivos',
        'bm_inode': 'bitmaps',
        'bm_block': 'bitmaps',
        'file': 'archivos'  // ✅ Cambiado de 'bitmaps' a 'archivos'
      }
    }
  },
  computed: {
    filteredReports() {
      return this.reportFiles
        .filter(r => {
          // ✅ Filtrar archivos .dot
          if (r.extension === '.dot') return false
          
          let baseName = r.name.replace(/\.(png|txt)$/, '').toLowerCase()
          baseName = baseName.replace(/_[0-9]+$/, '')
          const tab = this.reportTypes[baseName] || 'sistema'
          return tab === this.activeTab
        })
        .map(r => {
          let baseName = r.name.replace(/\.(png|txt)$/, '').toLowerCase()
          baseName = baseName.replace(/_[0-9]+$/, '')
          return {
            ...r,
            displayName: baseName.toUpperCase(),
            type: r.extension === '.png' ? 'image' : r.extension === '.txt' ? 'text' : 'dot'
          }
        })
    }
  },
  mounted() {
    this.refresh()
  },
  methods: {
    async refresh() {
      try {
        const result = await analyzeCommand('lsreports')
        if (result.success && result.data?.data?.reports) {
          // ✅ Filtrar archivos .dot
          this.reportFiles = result.data.data.reports.filter(r => 
            r.extension !== '.dot'
          )
        } else {
          this.reportFiles = []
        }
      } catch (error) {
        console.error('Error escaneando reportes:', error)
        this.reportFiles = []
      }
    },
    formatSize(bytes) {
      if (!bytes || bytes === 0) return '0 B'
      const k = 1024
      const sizes = ['B', 'KB', 'MB', 'GB']
      const i = Math.floor(Math.log(bytes) / Math.log(k))
      return parseFloat((bytes / Math.pow(k, i)).toFixed(1)) + ' ' + sizes[i]
    },
    viewReport(report) {
      this.selectedReport = report
      this.previewContent = ''
      
      if (report.type === 'text') {
        // Cargar contenido del reporte de texto
        this.loadTextReport(report)
      }
    },
    async loadTextReport(report) {
      try {
        // Intentar leer el contenido del archivo de texto
        const result = await analyzeCommand(`cat -file1=${report.path}`)
        if (result.success && result.data?.data?.content) {
          this.previewContent = result.data.data.content
        } else {
          this.previewContent = 'No se pudo cargar el contenido del reporte'
        }
      } catch (error) {
        console.error('Error cargando reporte de texto:', error)
        this.previewContent = 'Error al cargar el contenido'
      }
    },
    closePreview() {
      this.selectedReport = null
    },
    handleImageError() {
      console.warn('Error cargando imagen del reporte')
      this.selectedReport = null
    },
    downloadReport() {
      if (this.selectedReport) {
        alert(`📥 Descargar reporte:\n${this.selectedReport.path}`)
      }
    },
    copyPath() {
      if (this.selectedReport) {
        navigator.clipboard.writeText(this.selectedReport.path)
          .then(() => alert('📋 Ruta copiada al portapapeles'))
          .catch(() => alert('📋 Ruta: ' + this.selectedReport.path))
      }
    }
  }
}
</script>

<style scoped>
.reports-panel {
  background: #161b22;
  border-radius: 8px;
  border: 1px solid #30363d;
  padding: 8px 12px;
  height: 100%;
  display: flex;
  flex-direction: column;
  overflow: hidden;
}

.reports-header {
  display: flex;
  justify-content: space-between;
  align-items: center;
  margin-bottom: 4px;
  flex-shrink: 0;
}

.reports-title {
  font-size: 10px;
  font-weight: 600;
  color: #8b949e;
  text-transform: uppercase;
  letter-spacing: 0.5px;
}

.reports-badge {
  font-size: 9px;
  color: #3fb950;
  background: rgba(63, 185, 80, 0.1);
  padding: 0 8px;
  border-radius: 8px;
  border: 1px solid rgba(63, 185, 80, 0.2);
}

.btn-refresh {
  background: transparent;
  border: 1px solid #30363d;
  color: #8b949e;
  border-radius: 4px;
  padding: 0 6px;
  cursor: pointer;
  font-size: 12px;
  transition: all 0.3s ease;
}

.btn-refresh:hover {
  border-color: #58a6ff;
  color: #e6edf3;
}

.reports-tabs {
  display: flex;
  gap: 3px;
  margin-bottom: 4px;
  border-bottom: 1px solid #30363d;
  padding-bottom: 4px;
  flex-shrink: 0;
}

.tab-btn {
  background: transparent;
  border: none;
  color: #8b949e;
  padding: 1px 8px;
  font-size: 9px;
  font-weight: 500;
  cursor: pointer;
  border-radius: 4px;
  transition: all 0.3s ease;
}

.tab-btn:hover {
  color: #e6edf3;
  background: #21262d;
}

.tab-btn.active {
  color: #58a6ff;
  background: rgba(88, 166, 255, 0.1);
}

.reports-list {
  flex: 1;
  overflow-y: auto;
  display: flex;
  flex-direction: column;
  gap: 2px;
}

.empty-state {
  display: flex;
  flex-direction: column;
  align-items: center;
  justify-content: center;
  color: #8b949e;
  font-size: 11px;
  padding: 20px 10px;
  text-align: center;
}

.empty-state kbd {
  background: #0d1117;
  padding: 0 6px;
  border-radius: 3px;
  border: 1px solid #30363d;
  font-family: monospace;
  font-size: 10px;
  color: #e6edf3;
}

.hint {
  font-size: 9px;
  color: #30363d;
  margin-top: 4px;
}

.report-item {
  display: flex;
  justify-content: space-between;
  align-items: center;
  padding: 3px 8px;
  border-radius: 4px;
  background: #0d1117;
  border: 1px solid #30363d;
  cursor: pointer;
  transition: all 0.3s ease;
  min-height: 26px;
}

.report-item:hover {
  border-color: #58a6ff;
  background: #161b22;
}

.report-info {
  display: flex;
  align-items: center;
  gap: 6px;
}

.report-icon {
  font-size: 12px;
}

.report-name {
  font-size: 10px;
  font-weight: 500;
  color: #e6edf3;
}

.report-size {
  font-size: 8px;
  color: #30363d;
}

.report-status {
  font-size: 8px;
  max-width: 120px;
  overflow: hidden;
  text-overflow: ellipsis;
  white-space: nowrap;
}

.status-ok {
  color: #3fb950;
  font-size: 8px;
}

.report-preview {
  margin-top: 4px;
  border-top: 1px solid #30363d;
  padding-top: 4px;
  flex-shrink: 0;
  max-height: 160px;
  overflow: hidden;
  display: flex;
  flex-direction: column;
}

.preview-header {
  display: flex;
  align-items: center;
  gap: 6px;
  margin-bottom: 4px;
}

.preview-title {
  font-size: 9px;
  font-weight: 600;
  color: #e6edf3;
}

.preview-path {
  font-size: 7px;
  color: #30363d;
  font-family: monospace;
  flex: 1;
  overflow: hidden;
  text-overflow: ellipsis;
  white-space: nowrap;
}

.preview-close {
  background: transparent;
  border: none;
  color: #8b949e;
  font-size: 14px;
  cursor: pointer;
}

.preview-close:hover {
  color: #f85149;
}

.preview-content {
  background: #0d1117;
  border-radius: 4px;
  border: 1px solid #30363d;
  overflow: auto;
  min-height: 40px;
  max-height: 90px;
  display: flex;
  align-items: center;
  justify-content: center;
  padding: 4px;
}

.preview-content img {
  max-width: 100%;
  max-height: 80px;
  object-fit: contain;
}

.text-preview {
  font-size: 8px;
  color: #a6e3a1;
  font-family: monospace;
  margin: 0;
  padding: 4px;
  max-height: 80px;
  overflow: auto;
  white-space: pre-wrap;
  width: 100%;
}

.preview-info {
  font-size: 9px;
  color: #8b949e;
}

.preview-actions {
  display: flex;
  gap: 4px;
  margin-top: 4px;
}

.btn-download, .btn-copy {
  background: transparent;
  border: 1px solid #30363d;
  color: #8b949e;
  padding: 1px 8px;
  border-radius: 3px;
  font-size: 8px;
  cursor: pointer;
  transition: all 0.3s ease;
}

.btn-download:hover, .btn-copy:hover {
  border-color: #58a6ff;
  color: #e6edf3;
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