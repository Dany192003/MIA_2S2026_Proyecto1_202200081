<template>
  <div class="reports-panel">
    <div class="reports-header">
      <h3>📈 Reportes</h3>
      <span class="reports-badge">{{ reportCount }} generados</span>
    </div>

    <div class="reports-tabs">
      <button 
        v-for="tab in reportTabs" 
        :key="tab.key"
        class="tab-btn"
        :class="{ active: activeTab === tab.key }"
        @click="activeTab = tab.key"
      >
        {{ tab.label }}
      </button>
    </div>

    <div class="reports-grid">
      <button 
        v-for="report in currentReports" 
        :key="report.name"
        class="report-btn"
        :class="{ 'is-generated': report.generated }"
        @click="generateReport(report)"
      >
        <span class="report-icon">{{ report.icon }}</span>
        <span class="report-name">{{ report.name }}</span>
        <span class="report-status" :class="report.generated ? 'status-ok' : 'status-pending'">
          {{ report.generated ? '✅' : '⏳' }}
        </span>
      </button>
    </div>

    <div v-if="previewImage" class="report-preview">
      <div class="preview-header">
        <span class="preview-title">📸 Vista previa: {{ previewName }}</span>
        <button class="preview-close" @click="closePreview">✕</button>
      </div>
      <div class="preview-content">
        <img :src="previewImage" alt="Reporte" @error="handleImageError" />
      </div>
      <div class="preview-actions">
        <button class="btn-download" @click="downloadReport">📥 Descargar</button>
        <button class="btn-download-dot" @click="downloadDot">📄 Descargar .dot</button>
      </div>
    </div>
  </div>
</template>

<script>
export default {
  name: 'ReportsPanel',
  data() {
    return {
      activeTab: 'sistema',
      reportCount: 3,
      previewImage: null,
      previewName: '',
      reportTabs: [
        { key: 'sistema', label: 'Sistema' },
        { key: 'archivos', label: 'Archivos' },
        { key: 'bitmaps', label: 'Bitmaps' }
      ],
      reports: {
        sistema: [
          { name: 'MBR', icon: '💾', generated: true },
          { name: 'DISK', icon: '💿', generated: true },
          { name: 'SB', icon: '📋', generated: true }
        ],
        archivos: [
          { name: 'INODE', icon: '📄', generated: true },
          { name: 'BLOCK', icon: '📦', generated: false },
          { name: 'TREE', icon: '🌳', generated: false },
          { name: 'LS', icon: '📂', generated: false }
        ],
        bitmaps: [
          { name: 'bm_inode', icon: '🔢', generated: false },
          { name: 'bm_block', icon: '🔢', generated: false },
          { name: 'FILE', icon: '📝', generated: false }
        ]
      }
    }
  },
  computed: {
    currentReports() {
      return this.reports[this.activeTab] || []
    }
  },
  methods: {
    generateReport(report) {
      report.generated = true
      this.reportCount = this.reportCount + 1
      
      // Simular preview
      this.previewName = report.name
      this.previewImage = `https://via.placeholder.com/600x400/161b22/58a6ff?text=${report.name}`
      
      console.log(`📊 Generando reporte: ${report.name}`)
    },
    closePreview() {
      this.previewImage = null
      this.previewName = ''
    },
    handleImageError() {
      console.warn('⚠️ No se pudo cargar la imagen del reporte')
    },
    downloadReport() {
      console.log('📥 Descargando reporte...')
    },
    downloadDot() {
      console.log('📄 Descargando archivo .dot...')
    }
  }
}
</script>

<style scoped>
.reports-panel {
  background: #161b22;
  border-radius: 12px;
  border: 1px solid #30363d;
  padding: 16px 20px;
}

.reports-header {
  display: flex;
  justify-content: space-between;
  align-items: center;
  margin-bottom: 12px;
}

.reports-header h3 {
  margin: 0;
  font-size: 14px;
  font-weight: 600;
  color: #e6edf3;
}

