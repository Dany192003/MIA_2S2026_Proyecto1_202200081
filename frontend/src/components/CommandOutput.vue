<template>
  <div class="command-output">
    <div class="output-header">
      <span class="output-label">Salida</span>
      <div class="header-actions">
        <button v-if="hasResults" @click="exportResults" class="btn-export">Exportar</button>
        <span class="badge" v-if="result" :class="result.success ? 'success' : 'error'">
          {{ result.success ? 'Exito' : 'Error' }}
        </span>
      </div>
    </div>

    <div v-if="!result && batchResults.length === 0" class="empty-state">
      <p>Esperando comando</p>
      <span>Ingrese un comando en el panel izquierdo</span>
    </div>

    <div v-else class="results-container">
      <div v-if="result && !result._batch" class="results">
        <div class="status-banner" :class="result.success ? 'success' : 'error'">
          <span class="status-icon">{{ result.success ? '✓' : '✗' }}</span>
          <span class="status-text">{{ result.message }}</span>
        </div>

        <div class="section" v-if="result.command">
          <div class="section-header">
            <span>Comando</span>
          </div>
          <div class="command-display">{{ result.command }}</div>
        </div>

        <div class="section error-section" v-if="result.errors && result.errors.length > 0">
          <div class="section-header">
            <span>Errores</span>
            <span class="error-count">{{ result.errors.length }}</span>
          </div>
          <div v-for="(error, index) in result.errors" :key="index" class="error-item">
            <span class="error-type">{{ error.type }}</span>
            <span class="error-message">{{ error.message }}</span>
            <span class="error-location">Linea {{ error.line }}, Columna {{ error.column }}</span>
          </div>
        </div>

        <div class="section" v-if="result.data && Object.keys(result.data).length > 0">
          <div class="section-header">
            <span>Datos</span>
          </div>
          <pre class="data-display">{{ JSON.stringify(result.data, null, 2) }}</pre>
        </div>

        <div class="section tokens-section" v-if="result.tokens && result.tokens.length > 0">
          <div class="section-header">
            <span>Tokens</span>
            <span class="token-count">{{ result.tokens.length }}</span>
          </div>
          <div class="tokens-table">
            <div class="tokens-header">
              <div class="token-col token-col-index">#</div>
              <div class="token-col token-col-type">Tipo</div>
              <div class="token-col token-col-value">Valor</div>
              <div class="token-col token-col-position">Pos</div>
            </div>
            <div v-for="(token, index) in result.tokens" :key="index" class="tokens-row">
              <div class="token-col token-col-index">{{ index + 1 }}</div>
              <div class="token-col token-col-type">
                <span class="token-type-badge">{{ token.type }}</span>
              </div>
              <div class="token-col token-col-value">{{ token.value }}</div>
              <div class="token-col token-col-position">{{ token.line }}:{{ token.column }}</div>
            </div>
          </div>
        </div>

        <div class="summary">
          <div class="summary-item">
            <span class="summary-label">Tokens</span>
            <span class="summary-value">{{ result.tokens ? result.tokens.length : 0 }}</span>
          </div>
          <div class="summary-item">
            <span class="summary-label">Errores</span>
            <span class="summary-value" :class="result.errors && result.errors.length > 0 ? 'error-text' : 'success-text'">
              {{ result.errors ? result.errors.length : 0 }}
            </span>
          </div>
          <div class="summary-item">
            <span class="summary-label">Estado</span>
            <span class="summary-value" :class="result.success ? 'success-text' : 'error-text'">
              {{ result.success ? 'Aceptado' : 'Rechazado' }}
            </span>
          </div>
        </div>
      </div>

      <div v-else-if="batchResults.length > 0" class="batch-results">
        <div class="batch-summary">
          <div class="batch-summary-header">
            <span>Ejecucion por Lote</span>
            <span class="batch-total">{{ batchTotal }} comandos</span>
          </div>
          <div class="batch-stats">
            <div class="batch-stat success-stat">
              <span class="stat-number">{{ batchSuccess }}</span>
              <span class="stat-label">Exitos</span>
            </div>
            <div class="batch-stat error-stat">
              <span class="stat-number">{{ batchFailed }}</span>
              <span class="stat-label">Fallos</span>
            </div>
            <div class="batch-stat total-stat">
              <span class="stat-number">{{ batchTotal }}</span>
              <span class="stat-label">Total</span>
            </div>
          </div>
        </div>

        <div class="batch-results-list">
          <div v-for="item in batchResults" :key="item.index" class="batch-result-item" :class="item.result.success ? 'batch-success' : 'batch-error'">
            <div class="batch-result-header">
              <span class="batch-result-index">#{{ item.index }}</span>
              <span class="batch-result-status">{{ item.result.success ? '✓' : '✗' }}</span>
              <span class="batch-result-command">{{ item.command }}</span>
            </div>
            <div class="batch-result-message">{{ item.result.message }}</div>
          </div>
        </div>
      </div>
    </div>
  </div>
