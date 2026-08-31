<template>
  <div class="reports-panel">
    <div class="reports-header">
      <span class="reports-title">Reportes</span>
      <span class="reports-badge">{{ reportCount }}</span>
    </div>

    <div class="reports-tabs">
      <button v-for="tab in reportTabs" :key="tab.key" class="tab-btn" :class="{ active: activeTab === tab.key }" @click="activeTab = tab.key">
        {{ tab.label }}
      </button>
    </div>

    <div class="reports-grid">
      <button v-for="report in currentReports" :key="report.name" class="report-btn" :class="{ 'is-generated': report.generated }" @click="generateReport(report)">
        <span class="report-name">{{ report.name }}</span>
        <span class="report-status">{{ report.generated ? '✓' : '·' }}</span>
      </button>
    </div>

    <div v-if="previewImage" class="report-preview">
      <div class="preview-header">
        <span class="preview-title">{{ previewName }}</span>
        <button class="preview-close" @click="closePreview">×</button>
      </div>
      <div class="preview-content">
        <img :src="previewImage" alt="Reporte" @error="handleImageError" />
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
      reportCount: 0,
      previewImage: null,
      previewName: '',
      reportTabs: [
        { key: 'sistema', label: 'Sistema' },
        { key: 'archivos', label: 'Archivos' },
        { key: 'bitmaps', label: 'Bitmaps' }
      ],
      reports: {
        sistema: [
          { name: 'MBR', generated: false },
          { name: 'DISK', generated: false },
          { name: 'SB', generated: false }
        ],
        archivos: [
          { name: 'INODE', generated: false },
          { name: 'BLOCK', generated: false },
          { name: 'TREE', generated: false },
          { name: 'LS', generated: false }
        ],
        bitmaps: [
          { name: 'bm_inode', generated: false },
          { name: 'bm_block', generated: false },
          { name: 'FILE', generated: false }
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
      this.previewName = report.name
      this.previewImage = `https://via.placeholder.com/400x200/161b22/58a6ff?text=${report.name}`
    },
    closePreview() {
      this.previewImage = null
      this.previewName = ''
    },
    handleImageError() {
      // Error silencioso
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

.reports-grid {
  display: grid;
  grid-template-columns: repeat(auto-fill, minmax(55px, 1fr));
  gap: 4px;
  flex: 1;
  overflow-y: auto;
  align-content: start;
}

.report-btn {
  display: flex;
  justify-content: space-between;
  align-items: center;
  padding: 2px 6px;
  background: #0d1117;
  border: 1px solid #30363d;
  border-radius: 4px;
  color: #8b949e;
  cursor: pointer;
  transition: all 0.3s ease;
  font-size: 9px;
}

.report-btn:hover {
  border-color: #58a6ff;
}

.report-btn.is-generated {
  border-color: #3fb950;
}

.report-name {
  color: #e6edf3;
}

.report-status {
  font-size: 10px;
}

.report-btn.is-generated .report-status {
  color: #3fb950;
}

.report-preview {
  margin-top: 4px;
  border-top: 1px solid #30363d;
  padding-top: 4px;
  flex-shrink: 0;
}

.preview-header {
  display: flex;
  justify-content: space-between;
  align-items: center;
  margin-bottom: 4px;
}

.preview-title {
  font-size: 9px;
  color: #8b949e;
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
  overflow: hidden;
  min-height: 60px;
  display: flex;
  align-items: center;
  justify-content: center;
}

.preview-content img {
  max-width: 100%;
  max-height: 120px;
  object-fit: contain;
  padding: 4px;
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