.reports-badge {
  font-size: 11px;
  color: #3fb950;
  background: rgba(63, 185, 80, 0.12);
  padding: 2px 10px;
  border-radius: 12px;
  border: 1px solid rgba(63, 185, 80, 0.2);
}

.reports-tabs {
  display: flex;
  gap: 4px;
  margin-bottom: 12px;
  border-bottom: 1px solid #30363d;
  padding-bottom: 8px;
}

.tab-btn {
  background: transparent;
  border: none;
  color: #8b949e;
  padding: 4px 14px;
  font-size: 12px;
  font-weight: 500;
  cursor: pointer;
  border-radius: 6px;
  transition: all 0.3s ease;
}

.tab-btn:hover {
  color: #e6edf3;
  background: #21262d;
}

.tab-btn.active {
  color: #58a6ff;
  background: rgba(88, 166, 255, 0.12);
}

.reports-grid {
  display: grid;
  grid-template-columns: repeat(auto-fill, minmax(90px, 1fr));
  gap: 8px;
  margin-bottom: 12px;
}

.report-btn {
  display: flex;
  flex-direction: column;
  align-items: center;
  gap: 2px;
  padding: 8px 4px;
  background: #0d1117;
  border: 1px solid #30363d;
  border-radius: 8px;
  color: #8b949e;
  cursor: pointer;
  transition: all 0.3s ease;
  font-size: 11px;
}

.report-btn:hover {
  border-color: #58a6ff;
  transform: translateY(-2px);
  box-shadow: 0 4px 12px rgba(88, 166, 255, 0.1);
}

.report-btn.is-generated {
  border-color: #3fb950;
}

.report-icon {
  font-size: 20px;
}

.report-name {
  font-weight: 500;
  color: #e6edf3;
}

.report-status {
  font-size: 10px;
}

.status-ok {
  color: #3fb950;
}

.status-pending {
  color: #8b949e;
}

.report-preview {
  margin-top: 12px;
  border-top: 1px solid #30363d;
  padding-top: 12px;
}

.preview-header {
  display: flex;
  justify-content: space-between;
  align-items: center;
  margin-bottom: 8px;
}

.preview-title {
  font-size: 12px;
  color: #8b949e;
}

.preview-close {
  background: transparent;
  border: none;
  color: #8b949e;
  font-size: 18px;
  cursor: pointer;
  padding: 0 4px;
}

.preview-close:hover {
  color: #f85149;
}

.preview-content {
  background: #0d1117;
  border-radius: 8px;
  border: 1px solid #30363d;
  overflow: hidden;
  min-height: 100px;
  display: flex;
  align-items: center;
  justify-content: center;
}

.preview-content img {
  max-width: 100%;
  max-height: 300px;
  object-fit: contain;
  padding: 8px;
}

.preview-actions {
  display: flex;
  gap: 8px;
  margin-top: 8px;
}

.btn-download, .btn-download-dot {
  padding: 4px 14px;
  border-radius: 6px;
  font-size: 12px;
  font-weight: 500;
  cursor: pointer;
  transition: all 0.3s ease;
  border: 1px solid #30363d;
}

.btn-download {
  background: #58a6ff;
  color: #0d1117;
  border-color: #58a6ff;
}

.btn-download:hover {
  background: #79c0ff;
}

.btn-download-dot {
  background: transparent;
  color: #8b949e;
}

.btn-download-dot:hover {
  border-color: #58a6ff;
  color: #e6edf3;
}

@media (max-width: 600px) {
  .reports-panel {
    padding: 12px 14px;
  }
  .reports-grid {
    grid-template-columns: repeat(auto-fill, minmax(70px, 1fr));
  }
  .report-btn {
    padding: 6px 2px;
    font-size: 10px;
  }
  .report-icon {
    font-size: 16px;
  }
  .preview-content img {
    max-height: 200px;
  }
  .preview-actions {
    flex-direction: column;
  }
}
</style>