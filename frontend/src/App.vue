<template>
  <div id="app">
    <AppHeader />
    <main class="main-content">
      <div class="terminal-section">
        <CommandTerminal 
          @command-executed="handleCommandExecuted"
          @batch-completed="handleBatchCompleted"
        />
      </div>
      <div class="dashboard-section">
        <SystemSummary ref="systemSummary" />
      </div>
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
      // Solo refrescar si es un comando individual (no batch)
      if (!result || !result._batch) {
        this.refreshAll()
      }
    },
    handleBatchCompleted(results) {
      // Refrescar al final del batch
      this.refreshAll()
    },
    refreshAll() {
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
  background: #1e1e2e;
  color: #cdd6f4;
  font-family: 'JetBrains Mono', 'Fira Code', 'Consolas', monospace;
  overflow: hidden;
}

.main-content {
  flex: 1;
  padding: 8px 12px;
  display: flex;
  flex-direction: column;
  gap: 8px;
  min-height: 0;
  overflow: hidden;
}

.terminal-section {
  flex: 1;
  min-height: 400px;
  max-height: 70vh;
  overflow: hidden;
}

.terminal-section .command-terminal {
  height: 100%;
}

.panels-section {
  display: grid;
  grid-template-columns: 1fr 1fr 1fr;
  gap: 8px;
  flex: 0 0 auto;
  min-height: 100px;
  max-height: 160px;
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

@media (max-width: 1024px) {
  .panels-section {
    grid-template-columns: 1fr 1fr;
    max-height: 220px;
  }
}

@media (max-width: 768px) {
  .terminal-section {
    min-height: 300px;
    max-height: 55vh;
  }
  .panels-section {
    grid-template-columns: 1fr 1fr;
    max-height: 240px;
    min-height: 140px;
  }
}

@media (max-width: 480px) {
  .terminal-section {
    min-height: 250px;
    max-height: 50vh;
  }
  .panels-section {
    grid-template-columns: 1fr;
    max-height: 320px;
    min-height: 180px;
  }
}
</style>