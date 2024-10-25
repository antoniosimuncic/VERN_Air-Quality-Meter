# P0008_VERN_Air-Quality-Meter
Sveučilište VERN'  
Smjer: IoT  
Kolegij: IOT razvojni projekt  
Nositelj kolegija: doc. dr. sc. Petar Kolar  

**PAMETNI MONITOR KVALITETE ZRAKA**  

Student: Antonio Šimunčić  
Zagreb, Svibanj 2024.

# Uvod u projekt Pametnog Monitora Kvalitete Zraka

Zagađenje zraka postaje sve veći ekološki i zdravstveni problem koji utječe na kvalitetu života ljudi širom svijeta. Štetne čestice i plinovi u atmosferi mogu izazvati brojne zdravstvene tegobe, a praćenje kvalitete zraka ključno je za zaštitu zdravlja i okoliša.

Cilj ovog projekta je razviti **Smart Air Quality Monitor**, pametnu meteorološku stanicu koja integrira napredne senzore za mjerenje čestica prašine u zraku, CO2, vlage, tlaka i temperature. Ovaj uređaj bi omogućio praćenje kvalitete zraka u realnom vremenu, pružajući korisnicima važne informacije koje im pomažu da donesu informirane odluke i poduzmu potrebne mjere zaštite.

# 1. tjedan - Arhitektira sustava

Zahtjevi:
- Mjerenje temperature i vlage
- Mjerenje čestica prašine (PM1, PM2.5, PM4, PM10)
- Mjerenje VOC index-a (Volatile Organic Compounds)
- Mjerenje količine CO2 u zraku
- Mjerenje atmosferskog tlaka
- Prikaz podataka prema korisniku
- Bežično povezivanje na mrežu


Nakon definiranja zahtjeva za uređaj, započeo sam s potragom odgovarajućih komponenata na platformama kao što su Mouser, TME i Aliexpress kako bih pronašao one koje najbolje odgovaraju specifikaciji. Cilj je bio odabrati pouzdane komponente koje osiguravaju visoku točnost i dugotrajnost sustava te su kompatibilne s mikrokontrolerom i njegovom programskom podrškom.

Za mikrokontroler sam odabrao ESP32-S3-WROOM-1 (N16R8) zbog pristupačne cijene, dvije procesorske jezgre, velike količina ugrađene flash memorije (16MB) i ugrđenog PSRAM-a (8MB) za frame buffer kod generiranja grafičkog sučelja. Također mikrokontroler modul ima ugrađeni Wi-Fi i Bluetooth RF interface i antenu pa se s time uvelike skraćuje razvoj i smanjuje rizik da bežična komunikacija neće raditi.
