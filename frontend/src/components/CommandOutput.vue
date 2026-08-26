<template>
  <div class="command-output">
    <div class="output-header">
      <div class="header-left">
        <span class="icon"> </span>
        <h3>Resultado del Analisis</h3>
      </div>
      <div class="header-actions">
        <button 
          v-if="hasResults" 
          @click="exportResults" 
          class="btn-export"
          title="Exportar resultados a TXT"
        >
          Resumen detallado
        </button>
        <span class="badge" v-if="result" :class="result.success ? 'success-badge' : 'error-badge'">
          {{ result.success ? 'Valido' : 'Error' }}
        </span>
      </div>
    </div>
    
    <div v-if="!result && batchResults.length === 0" class="empty-state">
      <div class="empty-icon">[ ]</div>
      <p>Esperando comando</p>
      <span>Ingresa un comando en el panel izquierdo</span>
    </div>
    
    <div v-else>
      
      <div v-if="result && !result._batch" class="results">
        
        <div class="status-banner" :class="result.success ? 'success' : 'error'">
          <span class="status-icon">{{ result.success ? 'OK' : '✖' }}</span>
          <span class="status-text">{{ result.message }}</span>
        </div>
        
        <div class="section">
          <div class="section-header">
            <span class="section-icon"> </span>
            <h4>Comando ingresado</h4>
          </div>
          <div class="command-display">{{ result.command }}</div>
        </div>
        
        <div v-if="result.errors && result.errors.length > 0" class="section error-section">
          <div class="section-header">
            <span class="section-icon"> </span>
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
            <span class="section-icon"> </span>
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
            <span class="batch-icon">[L]</span>
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
              <span class="batch-result-status">{{ item.result.success ? 'OK' : '✖' }}</span>
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
  gap: 14px;
  height: 100%;
}

.output-header {
  display: flex;
  justify-content: space-between;
  align-items: center;
  padding-bottom: 10px;
  border-bottom: 1px solid #e8edf2;
}

.header-left {
  display: flex;
  align-items: center;
  gap: 8px;
}

.header-actions {
  display: flex;
  align-items: center;
  gap: 10px;
}

.icon {
  font-size: 18px;
  color: #5b7a9a;
}

.output-header h3 {
  margin: 0;
  font-size: 15px;
  font-weight: 600;
  color: #2c3e50;
  letter-spacing: 0.3px;
}

.btn-export {
  padding: 4px 14px;
  background: #2c3e50;
  color: white;
  border: none;
  border-radius: 8px;
  font-size: 12px;
  font-weight: 500;
  cursor: pointer;
  transition: all 0.3s ease;
}

.btn-export:hover {
  background: #1a2a3a;
  transform: scale(1.02);
}

.badge {
  padding: 2px 12px;
  border-radius: 16px;
  font-size: 11px;
  font-weight: 600;
}

.success-badge {
  background: #e8f5e9;
  color: #2e7d32;
}

.error-badge {
  background: #ffebee;
  color: #c62828;
}

.empty-state {
  flex: 1;
  display: flex;
  flex-direction: column;
  align-items: center;
  justify-content: center;
  color: #a0b4c8;
  text-align: center;
  padding: 30px 20px;
}

.empty-icon {
  font-size: 48px;
  margin-bottom: 12px;
  opacity: 0.4;
  color: #b0c4d8;
}

.empty-state p {
  margin: 0;
  font-size: 16px;
  font-weight: 500;
  color: #5b7a9a;
}

.empty-state span {
  font-size: 12px;
  color: #a0b4c8;
  margin-top: 4px;
}

.results {
  display: flex;
  flex-direction: column;
  gap: 12px;
  overflow-y: auto;
  max-height: 540px;
  padding-right: 4px;
}

.status-banner {
  display: flex;
  align-items: center;
  gap: 10px;
  padding: 10px 14px;
  border-radius: 12px;
  font-weight: 500;
  font-size: 13px;
}

.status-banner.success {
  background: #e8f5e9;
  border-left: 3px solid #2e7d32;
  color: #1b5e20;
}

.status-banner.error {
  background: #ffebee;
  border-left: 3px solid #c62828;
  color: #b71c1c;
}

.status-icon {
  font-size: 16px;
}

.section {
  background: #f7f9fc;
  border-radius: 12px;
  padding: 10px 12px;
  border: 1px solid #e8edf2;
}

