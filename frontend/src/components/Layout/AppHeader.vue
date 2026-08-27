<template>
  <header class="app-header">
    <div class="header-content">
      <div class="logo">
        <div class="logo-icon">
          <svg viewBox="0 0 24 24" width="32" height="32" fill="none" stroke="currentColor" stroke-width="2">
            <rect x="2" y="2" width="20" height="20" rx="2" />
            <line x1="8" y1="2" x2="8" y2="22" />
            <line x1="16" y1="2" x2="16" y2="22" />
            <line x1="2" y1="8" x2="22" y2="8" />
            <line x1="2" y1="16" x2="22" y2="16" />
          </svg>
        </div>
        <div>
          <h1>EXT2 <span>Analyzer</span></h1>
          <p class="subtitle">Administrador de Discos y Sistema de Archivos</p>
        </div>
      </div>

      <div class="header-actions">
        <div class="connection-status">
          <span class="status-dot" :class="isConnected ? 'online' : 'offline'"></span>
          <span class="status-text" :class="isConnected ? 'online' : 'offline'">
            {{ isConnected ? 'Conectado' : 'Desconectado' }}
          </span>
        </div>

        <div class="user-info">
          <span class="user-icon">👤</span>
          <span class="user-name">{{ currentUser || 'root' }}</span>
        </div>

        <button class="btn-refresh" @click="refresh" title="Actualizar">
          <svg viewBox="0 0 24 24" width="18" height="18" fill="none" stroke="currentColor" stroke-width="2">
            <path d="M21 12a9 9 0 01-9 9m9-9a9 9 0 00-9-9m9 9H3m9 9a9 9 0 01-9-9m9 9c1.66 0 3-4.03 3-9s-1.34-9-3-9m0 18c-1.66 0-3-4.03-3-9s1.34-9 3-9" />
          </svg>
        </button>
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
  background: linear-gradient(135deg, #0d1117 0%, #161b22 100%);
  border-bottom: 1px solid #30363d;
  padding: 12px 24px;
  position: sticky;
  top: 0;
  z-index: 100;
  backdrop-filter: blur(8px);
}

.header-content {
  max-width: 1600px;
  margin: 0 auto;
  display: flex;
  align-items: center;
  justify-content: space-between;
  flex-wrap: wrap;
  gap: 12px;
}

.logo {
  display: flex;
  align-items: center;
  gap: 12px;
}

.logo-icon {
  color: #58a6ff;
  display: flex;
  align-items: center;
}

.logo h1 {
  font-size: 20px;
  font-weight: 300;
  color: #e6edf3;
  margin: 0;
  letter-spacing: 0.5px;
}

.logo h1 span {
  color: #58a6ff;
  font-weight: 600;
}

.subtitle {
  font-size: 11px;
  color: #8b949e;
  margin: 0;
  letter-spacing: 0.3px;
}

.header-actions {
  display: flex;
  align-items: center;
  gap: 16px;
}

.connection-status {
  display: flex;
  align-items: center;
  gap: 6px;
  background: #161b22;
  padding: 4px 12px 4px 8px;
  border-radius: 20px;
  border: 1px solid #30363d;
}

.status-dot {
  width: 8px;
  height: 8px;
  border-radius: 50%;
  display: inline-block;
  transition: all 0.3s ease;
}

.status-dot.online {
  background: #3fb950;
  box-shadow: 0 0 8px rgba(63, 185, 80, 0.3);
}

.status-dot.offline {
  background: #f85149;
  box-shadow: 0 0 8px rgba(248, 81, 73, 0.3);
  animation: blink 1s ease-in-out infinite;
}

@keyframes blink {
  0%, 100% { opacity: 1; }
  50% { opacity: 0.3; }
}

.status-text {
  font-size: 12px;
  font-weight: 500;
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
  gap: 6px;
  color: #8b949e;
  font-size: 13px;
}

.user-icon {
  font-size: 16px;
}

.user-name {
  color: #e6edf3;
  font-weight: 500;
}

.btn-refresh {
  background: transparent;
  border: 1px solid #30363d;
  border-radius: 8px;
  color: #8b949e;
  padding: 6px 8px;
  cursor: pointer;
  display: flex;
  align-items: center;
  justify-content: center;
  transition: all 0.3s ease;
}

.btn-refresh:hover {
  background: #30363d;
  color: #e6edf3;
  border-color: #58a6ff;
}

.btn-refresh svg {
  transition: transform 0.6s ease;
}

.btn-refresh:hover svg {
  transform: rotate(180deg);
}

@media (max-width: 600px) {
  .app-header {
    padding: 10px 16px;
  }
  .logo h1 {
    font-size: 16px;
  }
  .subtitle {
    font-size: 10px;
  }
  .header-actions {
    gap: 10px;
  }
  .status-text {
    font-size: 10px;
  }
  .user-name {
    font-size: 11px;
  }
}
</style>