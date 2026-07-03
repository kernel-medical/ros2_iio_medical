#!/bin/bash
# Simulate ADS1299 8-channel EEG/ECG IIO sysfs interface
# Creates fake /tmp/iio_sim/iio:device0 matching what the ADS1299 kernel
# driver creates under /sys/bus/iio/devices/iio:device0
#
# Usage: ./simulate_ads1299.sh
# Then run iio_bridge with: --ros-args -p device_path:=/tmp/iio_sim/iio:device0

SIM_DIR="/tmp/iio_sim/iio:device0"
NUM_CHANNELS=8
SCALE="0.000186"   # ADS1299 LSB to mV at gain=1, VREF=4.5V
UPDATE_HZ=250      # simulate 250 Hz sample rate

echo "Creating fake ADS1299 IIO sysfs at $SIM_DIR"
mkdir -p "$SIM_DIR/scan_elements"

# Scale factor (LSB to mV)
echo "$SCALE" > "$SIM_DIR/in_voltage_scale"

# Create scan_elements for each channel
for i in $(seq 0 $((NUM_CHANNELS - 1))); do
    echo "1"          > "$SIM_DIR/scan_elements/in_voltage${i}_en"
    echo "$i"         > "$SIM_DIR/scan_elements/in_voltage${i}_index"
    echo "le:s24/32>>0" > "$SIM_DIR/scan_elements/in_voltage${i}_type"
done

echo "Sysfs created. Generating fake EEG/ECG data at ${UPDATE_HZ} Hz..."
echo "Press Ctrl+C to stop."
echo ""

SLEEP_US=$((1000000 / UPDATE_HZ))
T=0

# Generate synthetic biosignal data:
# Ch 0-3: sinusoidal alpha wave (10 Hz EEG)
# Ch 4-5: heartbeat-like ECG waveform
# Ch 6-7: background noise
while true; do
    for i in $(seq 0 $((NUM_CHANNELS - 1))); do
        # Sine wave with different frequencies per channel
        # Use awk for floating point math
        VAL=$(awk -v t="$T" -v ch="$i" 'BEGIN {
            pi = 3.14159265
            freq = 10 + ch * 3          # 10-31 Hz
            amp  = 100000 * (ch + 1)    # amplitude
            noise = int(rand() * 5000) - 2500
            printf "%d", int(amp * sin(2 * pi * freq * t / 1000)) + noise
        }')
        echo "$VAL" > "$SIM_DIR/in_voltage${i}_raw"
    done

    T=$((T + 1000 / UPDATE_HZ))
    sleep "0.004"   # ~250 Hz
done
