<template>
  <footer class="app-footer">
    <div class="footer-content">
      <span class="footer-item">
        <span class="footer-label">Curso</span>
        <span class="footer-value">MIA - 2S 2026</span>
      </span>
      <span class="footer-divider">|</span>
      <span class="footer-item">
        <span class="footer-label">Práctica</span>
        <span class="footer-value">Proyecto 1</span>
      </span>
      <span class="footer-divider">|</span>
      <span class="footer-item">
        <span class="footer-label">Carnet</span>
        <span class="footer-value">202200081</span>
      </span>
      <span class="footer-divider">|</span>
      <span class="footer-item">
        <span class="footer-label">Estado</span>
        <span class="footer-value" style="color: #3fb950;">● Funcionando</span>
      </span>
      <span class="footer-spacer"></span>
      <span class="footer-version">v2.0</span>
    </div>
  </footer>
</template>

<script>
export default {
  name: 'AppFooter'
}
</script>

<style scoped>
.app-footer {
  background: #0d1117;
  border-top: 1px solid #30363d;
  padding: 10px 24px;
  margin-top: auto;
}

.footer-content {
  max-width: 1600px;
  margin: 0 auto;
  display: flex;
  align-items: center;
  flex-wrap: wrap;
  gap: 4px 12px;
  font-size: 12px;
  color: #8b949e;
}

.footer-item {
  display: flex;
  align-items: center;
  gap: 4px;
}

.footer-label {
  color: #8b949e;
  font-weight: 300;
}

.footer-value {
  color: #e6edf3;
  font-weight: 500;
}

.footer-divider {
  color: #30363d;
}

.footer-spacer {
  flex: 1;
}

.footer-version {
  color: #30363d;
  font-size: 11px;
  font-weight: 300;
  font-family: monospace;
}

@media (max-width: 600px) {
  .app-footer {
    padding: 8px 16px;
  }
  .footer-content {
    font-size: 10px;
    gap: 2px 8px;
  }
  .footer-divider {
    display: none;
  }
  .footer-item {
    background: #161b22;
    padding: 2px 8px;
    border-radius: 4px;
    border: 1px solid #30363d;
  }
  .footer-spacer {
    display: none;
  }
  .footer-version {
    display: none;
  }
}
</style>