</template>

<script>
export default {
  name: 'CommandOutput',
  props: {
    result: {
      type: Object,
      default: null
    }
  },
  data() {
    return {
      batchResults: [],
      batchTotal: 0,
      batchSuccess: 0,
      batchFailed: 0,
      history: []
    }
  },
  computed: {
    hasResults() {
      return this.result || this.batchResults.length > 0
    }
  },
  watch: {
    result: {
      handler(newResult) {
        if (!newResult) return

        if (newResult._batch) {
          const existingIndex = this.batchResults.findIndex(item => item.index === newResult._batch.index)
          if (existingIndex !== -1) {
            this.batchResults[existingIndex] = {
              command: newResult._batch.command,
              result: newResult,
              index: newResult._batch.index
            }
          } else {
            this.batchResults.push({
              command: newResult._batch.command,
              result: newResult,
              index: newResult._batch.index
            })
          }
          this.batchResults.sort((a, b) => a.index - b.index)
          this.batchTotal = this.batchResults.length
          this.batchSuccess = this.batchResults.filter(r => r.result.success).length
          this.batchFailed = this.batchResults.filter(r => !r.result.success).length
          this.history = this.batchResults.map(item => ({
            command: item.command,
            result: item.result,
            index: item.index
          }))
        } else {
          const historyEntry = {
            command: newResult.command,
            result: newResult,
            index: this.history.length + 1,
            timestamp: new Date().toLocaleString()
          }
          const lastIndex = this.history.length - 1
          if (lastIndex >= 0 && this.history[lastIndex].command === newResult.command) {
            this.history[lastIndex] = historyEntry
          } else {
            this.history.push(historyEntry)
          }
          if (this.batchResults.length > 0) {
            this.batchResults = []
            this.batchTotal = 0
            this.batchSuccess = 0
            this.batchFailed = 0
          }
        }
      },
      deep: true,
      immediate: true
    }
  },
  methods: {
    exportResults() {
      const now = new Date()
      const timestamp = now.toISOString().replace(/[:.]/g, '-').slice(0, 19)
      const filename = `resultados_${timestamp}.txt`

      let content = []
      content.push('='.repeat(70))
      content.push('           EXT2 ANALYZER - RESULTADOS')
      content.push('='.repeat(70))
      content.push(`Fecha: ${now.toLocaleString()}`)
      content.push('='.repeat(70))
      content.push('')

      if (this.batchResults.length > 0) {
        content.push('EJECUCION POR LOTE')
        content.push(`   Total: ${this.batchTotal} comandos`)
        content.push(`   Exitos: ${this.batchSuccess}`)
        content.push(`   Fallos: ${this.batchFailed}`)
        content.push('')
        content.push('-'.repeat(70))
        content.push('')

        this.batchResults.forEach(item => {
          const status = item.result.success ? 'EXITO' : 'FALLO'
          content.push(`[${String(item.index).padStart(3, ' ')}] ${status}`)
          content.push(`    Comando: ${item.command}`)
          content.push(`    Mensaje: ${item.result.message}`)
          if (item.result.errors && item.result.errors.length > 0) {
            content.push(`    Errores (${item.result.errors.length}):`)
            item.result.errors.forEach(err => {
              content.push(`      - ${err.type}: ${err.message} (Linea ${err.line}, Columna ${err.column})`)
            })
          }
          content.push('')
          content.push('-'.repeat(40))
          content.push('')
        })
      } else if (this.result) {
        const status = this.result.success ? 'EXITO' : 'FALLO'
        content.push('RESULTADO INDIVIDUAL')
        content.push(`   Estado: ${status}`)
        content.push(`   Comando: ${this.result.command}`)
        content.push(`   Mensaje: ${this.result.message}`)
        content.push('')
        content.push('-'.repeat(70))
        content.push('')

        if (this.result.errors && this.result.errors.length > 0) {
          content.push(`ERRORES (${this.result.errors.length}):`)
          this.result.errors.forEach(err => {
            content.push(`  ${err.type}: ${err.message} (Linea ${err.line}, Columna ${err.column})`)
          })
          content.push('')
        }

        if (this.result.tokens && this.result.tokens.length > 0) {
          content.push(`TOKENS (${this.result.tokens.length}):`)
          content.push('  #  TIPO        VALOR                    POSICION')
          content.push('  -- ---------- ------------------------ ----------')
          this.result.tokens.forEach((token, idx) => {
            const num = String(idx + 1).padStart(2, ' ')
            const type = token.type.padEnd(10, ' ')
            const value = `"${token.value}"`.padEnd(24, ' ')
            const pos = `${token.line}:${token.column}`
            content.push(`  ${num} ${type} ${value} ${pos}`)
          })
          content.push('')
        }

        if (this.result.data && Object.keys(this.result.data).length > 0) {
          content.push('DATOS:')
          content.push(JSON.stringify(this.result.data, null, 2))
          content.push('')
        }
      }

      content.push('='.repeat(70))
      content.push('           FIN DEL REPORTE')
      content.push('='.repeat(70))

      const blob = new Blob([content.join('\n')], { type: 'text/plain;charset=utf-8' })
      const url = URL.createObjectURL(blob)
      const link = document.createElement('a')
      link.href = url
      link.download = filename
      document.body.appendChild(link)
      link.click()
      document.body.removeChild(link)
      URL.revokeObjectURL(url)
    }
  }
}
</script>

