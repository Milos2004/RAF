Ukoliko se čvor pullSamples crveni, neophodno je dodati dependency odnosno ide se na file -> dependencies -> add existing i pronaci lsl4vvvv fajl. Pull samples preuzima svaku vrednost pojedinačno iz lsl toka.

Respiration: U zavisnosti od faze disanja, srce će se smanjivati ili povećavati. Korišćenjem SineWave i Map čvorova postiže se ovaj efekat. Onda RandomSpread čvor kreira srca koja preko Wanderer i TransformSRT čvorova dobijaju putanju kretanja. 

Heart rate: U zavisnosti od toga koliki je heart rate (hr), tolikom brzinom će se menjati boja srca. Ukoliko dođe do nagle promene hr brzina smenjivanja boja će se povećati ili smanjiti u skladu sa promenom hr. Vreme trajanja jednog ciklusa od tri slike se dobija deljenjem 60 sekundi sa vrednošću hr-a pristiglu sa pullSamples čvora. Onda se izlazna vrednost LFO čvora mapira i konvertuje u int32 tako da obuhvati indekse slika koje pušta ImagePlayer čvor.

Napomena: projekat je rađen uz asistenciju ChatGPT-a i materijala sa vežbi.

