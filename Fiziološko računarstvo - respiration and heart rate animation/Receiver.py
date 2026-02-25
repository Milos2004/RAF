"""Example program to show how to read a multi-channel time series from LSL."""

from pylsl import StreamInlet, resolve_byprop


def main():
    # first resolve an HR stream on the lab network
    print("looking for an HR stream...")
    hr_streams = resolve_byprop("type", "heart_rate")

    # create a new inlet to read from the stream
    hr_inlet = StreamInlet(hr_streams[0])

    #BR stream
    print("looking for an BR stream...")
    br_streams = resolve_byprop("type", "respiration_signal")
    br_inlet = StreamInlet(br_streams[0])

    while True:
        # get a new sample (you can also omit the timestamp part if you're not
        # interested in it)
        hr_sample, timestamp = hr_inlet.pull_sample()
        br_sample, timestamp = br_inlet.pull_sample()
        print(f"Primio sam sledece -> HR: {round(hr_sample[0], 1)}, BR: {round(br_sample[0], 1)}")


if __name__ == "__main__":
    main()