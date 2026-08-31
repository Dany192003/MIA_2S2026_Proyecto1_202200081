<template>
  <div class="disk-list">
    <div class="disk-header">
      <span class="disk-title">Discos</span>
      <span class="disk-count">{{ disks.length }}</span>
    </div>

    <div v-if="disks.length === 0" class="empty-state">
      <p>Sin discos</p>
    </div>

    <div v-else class="disk-items">
      <div v-for="disk in disks" :key="disk.name" class="disk-item">
        <div class="disk-info">
          <span class="disk-name">{{ disk.name }}</span>
          <span class="disk-size">{{ disk.size }}{{ disk.unit }}</span>
        </div>
        <div class="disk-progress">
          <div class="progress-bar" :style="{ width: disk.usedPercent + '%', background: disk.color }"></div>
        </div>
        <div class="disk-partitions">
          <span v-for="part in disk.partitions" :key="part.name" class="partition-tag" :class="'tag-' + part.type.toLowerCase()">
            {{ part.name }}
          </span>
          <span v-if="disk.partitions.length === 0" class="partition-empty">vacio</span>
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
      disks: []
    }
  },
  methods: {
    refresh() {
      console.log('Actualizando lista de discos...')
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

.empty-state {
  flex: 1;
  display: flex;
  align-items: center;
  justify-content: center;
  color: #8b949e;
  font-size: 12px;
}

.disk-items {
  flex: 1;
  overflow-y: auto;
  display: flex;
  flex-direction: column;
  gap: 6px;
}

.disk-item {
  background: #0d1117;
  border-radius: 6px;
  padding: 6px 10px;
  border: 1px solid #30363d;
}

.disk-info {
  display: flex;
  justify-content: space-between;
  align-items: center;
  margin-bottom: 4px;
}

.disk-name {
  font-size: 12px;
  font-weight: 500;
  color: #e6edf3;
}

.disk-size {
  font-size: 10px;
  color: #8b949e;
}

.disk-progress {
  width: 100%;
  height: 3px;
  background: #30363d;
  border-radius: 2px;
  overflow: hidden;
  margin-bottom: 4px;
}

.progress-bar {
  height: 100%;
  border-radius: 2px;
  transition: width 0.5s ease;
}

.disk-partitions {
  display: flex;
  flex-wrap: wrap;
  gap: 3px;
}

.partition-tag {
  font-size: 8px;
  padding: 1px 6px;
  border-radius: 3px;
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
  font-size: 8px;
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