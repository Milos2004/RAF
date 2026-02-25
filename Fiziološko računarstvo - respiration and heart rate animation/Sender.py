import time
import numpy as np
import neurokit2 as nk
from pylsl import StreamInfo, StreamOutlet
import random


def main():
    # ==============================
    # LSL OUTLETS
    # ==============================
    info_hr = StreamInfo('HR', 'heart_rate', 1, 1, 'float32', 'myuid_hr')
    info_resp = StreamInfo('RESP', 'respiration_signal', 1, 1, 'float32', 'myuid_resp')


    outlet_hr = StreamOutlet(info_hr)
    outlet_resp = StreamOutlet(info_resp)


    # Čuvanje prethodnih vrednosti za HRV/BRV
    hr_signal = []
    br_signal = []


    print("Simulacija počinje...")


    # ==============================
    # Glavna petlja
    # ==============================
    while True:
        # 1. Simulacija EKG signala (5 sekundi, sampling_rate=250 Hz)
        ecg = nk.ecg_simulate(duration=5, sampling_rate=250)
        ecg_cleaned = nk.ecg_clean(ecg, sampling_rate=250)


        # 2. Detekcija R-talasa
        peaks = nk.ecg_findpeaks(ecg_cleaned, sampling_rate=250)
        rpeaks = peaks["ECG_R_Peaks"]


        # 3. Računanje trenutnog HR (u bpm)
        if len(rpeaks) > 1:
            rr_intervals = np.diff(rpeaks) / 250  # u sekundama
            hr_value = round(60 / np.mean(rr_intervals), 1)
        else:
            hr_value = 70  # default ako nema dovoljno R-talasa


        # 4. HRV (SDNN)
        if len(rpeaks) > 2:
            hrv_value = round(np.std(rr_intervals), 2)
        else:
            hrv_value = 0.0


        # 5. Simulacija disanja (sinusoidni signal, BR između 12-20 udisaja/min)
        br_value = round(random.uniform(12, 20), 1)
        br_signal.append(br_value)
        if len(br_signal) > 10:
            br_signal.pop(0)
        brv_value = round(np.std(br_signal), 2)


        # 6. Dodavanje trenutne HR vrednosti u listu
        hr_signal.append(hr_value)
        if len(hr_signal) > 10:
            hr_signal.pop(0)


        # 7. Slanje preko LSL
        outlet_hr.push_sample([hr_value])
        outlet_resp.push_sample([br_value])


        # 8. Ispis
        print(f"Vrednosti (Saljem samo HR i BR) -> HR: {hr_value} bpm, HRV: {hrv_value}, Respiration: {br_value} BR, BRV: {brv_value}")


        # Pauza pre sledećeg uzorka
        time.sleep(1)


if __name__ == "__main__":
    main()
