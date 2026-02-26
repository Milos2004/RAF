Svrha ovog repozitorijuma je da se u realnom vremenu prezentuje disanje i puls srca preko animacije.
Za realizaciju su korišćeni sledeći resursi:
- Python skripte
  1. Sender.py - šalje vrednosti Breath rate-a (Respiration) i Heart rate-a na LSL tok
  2. Receiver.py - prima te vrednosti
  3. Reader.py - čita iz .xdf fajla snimljene vrednosti koje je Sender poslao Receiver-u preko LSL-a
- Lab Recorder
  1. Služi za snimanje komunikacije tj. LSL toka između Sender-a i Receiver-a
  2. Dobijeni snimak se čuva u .xdf fajlu
- VVVV gamma
  1. Šema koja je zadužena za izvođenje animacije se sastoji iz dva dela koji su međusobno povezani - respiration i heart rate
  2. U šemi su korišćeni čvorovi iz fabričke biblioteke izuzev pullSamples čvora kojeg je napravila prof. dr Jelena Mladenović
  3. pullSamples čvor zahteva LSL4VVVV
- Folder slike - slike koje se smenjuju u animaciji
- Powershell
  1. Služi za pokretanje Python skripti
  2. Python skripte se pokreću jednostavnom komandom -> python naziv_skripte.py   

Način upotrebe repozitorijuma:
1. Pokrenuti Sender.py i Receiver.py
2. Preko Lab Recorder-a snimiti njihovu komunikaciju (obično do 40-tak vrednosti) i ona će biti sačuvana u fajlu .xdf formata
3. Prva dva koraka se mogu preskočiti jer ovaj repozitorijum sadrži već jedan takav fajl
4. Ukoliko je neophodan novi fajl mora mu se promeniti ime u klk.xdf i on mora biti u istom folderu gde su ostali resursi
5. Otvoriti animation_vvvv.vl fajl
6. Pokrenuti Reader.py

Ukoliko se čvor pullSamples crveni, neophodno je dodati dependency odnosno ide se na File -> Dependencies -> Add existing i pronaći LSL4VVVV fajl. Funkcija tog čvora je da čita vrednosti iz .xdf fajla koji predstavlja snimljen LSL tok.

Ako Renderer ne prikazuje prozor, potrebno ga je izbrisati, zatim dvoklikom otvoriti biblioteku i uzeti novi.

Respiration: U zavisnosti od faze disanja, srce će se smanjivati ili povećavati. Korišćenjem SineWave i Map čvorova postiže se ovaj efekat. Onda RandomSpread čvor kreira srca koja preko Wanderer i TransformSRT čvorova dobijaju putanju kretanja. 

Heart rate: U zavisnosti od toga koliki je heart rate (hr), tolikom brzinom će se menjati boja srca. Ukoliko dođe do nagle promene hr brzina smenjivanja boja će se povećati ili smanjiti u skladu sa promenom hr. Vreme trajanja jednog ciklusa od tri slike se dobija deljenjem 60 sekundi sa vrednošću hr-a pristiglu sa pullSamples čvora. Onda se izlazna vrednost LFO čvora mapira i konvertuje u int32 tako da obuhvati indekse slika koje pušta ImagePlayer čvor.

Napomene: 
- Projekat je rađen uz asistenciju ChatGPT-a i materijala sa vežbi, konkretno onih na kojima je obrađivan LSL način komunikacije.
- Projekat je bio zadatak za drugi kolokvijum iz predmeta Fiziološko računarstvo koji drži prof. dr Jelena Mladenović na Računarskom fakultetu. 


https://github.com/user-attachments/assets/c7374ef2-94c7-48cb-9728-b1fc85996794

