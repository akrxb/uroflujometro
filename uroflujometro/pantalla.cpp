#define LGFX_USE_V1
#include <LovyanGFX.hpp>
#include "pantalla.h"
#include "calculos.h"

// ══════════════════════════════════════════════════════
//  CONFIGURACIÓN DE HARDWARE (ESP32-C3 + GC9A01)
// ══════════════════════════════════════════════════════
class LGFX : public lgfx::LGFX_Device {
  lgfx::Panel_GC9A01 _panel;
  lgfx::Bus_SPI      _bus;
public:
  LGFX() {
    {
      auto cfg = _bus.config();
      cfg.spi_host = SPI2_HOST;
      cfg.freq_write = 40000000;
      cfg.pin_sclk = 10; // PIN_SCLK
      cfg.pin_mosi = 20; // PIN_MOSI
      cfg.pin_miso = -1;
      cfg.pin_dc   = 21;  // PIN_DC
      _bus.config(cfg);
      _panel.setBus(&_bus);
    }
    {
      auto cfg = _panel.config();
      cfg.pin_cs = 9;   // PIN_CS
      cfg.panel_width  = 240;
      cfg.panel_height = 240;
      cfg.invert = true;
      _panel.config(cfg);
    }
    setPanel(&_panel);
  }
};

static LGFX lcd;

// ══════════════════════════════════════════════════════
//  PALETA DE COLORES ACTUALIZADA
// ══════════════════════════════════════════════════════
static const uint32_t C_BG        = 0x000000; // Negro (Fondo superior)
static const uint32_t C_WHITE     = 0xFFFFFF; // Blanco
static const uint32_t C_PRIMARY   = 0x0056B3; // Azul Médico
static const uint32_t C_ACCENT    = 0x00D1B2; // Azul clarito (Líquido matraz)
static const uint32_t C_LTBLUE    = 0xF1F4F8; // Fondo inferior
static const uint32_t C_TEXT_DARK = 0x2C3E50; // Texto estados

static char prevChrono[12] = "";
static char prevFlow[8]    = "";
static Estado ultimoEstado = (Estado)-1; 

// ══════════════════════════════════════════════════════
//  DIBUJO DE ELEMENTOS ESTÁTICOS
// ══════════════════════════════════════════════════════
static void drawFlask() {
  const int cx = 47, cy = 128; 
  // Estructura del matraz (Blanco)
  lcd.fillRoundRect(cx - 7, cy - 22, 14, 4, 2, C_WHITE);
  lcd.fillRoundRect(cx - 5, cy - 18, 10, 11, 2, C_WHITE);
  lcd.fillTriangle(cx - 5, cy - 7,  cx + 5,  cy - 7, cx + 14, cy + 16, C_WHITE);
  lcd.fillTriangle(cx - 5, cy - 7,  cx - 14, cy + 16, cx + 14, cy + 16, C_WHITE);
  
  // LÍQUIDO AZUL CLARITO (C_ACCENT)
  lcd.fillTriangle(cx - 10, cy + 9, cx + 10, cy + 9,  cx + 14, cy + 16, C_ACCENT); 
  lcd.fillTriangle(cx - 10, cy + 9, cx - 14, cy + 16, cx + 14, cy + 16, C_ACCENT); 
  
  lcd.fillRect(cx - 14, cy + 16, 28, 1, C_WHITE);
}

void pantalla_init() {
  lcd.init();
  lcd.setRotation(0);
  
  // Fondo superior Negro (C_BG)
  lcd.fillScreen(C_BG); 
  // Fondo inferior Gris azulado
  lcd.fillRect(0, 128, 240, 112, C_LTBLUE); 

  // Banda central (Azul Médico)
  lcd.fillRoundRect(15, 100, 210, 56, 28, C_PRIMARY); 
  lcd.fillCircle(47, 128, 28, 0x004494); // Azul un poco más oscuro para el círculo del matraz
  drawFlask();

  // "UROFLUJOMETRO" en BLANCO
  lcd.setFont(&lgfx::fonts::FreeSans9pt7b);
  lcd.setTextColor(C_WHITE, C_BG);
  lcd.setTextDatum(lgfx::middle_center);
  lcd.drawString("UROFLUJOMETRO", 120, 78);
}

// ══════════════════════════════════════════════════════
//  ACTUALIZACIÓN DINÁMICA
// ══════════════════════════════════════════════════════
void pantalla_actualizar(Estado estado, uint32_t ms, float caudal) {
  
  // 1. CRONÓMETRO (Fondo negro ya asegurado)
  uint32_t ds = ms / 100;
  char bufC[12];
  snprintf(bufC, sizeof(bufC), "%02u:%02u'%u", (ds/600), (ds%600)/10, ds%10);
  
  if (strcmp(bufC, prevChrono) != 0) {
    lcd.setFont(&lgfx::fonts::FreeMonoBold18pt7b);
    lcd.setTextColor(C_WHITE, C_BG);
    lcd.setTextDatum(lgfx::middle_center);
    lcd.drawString(bufC, 120, 52);
    strcpy(prevChrono, bufC);
  }

  // 2. CAUDAL: Borrado mejorado
  char bufF[8];
  if (estado == RECORDING) {
    snprintf(bufF, sizeof(bufF), "%.2f", caudal);
  } else {
    strcpy(bufF, "---"); 
  }

  if (strcmp(bufF, prevFlow) != 0) {
    // AREA DE BORRADO AMPLIADA: Borra casi toda la banda derecha para evitar restos de números grandes
    // Desde el fin del círculo del matraz hasta casi el borde de la banda azul
    lcd.fillRect(75, 110, 135, 36, C_PRIMARY); 
    
    lcd.setFont(&lgfx::fonts::FreeMonoBold18pt7b);
    lcd.setTextColor(C_WHITE, C_PRIMARY);
    lcd.setTextDatum(lgfx::middle_center);
    lcd.drawString(bufF, 148, 128); // Mantiene posición
    strcpy(prevFlow, bufF);
  }

  // 3. ETIQUETA DE ESTADO (Texto +1pt -> Usando FreeSansBold10pt)
  if (estado != ultimoEstado) {
    const char* msg = "";
    switch (estado) {
      case IDLE:      msg = "EN ESPERA";   break;
      case TARING:    msg = "TARANDO...";  break;
      case RECORDING: msg = "REGISTRANDO"; break;
      case FINISHED:  msg = "FINALIZADO";  break;
    }

    lcd.fillRect(0, 195 - 20, 240, 40, C_LTBLUE);
    // Cambiado de 9pt a 10pt para que sea más grande
    lcd.setFont(&lgfx::fonts::FreeSansBold9pt7b); 
    lcd.setTextColor(C_TEXT_DARK, C_LTBLUE);
    lcd.setTextDatum(lgfx::middle_center);
    lcd.drawString(msg, 120, 195);
    
    ultimoEstado = estado;
  }
}