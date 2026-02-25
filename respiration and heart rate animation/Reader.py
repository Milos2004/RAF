import time
import pyxdf
import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation
from pylsl import StreamInfo, StreamOutlet


# ==============================
# Load XDF
# ==============================
XDF_FILE = "klk.xdf"


streams, _ = pyxdf.load_xdf(XDF_FILE)


hr_stream = next(s for s in streams if s["info"]["name"][0] == "HR")
resp_stream = next(s for s in streams if s["info"]["name"][0] == "RESP")


hr_data = hr_stream["time_series"]
hr_ts = hr_stream["time_stamps"]


resp_data = resp_stream["time_series"]
resp_ts = resp_stream["time_stamps"]


# ==============================
# LSL outlets
# ==============================
hr_outlet = StreamOutlet(StreamInfo("HR","heart_rate",1,0,"float32","xdf_hr"))
resp_outlet = StreamOutlet(StreamInfo("RESP","respiration_signal",1,0,"float32","xdf_resp"))


# ==============================
# Plotting
# ==============================
fig, (ax_hr, ax_resp) = plt.subplots(2,1, figsize=(10,6))
ax_hr.set_title("Heart Rate")
ax_hr.set_ylabel("BPM")
ax_hr.grid(True)


ax_resp.set_title("Respiration")
ax_resp.set_ylabel("BR")
ax_resp.grid(True)


hr_line, = ax_hr.plot([],[],color="red")
resp_line, = ax_resp.plot([],[],color="blue")


hr_vals, hr_times = [], []
resp_vals, resp_times = [], []


start_time = time.time()
t0 = min(hr_ts[0], resp_ts[0])
i_hr = 0
i_resp = 0


def update(frame):
    global i_hr, i_resp


    now = time.time() - start_time


    # HR
    if i_hr < len(hr_ts) and hr_ts[i_hr] - t0 <= now:
        hr_vals.append(float(hr_data[i_hr][0]))
        hr_times.append(now)
        hr_outlet.push_sample([hr_vals[-1]])
        i_hr += 1


    # RESP
    if i_resp < len(resp_ts) and resp_ts[i_resp] - t0 <= now:
        resp_vals.append(float(resp_data[i_resp][0]))
        resp_times.append(now)
        resp_outlet.push_sample([resp_vals[-1]])
        i_resp += 1


    # Crtanje samo onoliko podataka koliko imamo
    if hr_vals:
        hr_line.set_data(hr_times, hr_vals)
        ax_hr.set_xlim(min(hr_times), max(hr_times))
        ax_hr.set_ylim(min(hr_vals)-5, max(hr_vals)+5)


    if resp_vals:
        resp_line.set_data(resp_times, resp_vals)
        ax_resp.set_xlim(min(resp_times), max(resp_times))
        ax_resp.set_ylim(min(resp_vals)-2, max(resp_vals)+2)


    return hr_line, resp_line


ani = FuncAnimation(fig, update, interval=10)
plt.tight_layout()
plt.show()
