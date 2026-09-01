<template>
  <div class="disk-list">
    <div class="disk-header">
      <span class="disk-title">Discos</span>
      <span class="disk-count">{{ disks.length }}</span>
      <button class="btn-refresh" @click="refresh" title="Actualizar">↻</button>
    </div>

    <div v-if="loading" class="loading-state">
      <span>Cargando...</span>
    </div>

    <div v-else-if="disks.length === 0" class="empty-state">
      <p>Sin discos</p>
      <span class="hint">Usa mkdisk para crear uno</span>
    </div>

    <div v-else class="disk-items">
      <div v-for="disk in disks" :key="disk.path" class="disk-item">
        <div class="disk-info">
          <span class="disk-name">{{ disk.name }}</span>
          <span class="disk-size">{{ disk.size }}</span>
        </div>
        <div class="disk-path">{{ disk.path }}</div>
        <div class="disk-partitions">
          <span v-for="part in disk.partitions" :key="part.name" 
                class="partition-tag" :class="'tag-' + part.type.toLowerCase()">
            {{ part.name }} ({{ part.size }})
          </span>
          <span v-if="disk.partitions.length === 0" class="partition-empty">sin particiones</span>
        </div>
      </div>
    </div>
  </div>
</template>

<script>
import { analyzeCommand } from '../../services/api.js'

export default {
  name: 'DiskList',
  data() {
    return {
      disks: [],
      loading: false
    }
  },
  mounted() {
    this.refresh()
  },
  methods: {
    async refresh() {
      this.loading = true
      try {
        // Obtener discos físicos (archivos .mia)
        const result = await analyzeCommand('mounted')
        if (result.success && result.data && result.data.mounted) {
          // Por ahora, mostrar discos montados
          // TODO: Escanear carpeta discos/ para encontrar todos los .mia
          this.disks = result.data.mounted.map(m => ({
            name: m.disk.split('/').pop(),
            path: m.disk,
            size: '10 MB', // TODO: Obtener tamaño real
            partitions: [
              { name: 'part1', size: '5 MB', type: 'P' }
            ]
          }))
        }
      } catch (error) {
        console.error('Error actualizando discos:', error)
      }
      this.loading = false
    }
  }
}
</script>

<style scoped>
.disk-list {
  background: #161b22;
  border-radius: 8px;
  border: 1px solid #30363d;
  padding: 8px 12px;
  height: 100%;
  display: flex;
  flex-direction: column;
  overflow: hidden;
}

.disk-header {
  display: flex;
  justify-content: space-between;
  align-items: center;
  margin-bottom: 6px;
  flex-shrink: 0;
}

.disk-title {
  font-size: 10px;
  font-weight: 600;
  color: #8b949e;
  text-transform: uppercase;
  letter-spacing: 0.5px;
}

.disk-count {
  font-size: 10px;
  color: #8b949e;
  background: #0d1117;
  padding: 0 8px;
  border-radius: 8px;
  border: 1px solid #30363d;
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

.loading-state, .empty-state {
  flex: 1;
  display: flex;
  flex-direction: column;
  align-items: center;
  justify-content: center;
  color: #8b949e;
  font-size: 12px;
}

.hint {
  font-size: 9px;
  color: #30363d;
  margin-top: 4px;
}

.disk-items {
  flex: 1;
  overflow-y: auto;
  display: flex;
  flex-direction: column;
  gap: 4px;
}

.disk-item {
  background: #0d1117;
  border-radius: 6px;
  padding: 4px 8px;
  border: 1px solid #30363d;
}

.disk-info {
  display: flex;
  justify-content: space-between;
  align-items: center;
}

.disk-name {
  font-size: 11px;
  font-weight: 500;
  color: #e6edf3;
}

.disk-size {
  font-size: 9px;
  color: #8b949e;
}

.disk-path {
  font-size: 8px;
  color: #30363d;
  font-family: monospace;
  margin-bottom: 2px;
  overflow: hidden;
  text-overflow: ellipsis;
  white-space: nowrap;
}

.disk-partitions {
  display: flex;
  flex-wrap: wrap;
  gap: 2px;
}

.partition-tag {
  font-size: 7px;
  padding: 0 4px;
  border-radius: 2px;
  background: #21262d;
  color: #8b949e;
  border: 1px solid #30363d;
}

.partition-tag.tag-p {
  border-color: #58a6ff;
  color: #58a6ff;
}

.partition-tag.tag-e {
  border-color: #d29922;
  color: #d29922;
}

.partition-tag.tag-l {
  border-color: #3fb950;
  color: #3fb950;
}

.partition-empty {
  font-size: 7px;
  color: #30363d;
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