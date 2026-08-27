<template>
  <div class="system-summary">
    <div class="summary-header">
      <h3>📊 Resumen del Sistema</h3>
    </div>
    <div class="summary-cards">
      <div class="summary-card">
        <div class="card-icon">💾</div>
        <div class="card-info">
          <span class="card-number">{{ stats.discos }}</span>
          <span class="card-label">Discos</span>
        </div>
      </div>
      <div class="summary-card">
        <div class="card-icon">📁</div>
        <div class="card-info">
          <span class="card-number">{{ stats.particiones }}</span>
          <span class="card-label">Particiones</span>
        </div>
      </div>
      <div class="summary-card">
        <div class="card-icon">📂</div>
        <div class="card-info">
          <span class="card-number">{{ stats.archivos }}</span>
          <span class="card-label">Archivos</span>
        </div>
      </div>
      <div class="summary-card">
        <div class="card-icon">👤</div>
        <div class="card-info">
          <span class="card-number">{{ stats.usuarios }}</span>
          <span class="card-label">Usuarios</span>
        </div>
      </div>
      <div class="summary-card">
        <div class="card-icon">🔗</div>
        <div class="card-info">
          <span class="card-number">{{ stats.montadas }}</span>
          <span class="card-label">Montadas</span>
        </div>
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
        discos: 3,
        particiones: 6,
        archivos: 12,
        usuarios: 4,
        montadas: 2
      }
    }
  },
  methods: {
    refresh() {
      // TODO: Obtener datos reales del backend
      console.log('🔄 Actualizando resumen del sistema...')
    }
  }
}
</script>

<style scoped>
.system-summary {
  background: #161b22;
  border-radius: 12px;
  border: 1px solid #30363d;
  padding: 16px 20px;
}

.summary-header {
  margin-bottom: 12px;
}

.summary-header h3 {
  margin: 0;
  font-size: 14px;
  font-weight: 600;
  color: #e6edf3;
  letter-spacing: 0.3px;
}

.summary-cards {
  display: grid;
  grid-template-columns: repeat(auto-fit, minmax(100px, 1fr));
  gap: 12px;
}

.summary-card {
  background: #0d1117;
  border-radius: 10px;
  padding: 12px 14px;
  display: flex;
  align-items: center;
  gap: 12px;
  border: 1px solid #30363d;
  transition: all 0.3s ease;
}

.summary-card:hover {
  border-color: #58a6ff;
  transform: translateY(-2px);
  box-shadow: 0 4px 16px rgba(88, 166, 255, 0.08);
}

.card-icon {
  font-size: 24px;
  opacity: 0.8;
}

.card-info {
  display: flex;
  flex-direction: column;
}

.card-number {
  font-size: 20px;
  font-weight: 700;
  color: #e6edf3;
  line-height: 1.2;
}

.card-label {
  font-size: 11px;
  color: #8b949e;
  text-transform: uppercase;
  letter-spacing: 0.5px;
}

@media (max-width: 600px) {
  .system-summary {
    padding: 12px 14px;
  }
  .summary-cards {
    grid-template-columns: repeat(3, 1fr);
    gap: 8px;
  }
  .summary-card {
    padding: 10px;
    flex-direction: column;
    text-align: center;
    gap: 4px;
  }
  .card-icon {
    font-size: 20px;
  }
  .card-number {
    font-size: 16px;
  }
  .card-label {
    font-size: 9px;
  }
}
</style>