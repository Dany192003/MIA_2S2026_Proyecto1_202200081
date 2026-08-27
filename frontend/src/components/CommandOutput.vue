<template>
  <div class="command-output">
    <div class="output-header">
      <div class="header-left">
        <span class="icon">◆</span>
        <h3>Resultado del Analisis</h3>
      </div>
      <div class="header-actions">
        <button 
          v-if="hasResults" 
          @click="exportResults" 
          class="btn-export"
          title="Exportar resultados a TXT"
        >
          Exportar
        </button>
        <span class="badge" v-if="result" :class="result.success ? 'success-badge' : 'error-badge'">
          {{ result.success ? 'Valido' : 'Error' }}
        </span>
      </div>
    </div>
    
    <div v-if="!result && batchResults.length === 0" class="empty-state">
      <div class="empty-icon">◆</div>
      <p>Esperando comando</p>
      <span>Ingresa un comando en el panel izquierdo</span>
    </div>
    
    <div v-else>
      
      <div v-if="result && !result._batch" class="results">
        
        <div class="status-banner" :class="result.success ? 'success' : 'error'">
          <span class="status-icon">{{ result.success ? '✓' : '✗' }}</span>
          <span class="status-text">{{ result.message }}</span>
        </div>
        
        <div class="section">
          <div class="section-header">
            <span class="section-icon">⌨</span>
            <h4>Comando ingresado</h4>
          </div>
          <div class="command-display">{{ result.command }}</div>
        </div>
        
        <div v-if="result.errors && result.errors.length > 0" class="section error-section">
          <div class="section-header">
            <span class="section-icon">✗</span>
            <h4>Errores detectados</h4>
            <span class="error-count">{{ result.errors.length }}</span>
          </div>
          <div v-for="(error, index) in result.errors" :key="index" class="error-item">
            <span class="error-type">{{ error.type }}</span>
            <span class="error-message">{{ error.message }}</span>
            <span class="error-location">Linea {{ error.line }}, Columna {{ error.column }}</span>
          </div>
        </div>
        
        <div v-if="result.data" class="section">
          <div class="section-header">
            <span class="section-icon">{ }</span>
            <h4>Datos extraidos</h4>
            <span class="data-badge">JSON</span>
          </div>
          <pre class="data-display">{{ JSON.stringify(result.data, null, 2) }}</pre>
        </div>
        
        <div v-if="result.tokens && result.tokens.length > 0" class="section tokens-section">
          <div class="section-header">
            <span class="section-icon">T</span>
            <h4>Analisis Lexico - Tokens</h4>
            <span class="token-count">{{ result.tokens.length }} tokens</span>
          </div>
          
          <div class="tokens-table">
            <div class="tokens-header">
              <div class="token-col token-col-index">#</div>
              <div class="token-col token-col-type">Tipo</div>
              <div class="token-col token-col-value">Valor</div>
              <div class="token-col token-col-position">Posicion</div>
            </div>
            <div 
              v-for="(token, index) in result.tokens" 
              :key="index" 
              class="tokens-row"
            >
              <div class="token-col token-col-index">{{ index + 1 }}</div>
              <div class="token-col token-col-type">
                <span class="token-type-badge" :class="'type-' + token.type.toLowerCase()">
                  {{ token.type }}
                </span>
              </div>
              <div class="token-col token-col-value">
                <span class="token-value-display">"{{ token.value }}"</span>
              </div>
              <div class="token-col token-col-position">
                <span class="token-position">{{ token.line }}:{{ token.column }}</span>
              </div>
            </div>
          </div>
        </div>
        
        <div class="summary">
          <div class="summary-item">
            <span class="summary-label">Total de tokens</span>
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
            <span class="batch-icon">◈</span>
            <h4>Ejecucion por Lote</h4>
            <span class="batch-total">{{ batchTotal }} comandos</span>
          </div>
          <div class="batch-stats">
            <div class="batch-stat success-stat">
              <span class="stat-number">{{ batchSuccess }}</span>
              <span class="stat-label">Aceptados</span>
            </div>
            <div class="batch-stat error-stat">
              <span class="stat-number">{{ batchFailed }}</span>
              <span class="stat-label">Rechazados</span>
            </div>
            <div class="batch-stat total-stat">
              <span class="stat-number">{{ batchTotal }}</span>
              <span class="stat-label">Total</span>
            </div>
          </div>
        </div>
        
        <div class="batch-results-list">
          <div 
            v-for="item in batchResults" 
            :key="item.index"
            class="batch-result-item"
            :class="item.result.success ? 'batch-success' : 'batch-error'"
          >
            <div class="batch-result-header">
              <span class="batch-result-index">#{{ item.index }}</span>
              <span class="batch-result-status">{{ item.result.success ? '✓' : '✗' }}</span>
              <span class="batch-result-command">{{ item.command }}</span>
            </div>
            <div class="batch-result-message">{{ item.result.message }}</div>
            <div v-if="item.result.errors && item.result.errors.length > 0" class="batch-result-errors">
              <span v-for="(err, idx) in item.result.errors" :key="idx" class="batch-error-detail">
                - {{ err.message }}
              </span>
            </div>
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
        if (!newResult) {
          return
        }
        
        if (newResult._batch) {
          const existingIndex = this.batchResults.findIndex(
            item => item.index === newResult._batch.index
          )
          
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
      content.push('           EXT2 ANALYZER - REPORTE DE RESULTADOS')
      content.push('='.repeat(70))
      content.push(`Fecha: ${now.toLocaleString()}`)
      content.push('='.repeat(70))
      content.push('')
      
      if (this.batchResults.length > 0) {
        content.push('EJECUCION POR LOTE')
        content.push(`   Total: ${this.batchTotal} comandos`)
        content.push(`   Aceptados: ${this.batchSuccess}`)
        content.push(`   Rechazados: ${this.batchFailed}`)
        content.push('')
        content.push('-'.repeat(70))
        content.push('')
        
        this.batchResults.forEach(item => {
          const status = item.result.success ? 'ACEPTADO' : 'RECHAZADO'
          content.push(`[${String(item.index).padStart(3, ' ')}] ${status}`)
          content.push(`    Comando: ${item.command}`)
          content.push(`    Mensaje: ${item.result.message}`)
          
          if (item.result.errors && item.result.errors.length > 0) {
            content.push(`    Errores (${item.result.errors.length}):`)
            item.result.errors.forEach(err => {
              content.push(`      - ${err.type}: ${err.message} (Linea ${err.line}, Columna ${err.column})`)
            })
          }
          
          if (item.result.tokens && item.result.tokens.length > 0) {
            content.push(`    Tokens (${item.result.tokens.length}):`)
            item.result.tokens.forEach(token => {
              content.push(`      ${token.type}: "${token.value}" (${token.line}:${token.column})`)
            })
          }
          
          if (item.result.data && Object.keys(item.result.data).length > 0) {
            content.push(`    Datos: ${JSON.stringify(item.result.data)}`)
          }
          
          content.push('')
          content.push('-'.repeat(40))
          content.push('')
        })
        
      } else if (this.result) {
        const status = this.result.success ? 'ACEPTADO' : 'RECHAZADO'
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
          content.push('DATOS EXTRAIDOS:')
          content.push(JSON.stringify(this.result.data, null, 2))
          content.push('')
        }
      } else {
        content.push('No hay resultados para exportar.')
        content.push('')
      }
      
      content.push('='.repeat(70))
      content.push('           FIN DEL REPORTE')
      content.push('='.repeat(70))
      
      const finalContent = content.join('\n')
      
      const blob = new Blob([finalContent], { type: 'text/plain;charset=utf-8' })
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
  gap: 12px;
  height: 100%;
  background: #0d1117;
  color: #e6edf3;
  font-size: 13px;
}

