<template>
  <div id="app">
    <header class="header">
      <div class="header-content">
        <div class="logo">
          <span class="logo-icon">◆</span>
          <h1>EXT2 <span>Analyzer</span></h1>
        </div>
        <p class="subtitle">Analizador Léxico y Sintáctico de Comandos</p>
      </div>
    </header>
    
    <main class="main-content">
      <div class="container">
        <div class="panel panel-left">
          <CommandInput @command-submitted="handleCommand" />
        </div>
        <div class="panel panel-right">
          <CommandOutput :result="result" />
        </div>
      </div>
    </main>
    
    <footer class="footer">
      <div class="footer-content">
        <span>MIA - 2S 2026</span>
        <span>Práctica 1</span>
        <span>Carnet: 202200081</span>
      </div>
    </footer>
  </div>
</template>

<script>
import CommandInput from './components/CommandInput.vue'
import CommandOutput from './components/CommandOutput.vue'

export default {
  name: 'App',
  components: {
    CommandInput,
    CommandOutput
  },
  data() {
    return {
      result: null
    }
  },
  methods: {
    handleCommand(result) {
      this.result = result
    }
  }
}
</script>

<style>
* {
  margin: 0;
  padding: 0;
  box-sizing: border-box;
}

body {
  background: #f0f4f8;
  font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;
}

#app {
  min-height: 100vh;
  display: flex;
  flex-direction: column;
  background: linear-gradient(135deg, #eef3f8 0%, #dce8f0 100%);
}

.header {
  background: linear-gradient(135deg, #1a2a3a 0%, #2c3e50 100%);
  padding: 20px 0;
  box-shadow: 0 2px 12px rgba(0, 0, 0, 0.2);
}

.header-content {
  max-width: 1400px;
  margin: 0 auto;
  padding: 0 30px;
  display: flex;
  align-items: center;
  justify-content: space-between;
  flex-wrap: wrap;
}

.logo {
  display: flex;
  align-items: center;
  gap: 14px;
}

.logo-icon {
  font-size: 28px;
  color: #5b9bd5;
  font-weight: 300;
}

.header h1 {
  font-size: 26px;
  font-weight: 300;
  color: #ffffff;
  letter-spacing: 1px;
}

.header h1 span {
  color: #5b9bd5;
  font-weight: 600;
}

.subtitle {
  color: rgba(255, 255, 255, 0.7);
  font-size: 13px;
  font-weight: 300;
  letter-spacing: 0.5px;
  background: rgba(255, 255, 255, 0.08);
  padding: 4px 16px;
  border-radius: 20px;
  border: 1px solid rgba(255, 255, 255, 0.06);
}

.main-content {
  flex: 1;
  padding: 24px 30px;
  max-width: 1440px;
  width: 100%;
  margin: 0 auto;
}

.container {
  display: flex;
  gap: 24px;
  min-height: 500px;
}

.panel {
  flex: 1;
  background: rgba(255, 255, 255, 0.92);
  backdrop-filter: blur(8px);
  border-radius: 16px;
  padding: 24px;
  box-shadow: 0 4px 24px rgba(0, 0, 0, 0.06);
  border: 1px solid rgba(255, 255, 255, 0.5);
  transition: all 0.3s ease;
  min-width: 0;
}

.panel:hover {
  box-shadow: 0 6px 32px rgba(0, 0, 0, 0.08);
}

.panel-left {
  border-top: 3px solid #2c3e50;
}

.panel-right {
  border-top: 3px solid #5b9bd5;
}

.footer {
  background: rgba(26, 42, 58, 0.95);
  padding: 14px 30px;
  margin-top: auto;
  border-top: 1px solid rgba(255, 255, 255, 0.06);
}

.footer-content {
  max-width: 1400px;
  margin: 0 auto;
  display: flex;
  justify-content: center;
  gap: 40px;
  color: rgba(255, 255, 255, 0.5);
  font-size: 12px;
  font-weight: 300;
  letter-spacing: 0.5px;
}

.footer-content span {
  padding: 0 12px;
  border-right: 1px solid rgba(255, 255, 255, 0.08);
}

.footer-content span:last-child {
  border-right: none;
}

::-webkit-scrollbar {
  width: 6px;
  height: 6px;
}

::-webkit-scrollbar-track {
  background: rgba(0, 0, 0, 0.04);
  border-radius: 10px;
}

::-webkit-scrollbar-thumb {
  background: #5b9bd5;
  border-radius: 10px;
}

::-webkit-scrollbar-thumb:hover {
  background: #4a8bc4;
}

@media (max-width: 768px) {
  .container {
    flex-direction: column;
    min-height: auto;
  }
  .panel {
    min-height: 280px;
    border-radius: 14px;
  }
  .header-content {
    flex-direction: column;
    align-items: center;
    text-align: center;
    gap: 8px;
  }
  .header h1 {
    font-size: 20px;
  }
  .subtitle {
    font-size: 11px;
  }
  .main-content {
    padding: 16px;
  }
  .footer-content {
    flex-wrap: wrap;
    gap: 4px 16px;
  }
  .footer-content span {
    border-right: none;
    padding: 0 6px;
  }
}

@media (max-width: 480px) {
  .header {
    padding: 14px 0;
  }
  .header h1 {
    font-size: 17px;
  }
  .panel {
    padding: 16px;
    min-height: 220px;
    border-radius: 12px;
  }
  .main-content {
    padding: 12px;
  }
}
</style>