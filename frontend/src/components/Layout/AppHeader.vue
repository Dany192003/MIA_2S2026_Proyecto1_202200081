<template>
  <header class="app-header">
    <div class="header-content">
      <div class="logo">
        <h1>EXT2 <span>Analyzer</span></h1>
        <span class="subtitle">Sistema de Archivos</span>
      </div>

      <div class="header-actions">
        <div class="connection-status">
          <span class="status-dot" :class="isConnected ? 'online' : 'offline'"></span>
          <span class="status-text" :class="isConnected ? 'online' : 'offline'">
            {{ isConnected ? 'Conectado' : 'Desconectado' }}
          </span>
        </div>

        <div class="user-info">
          <span class="user-name">{{ currentUser || 'root' }}</span>
        </div>

        <button class="btn-refresh" @click="refresh" title="Actualizar">↻</button>
      </div>
    </div>
  </header>
</template>

<script>
export default {
  name: 'AppHeader',
  data() {
    return {
      isConnected: true,
      currentUser: 'root'
    }
  },
  methods: {
    refresh() {
      this.$emit('refresh')
    }
  }
}
</script>

<style scoped>
.app-header {
  background: #0d1117;
  border-bottom: 1px solid #30363d;
  padding: 8px 20px;
  flex-shrink: 0;
}

.header-content {
  max-width: 100%;
  margin: 0 auto;
  display: flex;
  align-items: center;
  justify-content: space-between;
  flex-wrap: wrap;
  gap: 8px;
}

.logo {
  display: flex;
  align-items: baseline;
  gap: 8px;
}

.logo h1 {
  font-size: 16px;
  font-weight: 300;
  color: #e6edf3;
  margin: 0;
}

.logo h1 span {
  color: #58a6ff;
  font-weight: 600;
}

.subtitle {
  font-size: 10px;
  color: #8b949e;
  letter-spacing: 0.3px;
}

.header-actions {
  display: flex;
  align-items: center;
  gap: 12px;
}

.connection-status {
  display: flex;
  align-items: center;
  gap: 4px;
  background: #161b22;
  padding: 2px 10px 2px 6px;
  border-radius: 12px;
  border: 1px solid #30363d;
  font-size: 10px;
}

.status-dot {
  width: 6px;
  height: 6px;
  border-radius: 50%;
  display: inline-block;
}

.status-dot.online {
  background: #3fb950;
}

.status-dot.offline {
  background: #f85149;
  animation: blink 1s ease-in-out infinite;
}

@keyframes blink {
  0%, 100% { opacity: 1; }
  50% { opacity: 0.3; }
}

.status-text.online {
  color: #3fb950;
}

.status-text.offline {
  color: #f85149;
}

.user-info {
  display: flex;
  align-items: center;
  gap: 4px;
  font-size: 12px;
}

.user-name {
  color: #e6edf3;
  font-weight: 500;
}

.btn-refresh {
  background: transparent;
  border: 1px solid #30363d;
  border-radius: 6px;
  color: #8b949e;
  padding: 2px 8px;
  cursor: pointer;
  font-size: 14px;
  transition: all 0.3s ease;
}

.btn-refresh:hover {
  background: #30363d;
  color: #e6edf3;
  border-color: #58a6ff;
}

@media (max-width: 480px) {
  .app-header {
    padding: 6px 12px;
  }
  .logo h1 {
    font-size: 13px;
  }
  .subtitle {
    display: none;
  }
  .status-text {
    font-size: 9px;
  }
}
</style>