/* ===== HEADER ===== */
.output-header {
  display: flex;
  justify-content: space-between;
  align-items: center;
  padding-bottom: 10px;
  border-bottom: 1px solid #30363d;
  flex-wrap: wrap;
  gap: 8px;
}

.header-left {
  display: flex;
  align-items: center;
  gap: 8px;
}

.icon {
  font-size: 16px;
  color: #8b949e;
}

.output-header h3 {
  margin: 0;
  font-size: 14px;
  font-weight: 600;
  color: #e6edf3;
  letter-spacing: 0.3px;
}

.header-actions {
  display: flex;
  align-items: center;
  gap: 10px;
  flex-wrap: wrap;
}

.btn-export {
  padding: 4px 14px;
  background: #21262d;
  color: #8b949e;
  border: 1px solid #30363d;
  border-radius: 6px;
  font-size: 11px;
  font-weight: 500;
  cursor: pointer;
  transition: all 0.3s ease;
}

.btn-export:hover {
  border-color: #58a6ff;
  color: #e6edf3;
}

.badge {
  padding: 2px 12px;
  border-radius: 12px;
  font-size: 11px;
  font-weight: 600;
}

.success-badge {
  background: rgba(63, 185, 80, 0.15);
  color: #3fb950;
  border: 1px solid rgba(63, 185, 80, 0.2);
}

