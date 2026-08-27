<template>
  <div class="command-terminal">
    <!-- Header -->
    <div class="terminal-header">
      <div class="terminal-dots">
        <span class="dot dot-red"></span>
        <span class="dot dot-yellow"></span>
        <span class="dot dot-green"></span>
      </div>
      <span class="terminal-title">Terminal de Comandos</span>
      <div class="terminal-actions">
        <button class="btn-clear-terminal" @click="clearTerminal" title="Limpiar terminal">
          🗑
        </button>
      </div>
    </div>

    <!-- Cuerpo: Grid 50/50 - MÁS ALTO -->
    <div class="terminal-grid">
      <div class="terminal-left">
        <CommandInput @command-submitted="handleCommand" />
      </div>
      <div class="terminal-right">
        <CommandOutput :result="currentResult" />
      </div>
    </div>

    <!-- Footer -->
    <div class="terminal-footer">
      <span class="terminal-hint">💡 Ctrl+Enter para ejecutar | Carga archivos .smia o .txt</span>
    </div>
  </div>
</template>

<script>
import CommandInput from '../CommandInput.vue'
import CommandOutput from '../CommandOutput.vue'

export default {
  name: 'CommandTerminal',
  components: {
    CommandInput,
    CommandOutput
  },
  data() {
    return {
      currentResult: null
    }
  },
  methods: {
    handleCommand(result) {
      this.currentResult = result
      this.$emit('command-executed', result)
    },
    clearTerminal() {
      this.currentResult = null
    }
  }
}
</script>

<style scoped>
.command-terminal {
  background: #0d1117;
  border-radius: 12px;
  border: 1px solid #30363d;
  overflow: hidden;
  display: flex;
  flex-direction: column;
  height: 100%;
  min-height: 450px;
}

.terminal-header {
  display: flex;
  align-items: center;
  gap: 12px;
  padding: 8px 16px;
  background: #161b22;
  border-bottom: 1px solid #30363d;
  flex-shrink: 0;
}

.terminal-dots {
  display: flex;
  gap: 6px;
}

.dot {
  width: 10px;
  height: 10px;
  border-radius: 50%;
  display: inline-block;
}

.dot-red { background: #f85149; }
.dot-yellow { background: #d29922; }
.dot-green { background: #3fb950; }

.terminal-title {
  flex: 1;
  font-size: 12px;
  color: #8b949e;
  font-weight: 500;
  text-align: center;
}

.terminal-actions {
  display: flex;
  gap: 4px;
}

.btn-clear-terminal {
  background: transparent;
  border: none;
  color: #8b949e;
  cursor: pointer;
  font-size: 14px;
  padding: 2px 6px;
  border-radius: 4px;
  transition: all 0.3s ease;
}

.btn-clear-terminal:hover {
  color: #f85149;
  background: rgba(248, 81, 73, 0.1);
}

/* ===== GRID MÁS ALTO ===== */
.terminal-grid {
  display: grid;
  grid-template-columns: 1fr 1.2fr;
  gap: 0;
  flex: 1;
  min-height: 350px;
}

.terminal-left {
  padding: 14px 18px;
  border-right: 1px solid #30363d;
  overflow-y: auto;
  background: #0d1117;
  display: flex;
  flex-direction: column;
}

.terminal-left .command-input {
  height: 100%;
}

.terminal-right {
  padding: 14px 18px;
  overflow-y: auto;
  background: #0d1117;
  display: flex;
  flex-direction: column;
}

.terminal-right .command-output {
  height: 100%;
}

.terminal-footer {
  padding: 6px 16px;
  background: #161b22;
  border-top: 1px solid #30363d;
  text-align: center;
  flex-shrink: 0;
}

.terminal-hint {
  font-size: 11px;
  color: #8b949e;
}

/* Responsive */
@media (max-width: 768px) {
  .command-terminal {
    min-height: 350px;
  }
  .terminal-grid {
    grid-template-columns: 1fr;
    min-height: 300px;
  }
  .terminal-left {
    border-right: none;
    border-bottom: 1px solid #30363d;
    max-height: 280px;
    padding: 10px 14px;
  }
  .terminal-right {
    max-height: 350px;
    padding: 10px 14px;
  }
}

@media (max-width: 480px) {
  .command-terminal {
    min-height: 300px;
  }
  .terminal-left {
    max-height: 220px;
    padding: 8px 10px;
  }
  .terminal-right {
    max-height: 280px;
    padding: 8px 10px;
  }
  .terminal-header {
    padding: 6px 10px;
  }
  .terminal-title {
    font-size: 10px;
  }
}
</style>