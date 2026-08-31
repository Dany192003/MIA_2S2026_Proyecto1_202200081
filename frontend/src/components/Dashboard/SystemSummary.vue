<template>
  <div class="system-summary">
    <div class="summary-header">
      <span class="summary-title">Sistema</span>
    </div>
    <div class="summary-cards">
      <div class="summary-card">
        <span class="card-number">{{ stats.discos }}</span>
        <span class="card-label">Discos</span>
      </div>
      <div class="summary-card">
        <span class="card-number">{{ stats.particiones }}</span>
        <span class="card-label">Particiones</span>
      </div>
      <div class="summary-card">
        <span class="card-number">{{ stats.archivos }}</span>
        <span class="card-label">Archivos</span>
      </div>
      <div class="summary-card">
        <span class="card-number">{{ stats.usuarios }}</span>
        <span class="card-label">Usuarios</span>
      </div>
      <div class="summary-card">
        <span class="card-number">{{ stats.montadas }}</span>
        <span class="card-label">Montadas</span>
      </div>
    </div>
  </div>
</template>

<script>
export default {
  name: 'SystemSummary',
  data() {
    return {
      stats: {
        discos: 0,
        particiones: 0,
        archivos: 0,
        usuarios: 0,
        montadas: 0
      }
    }
  },
  methods: {
    refresh() {
      console.log('Actualizando resumen del sistema...')
    }
  }
}
</script>

<style scoped>
.system-summary {
  background: #161b22;
  border-radius: 8px;
  border: 1px solid #30363d;
  padding: 8px 12px;
  height: 100%;
  display: flex;
  flex-direction: column;
}

.summary-header {
  margin-bottom: 6px;
  flex-shrink: 0;
}

.summary-title {
  font-size: 10px;
  font-weight: 600;
  color: #8b949e;
  text-transform: uppercase;
  letter-spacing: 0.5px;
}

.summary-cards {
  display: grid;
  grid-template-columns: repeat(5, 1fr);
  gap: 6px;
  flex: 1;
}

.summary-card {
  background: #0d1117;
  border-radius: 6px;
  padding: 4px 6px;
  text-align: center;
  border: 1px solid #30363d;
  display: flex;
  flex-direction: column;
  justify-content: center;
}

.card-number {
  font-size: 18px;
  font-weight: 700;
  color: #e6edf3;
  line-height: 1.2;
}

.card-label {
  font-size: 8px;
  color: #8b949e;
  text-transform: uppercase;
  letter-spacing: 0.3px;
}

@media (max-width: 640px) {
  .summary-cards {
    grid-template-columns: repeat(3, 1fr);
  }
}

@media (max-width: 480px) {
  .system-summary {
    padding: 6px 8px;
  }
  .card-number {
    font-size: 14px;
  }
  .card-label {
    font-size: 7px;
  }
}
</style>