.error-badge {
  background: rgba(248, 81, 73, 0.15);
  color: #f85149;
  border: 1px solid rgba(248, 81, 73, 0.2);
}

/* ===== EMPTY STATE ===== */
.empty-state {
  flex: 1;
  display: flex;
  flex-direction: column;
  align-items: center;
  justify-content: center;
  color: #8b949e;
  text-align: center;
  padding: 40px 20px;
}

.empty-icon {
  font-size: 40px;
  margin-bottom: 12px;
  opacity: 0.3;
  color: #30363d;
}

.empty-state p {
  margin: 0;
  font-size: 15px;
  color: #e6edf3;
}

.empty-state span {
  font-size: 12px;
  color: #8b949e;
  margin-top: 4px;
}

/* ===== RESULTS ===== */
.results {
  display: flex;
  flex-direction: column;
  gap: 10px;
  overflow-y: auto;
  max-height: 540px;
  padding-right: 4px;
}

/* ===== STATUS BANNER ===== */
.status-banner {
  display: flex;
  align-items: center;
  gap: 10px;
  padding: 10px 14px;
  border-radius: 8px;
  font-weight: 500;
  font-size: 13px;
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

.status-icon {
  font-size: 15px;
}

/* ===== SECTIONS ===== */
.section {
  background: #161b22;
  border-radius: 8px;
  padding: 10px 14px;
  border: 1px solid #30363d;
}

.section-header {
  display: flex;
  align-items: center;
  gap: 6px;
  margin-bottom: 6px;
}

.section-icon {
  font-size: 12px;
  color: #8b949e;
}

.section-header h4 {
  margin: 0;
  font-size: 10px;
  font-weight: 600;
  color: #8b949e;
  text-transform: uppercase;
  letter-spacing: 0.5px;
}

.error-count, .token-count, .data-badge {
  margin-left: auto;
  background: #0d1117;
  color: #8b949e;
  padding: 0 10px;
  border-radius: 10px;
  font-size: 9px;
  font-weight: 600;
  border: 1px solid #30363d;
}

.error-count {
  background: rgba(248, 81, 73, 0.1);
  color: #f85149;
  border-color: rgba(248, 81, 73, 0.2);
}

.data-badge {
  background: rgba(88, 166, 255, 0.1);
  color: #58a6ff;
  border-color: rgba(88, 166, 255, 0.2);
}

/* ===== COMMAND DISPLAY ===== */
.command-display {
  padding: 8px 12px;
  background: #0d1117;
  border-radius: 6px;
  font-family: 'Courier New', monospace;
  font-size: 12px;
  color: #e6edf3;
  border: 1px solid #30363d;
  word-break: break-all;
  line-height: 1.6;
}

/* ===== ERRORS ===== */
.error-section {
  border-left: 2px solid #f85149;
}

.error-item {
  display: flex;
  align-items: center;
  gap: 6px;
  padding: 5px 10px;
  background: #0d1117;
  border-radius: 6px;
  margin-bottom: 4px;
  font-size: 12px;
  flex-wrap: wrap;
  border-left: 2px solid #f85149;
}

.error-item:last-child {
  margin-bottom: 0;
}

.error-type {
  padding: 1px 8px;
  background: #f85149;
  color: white;
  border-radius: 4px;
  font-size: 9px;
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

/* ===== DATA ===== */
.data-display {
  padding: 8px 12px;
  background: #0d1117;
  border-radius: 6px;
  font-family: 'Courier New', monospace;
  font-size: 11px;
  color: #aed581;
  border: 1px solid #30363d;
  overflow-x: auto;
  margin: 0;
  max-height: 130px;
  overflow-y: auto;
}

/* ===== TOKENS TABLE ===== */
.tokens-section {
  border-top: 2px solid #58a6ff;
}

.tokens-table {
  background: #0d1117;
  border-radius: 8px;
  border: 1px solid #30363d;
  overflow: hidden;
}

.tokens-header {
  display: grid;
  grid-template-columns: 35px 85px 1fr 75px;
  background: #161b22;
  color: #8b949e;
  font-size: 9px;
  font-weight: 600;
  text-transform: uppercase;
  letter-spacing: 0.5px;
  padding: 6px 10px;
  border-bottom: 1px solid #30363d;
}

.tokens-row {
  display: grid;
  grid-template-columns: 35px 85px 1fr 75px;
  padding: 4px 10px;
  font-size: 11px;
  border-bottom: 1px solid #21262d;
  transition: background 0.15s ease;
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
  font-size: 10px;
  justify-content: center;
}

.token-col-type {
  justify-content: flex-start;
}

.token-col-value {
  justify-content: flex-start;
  font-family: 'Courier New', monospace;
}

.token-col-position {
  justify-content: center;
  font-size: 10px;
  color: #8b949e;
}

.token-type-badge {
  padding: 1px 8px;
  border-radius: 10px;
  font-size: 8px;
  font-weight: 600;
  text-transform: uppercase;
}

.type-command {
  background: #58a6ff;
  color: #0d1117;
}

.type-parameter {
  background: #58a6ff;
  color: #0d1117;
}

.type-value {
  background: #3fb950;
  color: #0d1117;
}

.type-number {
  background: #58a6ff;
  color: #0d1117;
}

.type-string {
  background: #d29922;
  color: #0d1117;
}

.type-equals {
  background: #8b949e;
  color: #0d1117;
}

.type-flag {
  background: #bc8cff;
  color: #0d1117;
}

.type-identifier {
  background: #8b949e;
  color: #0d1117;
}

.token-value-display {
  font-family: 'Courier New', monospace;
  font-size: 11px;
  color: #e6edf3;
  word-break: break-all;
}

.token-position {
  font-size: 9px;
  color: #8b949e;
  font-family: monospace;
}

/* ===== SUMMARY ===== */
.summary {
  display: grid;
  grid-template-columns: repeat(3, 1fr);
  gap: 8px;
  margin-top: 2px;
}

.summary-item {
  background: #161b22;
  padding: 8px 10px;
  border-radius: 8px;
  text-align: center;
  border: 1px solid #30363d;
}

.summary-label {
  display: block;
  font-size: 9px;
  color: #8b949e;
  text-transform: uppercase;
  letter-spacing: 0.3px;
  font-weight: 600;
}

.summary-value {
  display: block;
  font-size: 18px;
  font-weight: 700;
  margin-top: 2px;
  color: #e6edf3;
}

.success-text {
  color: #3fb950;
}

.error-text {
  color: #f85149;
}

/* ===== BATCH RESULTS ===== */
.batch-results {
  display: flex;
  flex-direction: column;
  gap: 10px;
  overflow-y: auto;
  max-height: 540px;
  padding-right: 4px;
}

.batch-summary {
  background: #161b22;
  border-radius: 8px;
  padding: 14px 16px;
  border: 1px solid #30363d;
  border-top: 3px solid #58a6ff;
}

.batch-summary-header {
  display: flex;
  align-items: center;
  gap: 8px;
  margin-bottom: 10px;
}

.batch-icon {
  font-size: 16px;
  color: #8b949e;
}

.batch-summary-header h4 {
  margin: 0;
  font-size: 13px;
  font-weight: 600;
  color: #e6edf3;
}

.batch-total {
  margin-left: auto;
  font-size: 11px;
  color: #8b949e;
  background: #0d1117;
  padding: 2px 12px;
  border-radius: 12px;
  border: 1px solid #30363d;
}

.batch-stats {
  display: flex;
  gap: 10px;
}

.batch-stat {
  flex: 1;
  text-align: center;
  padding: 6px 8px;
  border-radius: 6px;
  background: #0d1117;
  border: 1px solid #30363d;
}

.stat-number {
  display: block;
  font-size: 20px;
  font-weight: 700;
}

.stat-label {
  font-size: 9px;
  color: #8b949e;
  text-transform: uppercase;
  letter-spacing: 0.3px;
}

.success-stat .stat-number {
  color: #3fb950;
}

.error-stat .stat-number {
  color: #f85149;
}

.total-stat .stat-number {
  color: #58a6ff;
}

.batch-results-list {
  display: flex;
  flex-direction: column;
  gap: 6px;
}

.batch-result-item {
  padding: 8px 12px;
  border-radius: 6px;
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
  gap: 8px;
  flex-wrap: wrap;
}

.batch-result-index {
  font-size: 10px;
  font-weight: 700;
  color: #8b949e;
  min-width: 30px;
}

.batch-result-status {
  font-size: 13px;
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
  color: #8b949e;
  margin-top: 2px;
  padding-left: 38px;
}

.batch-result-item.batch-success .batch-result-message {
  color: #3fb950;
}

.batch-result-item.batch-error .batch-result-message {
  color: #f85149;
}

.batch-result-errors {
  padding-left: 38px;
  margin-top: 2px;
  font-size: 10px;
  color: #f85149;
  display: flex;
  flex-direction: column;
  gap: 1px;
}

.batch-error-detail {
  font-size: 10px;
  color: #f85149;
}

/* ===== RESPONSIVE ===== */
@media (max-width: 768px) {
  .results, .batch-results {
    max-height: 380px;
  }
  .summary-value {
    font-size: 16px;
  }
  .tokens-header,
  .tokens-row {
    grid-template-columns: 28px 65px 1fr 60px;
    font-size: 9px;
    padding: 4px 8px;
  }
  .token-col-index {
    font-size: 8px;
  }
  .token-value-display {
    font-size: 10px;
  }
  .token-type-badge {
    font-size: 7px;
    padding: 1px 6px;
  }
  .batch-stats {
    flex-direction: row;
  }
  .header-actions {
    flex-wrap: wrap;
    gap: 6px;
  }
}

@media (max-width: 480px) {
  .output-header {
    flex-direction: column;
    align-items: stretch;
    gap: 6px;
  }
  .output-header h3 {
    font-size: 13px;
  }
  .badge {
    font-size: 10px;
    padding: 1px 8px;
  }
  .btn-export {
    font-size: 10px;
    padding: 3px 10px;
  }
  .command-display {
    font-size: 11px;
    padding: 6px 10px;
  }
  .error-item {
    font-size: 11px;
  }
  .summary-value {
    font-size: 14px;
  }
  .results, .batch-results {
    max-height: 320px;
  }
  .tokens-header,
  .tokens-row {
    grid-template-columns: 22px 55px 1fr 50px;
    font-size: 8px;
    padding: 3px 6px;
    gap: 2px;
  }
  .token-col-index {
    font-size: 7px;
  }
  .token-value-display {
    font-size: 9px;
  }
  .token-type-badge {
    font-size: 6px;
    padding: 1px 4px;
  }
  .token-col-position {
    font-size: 7px;
  }
  .summary {
    grid-template-columns: repeat(3, 1fr);
    gap: 4px;
  }
  .summary-value {
    font-size: 14px;
  }
  .batch-summary-header h4 {
    font-size: 12px;
  }
  .batch-stats {
    flex-direction: column;
    gap: 4px;
  }
  .batch-result-command {
    font-size: 10px;
  }
  .batch-result-message {
    font-size: 10px;
    padding-left: 0;
  }
  .batch-result-errors {
    padding-left: 0;
  }
}
</style>