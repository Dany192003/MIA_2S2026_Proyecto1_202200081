import axios from 'axios'

// Usar el proxy de Vite (no el backend directamente)
const API_BASE_URL = '/api'

const api = axios.create({
  baseURL: API_BASE_URL,
  headers: {
    'Content-Type': 'application/json',
  },
  timeout: 10000,
})

// Función para analizar un comando
export const analyzeCommand = async (command) => {
  try {
    const response = await api.post('/analyze', { command })
    return {
      success: true,
      data: response.data,
      error: null
    }
  } catch (error) {
    console.error('Error en analyzeCommand:', error)
    return {
      success: false,
      data: null,
      error: error.message || 'Error de conexión con el servidor'
    }
  }
}

// Función para verificar el estado del backend
export const checkBackendStatus = async () => {
  try {
    const response = await api.get('/health', { timeout: 3000 })
    return {
      online: response.status === 200,
      message: response.data?.status || 'OK'
    }
  } catch (error) {
    return {
      online: false,
      message: error.message || 'Backend no disponible'
    }
  }
}

export default api