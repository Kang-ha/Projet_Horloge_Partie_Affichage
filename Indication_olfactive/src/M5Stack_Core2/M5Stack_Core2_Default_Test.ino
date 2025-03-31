#include <M5Core2.h>

void testDefault() {
  // Met à jour l'état interne (boutons, etc.)
  M5.update();

  // Lecture de la tension de la batterie via la puce AXP192
  float batteryVoltage = M5.Axp.GetBatVoltage();

  // Conversion tension -> pourcentage (approximation LiPo)
  float batteryPercent = (batteryVoltage - 3.0) / (4.2 - 3.0) * 100.0;

  // On borne entre 0% et 100%
  if (batteryPercent > 100.0) batteryPercent = 100.0;
  if (batteryPercent < 0.0)   batteryPercent = 0.0;

  // Effacer la zone avant de réafficher
  M5.Lcd.fillRect(20, 60, 200, 20, BLACK);
  M5.Lcd.setCursor(20, 60);
  M5.Lcd.printf("Battery: %.1f%%", batteryPercent);

  // Détecter les pressions sur les boutons A, B et C
  if (M5.BtnA.wasPressed()) {
    Serial.println("Bouton A pressé");
    M5.Lcd.setCursor(20, 100);
    M5.Lcd.println("Btn A pressed");
  }
  if (M5.BtnB.wasPressed()) {
    Serial.println("Bouton B pressé");
    M5.Lcd.setCursor(20, 120);
    M5.Lcd.println("Btn B pressed");
  }
  if (M5.BtnC.wasPressed()) {
    Serial.println("Bouton C pressé");
    M5.Lcd.setCursor(20, 140);
    M5.Lcd.println("Btn C pressed");
  }

  // Attendre 1 seconde avant la prochaine mise à jour
  delay(1000);
}