<style scoped>
.command-output {
  display: flex;
  flex-direction: column;
  height: 100%;
  background: #0d1117;
  color: #e6edf3;
  font-size: 13px;
  overflow: hidden;
}

.output-header {
  display: flex;
  justify-content: space-between;
  align-items: center;
  padding-bottom: 4px;
  border-bottom: 1px solid #21262d;
  flex-shrink: 0;
}

.output-label {
  font-size: 10px;
  font-weight: 600;
  color: #8b949e;
  text-transform: uppercase;
  letter-spacing: 0.5px;
}

.header-actions {
  display: flex;
  align-items: center;
  gap: 8px;
}

.btn-export {
  padding: 2px 10px;
  background: #21262d;
  color: #8b949e;
  border: 1px solid #30363d;
  border-radius: 4px;
  font-size: 10px;
  cursor: pointer;
  transition: all 0.3s ease;
}

.btn-export:hover {
  border-color: #58a6ff;
  color: #e6edf3;
}

.badge {
  padding: 1px 10px;
  border-radius: 10px;
  font-size: 10px;
  font-weight: 600;
}

.badge.success {
  background: rgba(63, 185, 80, 0.15);
  color: #3fb950;
  border: 1px solid rgba(63, 185, 80, 0.2);
}

.badge.error {
  background: rgba(248, 81, 73, 0.15);
  color: #f85149;
  border: 1px solid rgba(248, 81, 73, 0.2);
}

.empty-state {
  flex: 1;
  display: flex;
  flex-direction: column;
  align-items: center;
  justify-content: center;
  color: #8b949e;
  text-align: center;
  padding: 30px 20px;
}

.empty-state p {
  margin: 0;
  font-size: 14px;
  color: #e6edf3;
}

.empty-state span {
  font-size: 12px;
  color: #8b949e;
  margin-top: 4px;
}

.results-container {
  flex: 1;
  overflow: hidden;
  display: flex;
  flex-direction: column;
  min-height: 0;
}

.results {
  flex: 1;
  overflow-y: auto;
  padding-right: 4px;
  display: flex;
  flex-direction: column;
  gap: 8px;
}

