/**
 * ======================================
 *    M5Stack_Core2_Default_Test.ino
 * ======================================
 * 
 * Ce fichier contient un test simple pour :
 * - afficher le niveau de batterie sur l'écran
 * - détecter les pressions des boutons A, B et C
 * 
 * Le test met à jour les données toutes les secondes.
 */

#include <M5Core2.h>  // Librairie principale du M5Stack Core2

// ===============================
// Fonction de test par défaut
// ===============================
void testDefault() {
  // 🔄 Met à jour les entrées (notamment les boutons)
  M5.update();

  // 🔋 Lecture de la tension de la batterie (volts)
  float batteryVoltage = M5.Axp.GetBatVoltage();

  // 🔄 Conversion en pourcentage (approximation de l’état de charge d’une LiPo)
  float batteryPercent = (batteryVoltage - 3.0) / (4.2 - 3.0) * 100.0;

  // 🔒 Limite la valeur entre 0% et 100%
  if (batteryPercent > 100.0) batteryPercent = 100.0;
  if (batteryPercent < 0.0)   batteryPercent = 0.0;

  // 🖥️ Efface la zone batterie de l'écran avant d'écrire
  M5.Lcd.fillRect(20, 60, 200, 20, BLACK);

  // 🖊️ Affiche le pourcentage de batterie à l'écran
  M5.Lcd.setCursor(20, 60);
  M5.Lcd.printf("Battery: %.1f%%", batteryPercent);

  // 🔘 Si le bouton A est pressé
  if (M5.BtnA.wasPressed()) {
    Serial.println("Bouton A pressé");           // Log série
    M5.Lcd.setCursor(20, 100);                   // Affichage écran
    M5.Lcd.println("Btn A pressed");
  }

  // 🔘 Si le bouton B est pressé
  if (M5.BtnB.wasPressed()) {
    Serial.println("Bouton B pressé");
    M5.Lcd.setCursor(20, 120);
    M5.Lcd.println("Btn B pressed");
  }

  // 🔘 Si le bouton C est pressé
  if (M5.BtnC.wasPressed()) {
    Serial.println("Bouton C pressé");
    M5.Lcd.setCursor(20, 140);
    M5.Lcd.println("Btn C pressed");
  }

  // ⏱️ Attente d’1 seconde avant prochaine mise à jour
  delay(1000);
}