.section-header {
  display: flex;
  align-items: center;
  gap: 6px;
  margin-bottom: 8px;
}

.section-icon {
  font-size: 12px;
  color: #5b7a9a;
}

.section-header h4 {
  margin: 0;
  font-size: 11px;
  font-weight: 600;
  color: #5b7a9a;
  text-transform: uppercase;
  letter-spacing: 0.3px;
}

.error-count, .token-count, .data-badge {
  margin-left: auto;
  background: #e8edf2;
  color: #5b7a9a;
  padding: 0 8px;
  border-radius: 12px;
  font-size: 9px;
  font-weight: 600;
}

.error-count {
  background: #ffcdd2;
  color: #c62828;
}

.data-badge {
  background: #e3ecf5;
  color: #2c3e50;
  font-family: monospace;
}

.command-display {
  padding: 8px 12px;
  background: white;
  border-radius: 10px;
  font-family: 'Courier New', monospace;
  font-size: 12px;
  color: #1a2a3a;
  border: 1px solid #e8edf2;
  word-break: break-all;
  line-height: 1.6;
}

.error-section {
  border-left: 2px solid #c62828;
}

.error-item {
  display: flex;
  align-items: center;
  gap: 6px;
  padding: 5px 10px;
  background: white;
  border-radius: 8px;
  margin-bottom: 4px;
  font-size: 12px;
  flex-wrap: wrap;
  border-left: 2px solid #c62828;
}

.error-item:last-child {
  margin-bottom: 0;
}

.error-type {
  padding: 1px 8px;
  background: #c62828;
  color: white;
  border-radius: 6px;
  font-size: 9px;
  font-weight: 600;
  text-transform: uppercase;
}

.error-message {
  color: #b71c1c;
  flex: 1;
}

.error-location {
  color: #90a4ae;
  font-size: 10px;
}

.data-display {
  padding: 8px 12px;
  background: #1a2a3a;
  border-radius: 10px;
  font-family: 'Courier New', monospace;
  font-size: 11px;
  color: #a5d6a7;
  border: 1px solid #2c3e50;
  overflow-x: auto;
  margin: 0;
  max-height: 130px;
  overflow-y: auto;
}

.tokens-section {
  border-top: 2px solid #5b9bd5;
}

.tokens-table {
  background: white;
  border-radius: 10px;
  border: 1px solid #e8edf2;
  overflow: hidden;
}

.tokens-header {
  display: grid;
  grid-template-columns: 40px 90px 1fr 80px;
  background: #2c3e50;
  color: white;
  font-size: 10px;
  font-weight: 600;
  text-transform: uppercase;
  letter-spacing: 0.5px;
  padding: 6px 10px;
}

.tokens-row {
  display: grid;
  grid-template-columns: 40px 90px 1fr 80px;
  padding: 5px 10px;
  font-size: 11px;
  border-bottom: 1px solid #f0f2f5;
  transition: background 0.15s ease;
  align-items: center;
}

.tokens-row:last-child {
  border-bottom: none;
}

.tokens-row:hover {
  background: #f0f4f8;
}

.token-col {
  display: flex;
  align-items: center;
}

.token-col-index {
  font-weight: 700;
  color: #000000;
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
  color: #90a4ae;
}

.token-type-badge {
  padding: 1px 8px;
  border-radius: 12px;
  font-size: 9px;
  font-weight: 600;
  text-transform: uppercase;
}

.type-command {
  background: #2c3e50;
  color: white;
}

.type-parameter {
  background: #5b9bd5;
  color: white;
}

.type-value {
  background: #e8f5e9;
  color: #2e7d32;
}

.type-number {
  background: #e3f2fd;
  color: #0d47a1;
}

.type-string {
  background: #fff3e0;
  color: #e65100;
}

.type-equals {
  background: #f5f5f5;
  color: #616161;
}

.type-flag {
  background: #f3e5f5;
  color: #6a1b9a;
}

.type-identifier {
  background: #e8eaf6;
  color: #283593;
}

.token-value-display {
  font-family: 'Courier New', monospace;
  font-size: 12px;
  color: #1a2a3a;
  word-break: break-all;
}

.token-position {
  font-size: 10px;
  color: #90a4ae;
  font-family: monospace;
}

.summary {
  display: grid;
  grid-template-columns: repeat(3, 1fr);
  gap: 8px;
  margin-top: 2px;
}

