#pragma once

// Portal başlat (eski setup fonksiyonunun içeriği)
void portal_begin();

// Portal döngüsü (eski loop fonksiyonunun içeriği)
void portal_loop_once();
void portal_set_ssid(const char* ssid);
void portal_set_pass(const char* pass);        // pass == NULL -> şifresiz AP
void portal_set_html(const String& html);
