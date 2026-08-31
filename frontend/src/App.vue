<template>
  <div id="app">
    <AppHeader />

    <main class="main-content">
      <!-- TERMINAL: PROTAGONISTA - GRANDE -->
      <div class="terminal-section">
        <CommandTerminal @command-executed="handleCommandExecuted" />
      </div>

      <!-- PANELES INFERIORES: PEQUEÑOS -->
      <div class="panels-section">
        <div class="panel-item">
          <SystemSummary ref="systemSummary" />
        </div>
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

#app {
  min-height: 100vh;
  display: flex;
  flex-direction: column;
  background: #0d1117;
  color: #e6edf3;
  font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, sans-serif;
}

.main-content {
  flex: 1;
  padding: 10px 16px;
  display: flex;
  flex-direction: column;
  gap: 10px;
  min-height: 0;
  max-width: 100%;
  width: 100%;
  margin: 0 auto;
}

/* ============================================================
   TERMINAL: PROTAGONISTA - OCUPA CASI TODO
   ============================================================ */
.terminal-section {
  flex: 1;
  min-height: 500px;
  max-height: 70vh;
  overflow: hidden;
}

.terminal-section .command-terminal {
  height: 100%;
}

/* ============================================================
   PANELES INFERIORES: 4 COLUMNAS PEQUEÑAS
   ============================================================ */
.panels-section {
  display: grid;
  grid-template-columns: 1fr 1fr 1fr 1fr;
  gap: 10px;
  flex: 0 0 auto;
  min-height: 120px;
  max-height: 200px;
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
   RESPONSIVE
   ============================================================ */
@media (max-width: 1024px) {
  .panels-section {
    grid-template-columns: 1fr 1fr;
    max-height: 280px;
  }
  .terminal-section {
    min-height: 400px;
    max-height: 60vh;
  }
}

@media (max-width: 640px) {
  .main-content {
    padding: 6px 10px;
    gap: 6px;
  }
  .terminal-section {
    min-height: 300px;
    max-height: 55vh;
  }
  .panels-section {
    grid-template-columns: 1fr 1fr;
    max-height: 300px;
    min-height: 160px;
  }
}

@media (max-width: 480px) {
  .terminal-section {
    min-height: 250px;
    max-height: 50vh;
  }
  .panels-section {
    grid-template-columns: 1fr;
    max-height: 400px;
    min-height: 200px;
  }
}
</style>