.summary-item {
  background: white;
  padding: 8px 10px;
  border-radius: 10px;
  text-align: center;
  border: 1px solid #e8edf2;
}

.summary-label {
  display: block;
  font-size: 9px;
  color: #a0b4c8;
  text-transform: uppercase;
  letter-spacing: 0.3px;
  font-weight: 600;
}

.summary-value {
  display: block;
  font-size: 18px;
  font-weight: 700;
  margin-top: 2px;
}

.success-text {
  color: #2e7d32;
}

.error-text {
  color: #c62828;
}

.batch-results {
  display: flex;
  flex-direction: column;
  gap: 12px;
  overflow-y: auto;
  max-height: 540px;
  padding-right: 4px;
}

.batch-summary {
  background: #f7f9fc;
  border-radius: 12px;
  padding: 14px 16px;
  border: 1px solid #e8edf2;
  border-top: 3px solid #2c3e50;
}

.batch-summary-header {
  display: flex;
  align-items: center;
  gap: 8px;
  margin-bottom: 10px;
}

.batch-icon {
  font-size: 18px;
}

.batch-summary-header h4 {
  margin: 0;
  font-size: 13px;
  font-weight: 600;
  color: #2c3e50;
}

.batch-total {
  margin-left: auto;
  font-size: 11px;
  color: #a0b4c8;
  background: #e8edf2;
  padding: 2px 12px;
  border-radius: 12px;
}

.batch-stats {
  display: flex;
  gap: 12px;
}

.batch-stat {
  flex: 1;
  text-align: center;
  padding: 6px 8px;
  border-radius: 8px;
  background: white;
  border: 1px solid #e8edf2;
}

.stat-number {
  display: block;
  font-size: 22px;
  font-weight: 700;
}

.stat-label {
  font-size: 10px;
  color: #a0b4c8;
  text-transform: uppercase;
  letter-spacing: 0.3px;
}

.success-stat .stat-number {
  color: #2e7d32;
}

.error-stat .stat-number {
  color: #c62828;
}

.total-stat .stat-number {
  color: #2c3e50;
}

.batch-results-list {
  display: flex;
  flex-direction: column;
  gap: 6px;
}

.batch-result-item {
  padding: 8px 12px;
  border-radius: 8px;
  border-left: 3px solid;
  background: white;
  border: 1px solid #e8edf2;
  border-left-width: 3px;
}

.batch-result-item.batch-success {
  border-left-color: #2e7d32;
}

.batch-result-item.batch-error {
  border-left-color: #c62828;
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
  color: #a0b4c8;
  min-width: 30px;
}

.batch-result-status {
  font-size: 14px;
}

.batch-result-command {
  font-family: 'Courier New', monospace;
  font-size: 11px;
  color: #1a2a3a;
  word-break: break-all;
  flex: 1;
}

.batch-result-message {
  font-size: 11px;
  color: #5b7a9a;
  margin-top: 2px;
  padding-left: 38px;
}

.batch-result-item.batch-success .batch-result-message {
  color: #2e7d32;
}

.batch-result-item.batch-error .batch-result-message {
  color: #c62828;
}

.batch-result-errors {
  padding-left: 38px;
  margin-top: 2px;
  font-size: 10px;
  color: #c62828;
  display: flex;
  flex-direction: column;
  gap: 1px;
}

.batch-error-detail {
  font-size: 10px;
  color: #c62828;
}

@media (max-width: 768px) {
  .results, .batch-results {
    max-height: 380px;
  }
  .summary-value {
    font-size: 16px;
  }
  .tokens-header,
  .tokens-row {
    grid-template-columns: 30px 70px 1fr 65px;
    font-size: 10px;
    padding: 4px 8px;
  }
  .token-col-index {
    font-size: 9px;
  }
  .token-value-display {
    font-size: 11px;
  }
  .token-type-badge {
    font-size: 8px;
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
    flex-wrap: wrap;
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
    grid-template-columns: 25px 60px 1fr 55px;
    font-size: 9px;
    padding: 3px 6px;
    gap: 2px;
  }
  .token-col-index {
    font-size: 8px;
  }
  .token-value-display {
    font-size: 10px;
  }
  .token-type-badge {
    font-size: 7px;
    padding: 1px 4px;
  }
  .token-col-position {
    font-size: 8px;
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
    gap: 6px;
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