.batch-results {
  flex: 1;
  overflow-y: auto;
  padding-right: 4px;
}

.status-banner {
  display: flex;
  align-items: center;
  gap: 8px;
  padding: 8px 12px;
  border-radius: 6px;
  font-weight: 500;
  font-size: 13px;
  flex-shrink: 0;
}

.status-banner.success {
  background: rgba(63, 185, 80, 0.08);
  border-left: 3px solid #3fb950;
  color: #3fb950;
}

.status-banner.error {
  background: rgba(248, 81, 73, 0.08);
  border-left: 3px solid #f85149;
  color: #f85149;
}

.section {
  background: #161b22;
  border-radius: 6px;
  padding: 8px 12px;
  border: 1px solid #30363d;
  flex-shrink: 0;
}

.section-header {
  display: flex;
  align-items: center;
  gap: 6px;
  margin-bottom: 4px;
  font-size: 10px;
  font-weight: 600;
  color: #8b949e;
  text-transform: uppercase;
  letter-spacing: 0.5px;
}

.error-count, .token-count {
  margin-left: auto;
  background: #0d1117;
  color: #8b949e;
  padding: 0 8px;
  border-radius: 8px;
  font-size: 9px;
  border: 1px solid #30363d;
}

.error-count {
  background: rgba(248, 81, 73, 0.1);
  color: #f85149;
  border-color: rgba(248, 81, 73, 0.2);
}

.command-display {
  padding: 6px 10px;
  background: #0d1117;
  border-radius: 4px;
  font-family: 'Courier New', monospace;
  font-size: 12px;
  color: #e6edf3;
  border: 1px solid #30363d;
  word-break: break-all;
}

.error-section {
  border-left: 2px solid #f85149;
}

.error-item {
  display: flex;
  align-items: center;
  gap: 6px;
  padding: 4px 8px;
  background: #0d1117;
  border-radius: 4px;
  margin-bottom: 4px;
  font-size: 12px;
  flex-wrap: wrap;
  border-left: 2px solid #f85149;
}

.error-item:last-child {
  margin-bottom: 0;
}

.error-type {
  padding: 0 6px;
  background: #f85149;
  color: white;
  border-radius: 3px;
  font-size: 8px;
  font-weight: 600;
  text-transform: uppercase;
}

.error-message {
  color: #f85149;
  flex: 1;
}

.error-location {
  color: #8b949e;
  font-size: 10px;
}

.data-display {
  padding: 6px 10px;
  background: #0d1117;
  border-radius: 4px;
  font-family: 'Courier New', monospace;
  font-size: 11px;
  color: #aed581;
  border: 1px solid #30363d;
  overflow-x: auto;
  margin: 0;
  max-height: 100px;
  overflow-y: auto;
}

.tokens-section {
  border-top: 2px solid #58a6ff;
}

.tokens-table {
  background: #0d1117;
  border-radius: 6px;
  border: 1px solid #30363d;
  overflow: hidden;
}

.tokens-header {
  display: grid;
  grid-template-columns: 30px 70px 1fr 55px;
  background: #161b22;
  color: #8b949e;
  font-size: 9px;
  font-weight: 600;
  text-transform: uppercase;
  padding: 4px 8px;
  border-bottom: 1px solid #30363d;
}

.tokens-row {
  display: grid;
  grid-template-columns: 30px 70px 1fr 55px;
  padding: 3px 8px;
  font-size: 11px;
  border-bottom: 1px solid #21262d;
  align-items: center;
}

.tokens-row:last-child {
  border-bottom: none;
}

.tokens-row:hover {
  background: #161b22;
}

.token-col {
  display: flex;
  align-items: center;
}

.token-col-index {
  font-weight: 700;
  color: #8b949e;
  font-size: 9px;
  justify-content: center;
}

.token-col-type {
  justify-content: flex-start;
}

.token-col-value {
  justify-content: flex-start;
  font-family: 'Courier New', monospace;
  font-size: 11px;
  color: #e6edf3;
  word-break: break-all;
}

.token-col-position {
  justify-content: center;
  font-size: 9px;
  color: #8b949e;
  font-family: monospace;
}

