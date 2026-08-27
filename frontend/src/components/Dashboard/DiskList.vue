<template>
  <div class="disk-list">
    <div class="disk-header">
      <h3>💾 Discos</h3>
      <span class="disk-count">{{ disks.length }} discos</span>
    </div>

    <div v-if="disks.length === 0" class="empty-state">
      <span class="empty-icon">💿</span>
      <p>No hay discos creados</p>
      <span class="empty-hint">Crea un disco usando el comando MKDISK</span>
    </div>

    <div v-else class="disk-items">
      <div v-for="disk in disks" :key="disk.name" class="disk-item">
        <div class="disk-info">
          <div class="disk-name">
            <span class="disk-icon">📀</span>
            <span class="disk-name-text">{{ disk.name }}</span>
            <span class="disk-size">{{ disk.size }} {{ disk.unit }}</span>
          </div>
          <div class="disk-usage">
            <span class="disk-usage-text">{{ disk.usedPercent }}% usado</span>
            <span class="disk-usage-detail">{{ disk.used }} / {{ disk.size }} {{ disk.unit }}</span>
          </div>
        </div>

        <div class="disk-progress">
          <div class="progress-bar" :style="{ width: disk.usedPercent + '%', background: disk.color }"></div>
        </div>

        <div class="disk-partitions">
          <div 
            v-for="part in disk.partitions" 
            :key="part.name" 
            class="partition-item"
            :class="{
              'partition-primary': part.type === 'P',
              'partition-extended': part.type === 'E',
              'partition-logical': part.type === 'L'
            }"
          >
            <span class="partition-icon">
              {{ part.type === 'P' ? '📁' : part.type === 'E' ? '📦' : '📄' }}
            </span>
            <span class="partition-name">{{ part.name }}</span>
            <span class="partition-size">{{ part.size }} {{ part.unit }}</span>
            <span class="partition-type">{{ part.typeLabel }}</span>
            <span v-if="part.mounted" class="partition-mounted">🔗 Montada</span>
          </div>
        </div>
      </div>
    </div>
  </div>
</template>

<script>
export default {
  name: 'DiskList',
  data() {
    return {
      disks: [
        {
          name: 'disco1.mia',
          size: 20,
          unit: 'MB',
          used: 15,
          usedPercent: 75,
          color: '#58a6ff',
          partitions: [
            { name: 'Part1', size: 5, unit: 'MB', type: 'P', typeLabel: 'Primaria', mounted: true },
            { name: 'Part2', size: 10, unit: 'MB', type: 'E', typeLabel: 'Extendida', mounted: false },
            { name: 'Part3', size: 3, unit: 'MB', type: 'L', typeLabel: 'Lógica', mounted: false },
            { name: 'Part4', size: 3, unit: 'MB', type: 'L', typeLabel: 'Lógica', mounted: false }
          ]
        },
        {
          name: 'disco2.mia',
          size: 10,
          unit: 'MB',
          used: 8,
          usedPercent: 80,
          color: '#3fb950',
          partitions: [
            { name: 'Datos', size: 8, unit: 'MB', type: 'P', typeLabel: 'Primaria', mounted: false }
          ]
        },
        {
          name: 'disco3.mia',
          size: 5,
          unit: 'MB',
          used: 2,
          usedPercent: 40,
          color: '#d29922',
          partitions: []
        }
      ]
    }
  },
  methods: {
    refresh() {
      // TODO: Obtener datos reales del backend
      console.log('🔄 Actualizando lista de discos...')
    }
  }
}
</script>

<style scoped>
.disk-list {
  background: #161b22;
  border-radius: 12px;
  border: 1px solid #30363d;
  padding: 16px 20px;
}

.disk-header {
  display: flex;
  justify-content: space-between;
  align-items: center;
  margin-bottom: 14px;
}

.disk-header h3 {
  margin: 0;
  font-size: 14px;
  font-weight: 600;
  color: #e6edf3;
  letter-spacing: 0.3px;
}

.disk-count {
  font-size: 12px;
  color: #8b949e;
  background: #0d1117;
  padding: 2px 10px;
  border-radius: 12px;
  border: 1px solid #30363d;
}

.empty-state {
  display: flex;
  flex-direction: column;
  align-items: center;
  justify-content: center;
  padding: 30px 20px;
  color: #8b949e;
  text-align: center;
}

.empty-icon {
  font-size: 40px;
  opacity: 0.4;
  margin-bottom: 10px;
}

.empty-state p {
  margin: 0;
  font-size: 14px;
  color: #e6edf3;
}

.empty-hint {
  font-size: 12px;
  color: #8b949e;
  margin-top: 4px;
}

.disk-items {
  display: flex;
  flex-direction: column;
  gap: 16px;
}

.disk-item {
  background: #0d1117;
  border-radius: 10px;
  padding: 14px 16px;
  border: 1px solid #30363d;
  transition: all 0.3s ease;
}

.disk-item:hover {
  border-color: #58a6ff;
}

.disk-info {
  display: flex;
  justify-content: space-between;
  align-items: center;
  margin-bottom: 8px;
  flex-wrap: wrap;
  gap: 6px;
}

.disk-name {
  display: flex;
  align-items: center;
  gap: 8px;
}

.disk-icon {
  font-size: 18px;
}

.disk-name-text {
  font-weight: 600;
  color: #e6edf3;
  font-size: 14px;
}

.disk-size {
  font-size: 12px;
  color: #8b949e;
  background: #161b22;
  padding: 0 8px;
  border-radius: 4px;
  border: 1px solid #30363d;
}

.disk-usage {
  display: flex;
  align-items: center;
  gap: 10px;
  font-size: 12px;
}

.disk-usage-text {
  color: #e6edf3;
  font-weight: 500;
}

.disk-usage-detail {
  color: #8b949e;
}

.disk-progress {
  width: 100%;
  height: 6px;
  background: #30363d;
  border-radius: 4px;
  overflow: hidden;
  margin-bottom: 10px;
}

.progress-bar {
  height: 100%;
  border-radius: 4px;
  transition: width 0.8s ease;
}

.disk-partitions {
  display: flex;
  flex-wrap: wrap;
  gap: 6px;
  padding-left: 4px;
}

.partition-item {
  display: flex;
  align-items: center;
  gap: 6px;
  padding: 3px 10px 3px 6px;
  border-radius: 6px;
  font-size: 11px;
  border: 1px solid #30363d;
  background: #161b22;
  transition: all 0.2s ease;
}

.partition-item:hover {
  transform: scale(1.02);
}

.partition-primary {
  border-left: 3px solid #58a6ff;
}

.partition-extended {
  border-left: 3px solid #d29922;
}

.partition-logical {
  border-left: 3px solid #3fb950;
}

.partition-icon {
  font-size: 12px;
}

.partition-name {
  color: #e6edf3;
  font-weight: 500;
}

.partition-size {
  color: #8b949e;
}

.partition-type {
  color: #8b949e;
  font-size: 9px;
  text-transform: uppercase;
  background: #0d1117;
  padding: 0 6px;
  border-radius: 3px;
}

.partition-mounted {
  color: #3fb950;
  font-size: 9px;
  display: flex;
  align-items: center;
  gap: 2px;
}

@media (max-width: 600px) {
  .disk-list {
    padding: 12px 14px;
  }
  .disk-info {
    flex-direction: column;
    align-items: flex-start;
  }
  .disk-usage {
    width: 100%;
    justify-content: space-between;
  }
  .partition-item {
    font-size: 10px;
    padding: 2px 6px 2px 4px;
  }
  .partition-size {
    display: none;
  }
}
</style>