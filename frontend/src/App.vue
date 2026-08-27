<template>
  <div id="app">
    <!-- Header -->
    <AppHeader />

    <!-- Contenido principal -->
    <main class="main-content">
      <!-- TERMINAL: ocupa la mayor parte -->
      <div class="terminal-section">
        <CommandTerminal @command-executed="handleCommandExecuted" />
      </div>

      <!-- DASHBOARD: resumen en tarjetas -->
      <div class="dashboard-section">
        <SystemSummary ref="systemSummary" />
      </div>

      <!-- PANELES SECUNDARIOS: Discos + Explorador + Reportes -->
      <div class="panels-section">
        <div class="panel-item">
          <DiskList ref="diskList" />
        </div>
        <div class="panel-item">
          <FileExplorer ref="fileExplorer" />
        </div>
        <div class="panel-item">
          <ReportsPanel />
        </div>
      </div>
    </main>

    <!-- Footer -->
    <AppFooter />
  </div>
</template>

<script>
import AppHeader from './components/Layout/AppHeader.vue'
import AppFooter from './components/Layout/AppFooter.vue'
import SystemSummary from './components/Dashboard/SystemSummary.vue'
import DiskList from './components/Dashboard/DiskList.vue'
import FileExplorer from './components/Dashboard/FileExplorer.vue'
import ReportsPanel from './components/Dashboard/ReportsPanel.vue'
import CommandTerminal from './components/Terminal/CommandTerminal.vue'

export default {
  name: 'App',
  components: {
    AppHeader,
    AppFooter,
    SystemSummary,
    DiskList,
    FileExplorer,
    ReportsPanel,
    CommandTerminal
  },
  methods: {
    handleCommandExecuted(result) {
      this.$refs.systemSummary?.refresh()
      this.$refs.diskList?.refresh()
      this.$refs.fileExplorer?.refresh()
    }
  }
}
</script>

<style scoped>
#app {
  min-height: 100vh;
  display: flex;
  flex-direction: column;
  background: #0d1117;
  color: #e6edf3;
  font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;
}

.main-content {
  flex: 1;
  padding: 16px 24px;
  max-width: 100%;
  width: 100%;
  margin: 0 auto;
  display: flex;
  flex-direction: column;
  gap: 16px;
}

/* TERMINAL: ocupa la mayor parte - MÁS ALTO */
.terminal-section {
  flex: 1;
  min-height: 500px;
  max-height: 70vh;
}

.terminal-section .command-terminal {
  height: 100%;
}

/* DASHBOARD: tarjetas de resumen */
.dashboard-section {
  flex-shrink: 0;
}

/* PANELES SECUNDARIOS: 3 columnas */
.panels-section {
  display: grid;
  grid-template-columns: 1fr 1fr 1fr;
  gap: 16px;
  flex-shrink: 0;
}

.panel-item {
  min-height: 200px;
  max-height: 350px;
  overflow: hidden;
}

.panel-item > * {
  height: 100%;
  overflow-y: auto;
}

/* Responsive */
@media (max-width: 1024px) {
  .panels-section {
    grid-template-columns: 1fr 1fr;
  }
  .panels-section .panel-item:last-child {
    grid-column: span 2;
  }
}

@media (max-width: 768px) {
  .main-content {
    padding: 12px 16px;
    gap: 12px;
  }
  .terminal-section {
    min-height: 350px;
    max-height: 55vh;
  }
  .panels-section {
    grid-template-columns: 1fr;
  }
  .panels-section .panel-item:last-child {
    grid-column: span 1;
  }
  .panel-item {
    min-height: 150px;
    max-height: 250px;
  }
}

@media (max-width: 480px) {
  .main-content {
    padding: 8px 10px;
    gap: 8px;
  }
  .terminal-section {
    min-height: 280px;
    max-height: 50vh;
  }
  .panel-item {
    min-height: 120px;
    max-height: 200px;
  }
}
</style>