.token-type-badge {
  padding: 0 6px;
  border-radius: 8px;
  font-size: 8px;
  font-weight: 600;
  text-transform: uppercase;
  background: #58a6ff;
  color: #0d1117;
}

.summary {
  display: grid;
  grid-template-columns: repeat(3, 1fr);
  gap: 6px;
  flex-shrink: 0;
}

.summary-item {
  background: #161b22;
  padding: 6px 8px;
  border-radius: 6px;
  text-align: center;
  border: 1px solid #30363d;
}

.summary-label {
  display: block;
  font-size: 8px;
  color: #8b949e;
  text-transform: uppercase;
  letter-spacing: 0.3px;
  font-weight: 600;
}

.summary-value {
  display: block;
  font-size: 16px;
  font-weight: 700;
  margin-top: 1px;
  color: #e6edf3;
}

.success-text {
  color: #3fb950;
}

.error-text {
  color: #f85149;
}

/* BATCH */
.batch-summary {
  background: #161b22;
  border-radius: 6px;
  padding: 10px 14px;
  border: 1px solid #30363d;
  border-top: 3px solid #58a6ff;
  margin-bottom: 8px;
}

.batch-summary-header {
  display: flex;
  align-items: center;
  gap: 8px;
  margin-bottom: 8px;
  font-size: 12px;
  font-weight: 600;
  color: #e6edf3;
}

.batch-total {
  margin-left: auto;
  font-size: 10px;
  color: #8b949e;
  background: #0d1117;
  padding: 1px 10px;
  border-radius: 10px;
  border: 1px solid #30363d;
}

.batch-stats {
  display: flex;
  gap: 8px;
}

.batch-stat {
  flex: 1;
  text-align: center;
  padding: 4px 6px;
  border-radius: 4px;
  background: #0d1117;
  border: 1px solid #30363d;
}

.stat-number {
  display: block;
  font-size: 18px;
  font-weight: 700;
}

.stat-label {
  font-size: 8px;
  color: #8b949e;
  text-transform: uppercase;
  letter-spacing: 0.3px;
}

.success-stat .stat-number { color: #3fb950; }
.error-stat .stat-number { color: #f85149; }
.total-stat .stat-number { color: #58a6ff; }

.batch-results-list {
  display: flex;
  flex-direction: column;
  gap: 4px;
}

.batch-result-item {
  padding: 6px 10px;
  border-radius: 4px;
  border-left: 3px solid;
  background: #161b22;
  border: 1px solid #30363d;
  border-left-width: 3px;
}

.batch-result-item.batch-success {
  border-left-color: #3fb950;
}

.batch-result-item.batch-error {
  border-left-color: #f85149;
}

.batch-result-header {
  display: flex;
  align-items: center;
  gap: 6px;
  flex-wrap: wrap;
}

.batch-result-index {
  font-size: 9px;
  font-weight: 700;
  color: #8b949e;
  min-width: 25px;
}

.batch-result-status {
  font-size: 12px;
}

.batch-result-command {
  font-family: 'Courier New', monospace;
  font-size: 11px;
  color: #e6edf3;
  word-break: break-all;
  flex: 1;
}

.batch-result-message {
  font-size: 11px;
  padding-left: 30px;
}

.batch-result-item.batch-success .batch-result-message {
  color: #3fb950;
}

.batch-result-item.batch-error .batch-result-message {
  color: #f85149;
}

::-webkit-scrollbar {
  width: 4px;
}

::-webkit-scrollbar-track {
  background: #0d1117;
}

::-webkit-scrollbar-thumb {
  background: #30363d;
  border-radius: 2px;
}

::-webkit-scrollbar-thumb:hover {
  background: #58a6ff;
}

@media (max-width: 480px) {
  .tokens-header,
  .tokens-row {
    grid-template-columns: 20px 50px 1fr 40px;
    font-size: 8px;
    padding: 2px 4px;
  }
  .token-col-value {
    font-size: 9px;
  }
  .token-type-badge {
    font-size: 7px;
    padding: 0 4px;
  }
  .summary {
    grid-template-columns: repeat(3, 1fr);
  }
}
</style>