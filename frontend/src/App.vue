<template>
  <div id="app">
    <AppHeader />

    <main class="main-content">
      <!-- TERMINAL -->
      <div class="terminal-section">
        <CommandTerminal @command-executed="handleCommandExecuted" />
      </div>

      <!-- DASHBOARD - SIEMPRE ABAJO -->
      <div class="dashboard-section">
        <SystemSummary ref="systemSummary" />
      </div>

      <!-- PANELES INFERIORES -->
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
* {
  box-sizing: border-box;
}

html, body {
  margin: 0;
  padding: 0;
  height: 100%;
  overflow: hidden;
}

#app {
  height: 100vh;
  display: flex;
  flex-direction: column;
  background: #0d1117;
  color: #e6edf3;
  font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, sans-serif;
  overflow: hidden;
}

/* ============================================================
   HEADER: FIJO ARRIBA
   ============================================================ */
.app-header {
  flex-shrink: 0;
}

/* ============================================================
   MAIN: OCUPA TODO EL ESPACIO RESTANTE CON SCROLL
   ============================================================ */
.main-content {
  flex: 1;
  padding: 10px 16px;
  display: flex;
  flex-direction: column;
  gap: 10px;
  overflow-y: auto;
  min-height: 0;
}

/* ============================================================
   TERMINAL: TAMAÑO FLEXIBLE CON MINIMO
   ============================================================ */
.terminal-section {
  flex: 0 0 auto;
  min-height: 350px;
  max-height: 65vh;
  overflow: hidden;
}

.terminal-section .command-terminal {
  height: 100%;
}

/* ============================================================
   DASHBOARD: SIEMPRE VISIBLE
   ============================================================ */
.dashboard-section {
  flex: 0 0 auto;
}

/* ============================================================
   PANELES INFERIORES: 3 COLUMNAS
   ============================================================ */
.panels-section {
  display: grid;
  grid-template-columns: 1fr 1fr 1fr;
  gap: 10px;
  flex: 0 0 auto;
  min-height: 140px;
  max-height: 220px;
}

.panel-item {
  min-height: 0;
  max-height: 100%;
  overflow: hidden;
  display: flex;
  flex-direction: column;
}

.panel-item > * {
  flex: 1;
  overflow-y: auto;
  min-height: 0;
}

/* ============================================================
   FOOTER: FIJO ABAJO
   ============================================================ */
.app-footer {
  flex-shrink: 0;
}

/* ============================================================
   RESPONSIVE
   ============================================================ */
@media (max-width: 1024px) {
  .panels-section {
    grid-template-columns: 1fr 1fr;
    max-height: 260px;
  }
  .terminal-section {
    min-height: 300px;
    max-height: 55vh;
  }
}

@media (max-width: 768px) {
  .main-content {
    padding: 8px 12px;
    gap: 8px;
  }
  .terminal-section {
    min-height: 250px;
    max-height: 50vh;
  }
  .panels-section {
    grid-template-columns: 1fr 1fr;
    max-height: 280px;
    min-height: 160px;
  }
}

@media (max-width: 480px) {
  .main-content {
    padding: 6px 8px;
    gap: 6px;
  }
  .terminal-section {
    min-height: 200px;
    max-height: 45vh;
  }
  .panels-section {
    grid-template-columns: 1fr;
    max-height: 350px;
    min-height: 180px;
  }
}
</style>