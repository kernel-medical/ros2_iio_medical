# Production Hardware Guide

## Development vs Production Board

| | Prototyping | Production |
|---|---|---|
| Board | Orange Pi Zero 2W | STM32MP157 / NXP i.MX8M Plus |
| SoC | Allwinner H618 | STM32MP157 / i.MX8M Plus |
| Supply | Consumer, no guarantee | 10-year availability |
| Temp rating | 0–70°C | −40°C to 85°C industrial |
| Real-time | PREEMPT_RT needed | M4 coprocessor (STM32) or i.MX8 |
| Certification support | None | IEC 60601-1, ISO 13485 ready |

---

## Recommended Production SoCs

### Option 1 — STMicro STM32MP157 (Best for signal acquisition)

**Key advantage: dual-core — M4 handles timing, A7 handles Linux.**

```
ADS1299 / MAX30102
       │ SPI / I2C + DMA
       ▼
  ┌─────────────────────────────┐
  │  Cortex-M4 (real-time)      │  FreeRTOS + OpenAMP
  │  Exact SPI timing           │  No OS jitter possible
  │  Hardware DMA to SRAM       │  PREEMPT_RT not needed
  └────────────┬────────────────┘
               │ OpenAMP RPMsg (shared SRAM)
               ▼
  ┌─────────────────────────────┐
  │  Cortex-A7 (Linux)          │  IIO subsystem
  │  remoteproc + rpmsg driver  │  ros2_iio_medical
  │  IIO sysfs interface        │  ROS 2 Humble
  └─────────────────────────────┘
```

**The IIO sysfs interface is identical** — `iio_triggered_bridge` works
**unchanged**. The M4→A7 boundary is invisible to ros2_iio_medical.

| Spec | Value |
|---|---|
| Cortex-A7 | 650 MHz (Linux, ROS 2) |
| Cortex-M4 | 209 MHz (real-time firmware, sensor acquisition) |
| RAM | 512 MB DDR3 |
| Supply | 10-year (STMicro) |
| Temp | −40°C to 85°C |
| Package | TFBGA448 |

**SoM options (production-ready modules):**

| SoM | Maker | Notes |
|---|---|---|
| STM32MP157F-DK2 | STMicro | Dev kit, easy start |
| DHSOM-STM32MP1 | DH Electronics | Industrial SoM, medical track record |
| phyCORE-STM32MP1 | Phytec | Long-term supply, Yocto BSP |
| MSC SM2S-MB-EP5 | MSC Technologies | Extended temp, fanless |

---

### Option 2 — NXP i.MX8M Plus (Best for AI + ROS 2)

**Key advantage: NPU (2 TOPS) for on-device biosignal ML inference.**

| Spec | Value |
|---|---|
| CPU | Quad Cortex-A53 @ 1.8 GHz |
| NPU | 2.3 TOPS (for ML on ECG/EEG) |
| RAM | Up to 8 GB LPDDR4 |
| Supply | 10-year (NXP) |
| Temp | −40°C to 85°C |
| U-Boot | You already have U-Boot patches for i.MX! |

**SoM options:**

| SoM | Maker | Notes |
|---|---|---|
| Verdin iMX8M Plus | Toradex | Best ROS 2 support, Torizon OS |
| DART-MX8M-PLUS | Variscite | Compact, Linux BSP |
| phyCORE-i.MX 8M Plus | Phytec | Medical customer base |
| SOM-MX8MPLUS | Advantech | Medical / industrial |

---

### Option 3 — TI AM62x (Cost-optimized)

| Spec | Value |
|---|---|
| CPU | Quad Cortex-A53 @ 1.4 GHz |
| Supply | 10-year (SITARA) |
| Temp | −40°C to 85°C |
| Price | Lower than i.MX8 |

**SoM options:** Toradex Verdin AM62, BeaglePlay, TI SK-AM62

---

## STM32MP157 — Software Architecture Detail

### M4 Firmware (FreeRTOS + OpenAMP)

The M4 runs a bare-metal firmware that:
1. Initialises SPI/I2C peripheral with DMA
2. Receives DRDY interrupt from ADS1299
3. Reads sample via SPI DMA (zero CPU overhead)
4. Pushes sample to shared SRAM via OpenAMP RPMsg
5. Signals A7 via mailbox interrupt

**Acquisition timing is entirely in hardware** — DRDY → DMA → RPMsg.
No OS scheduler involved. Jitter: < 1 µs.

### A7 Linux Driver (remoteproc + rpmsg_iio)

The A7 side:
1. `remoteproc` loads M4 firmware from rootfs at boot
2. `rpmsg` driver receives samples from M4 via mailbox
3. Exposes data via standard IIO sysfs (`/sys/bus/iio/devices/iio:device0`)
4. `ros2_iio_medical` reads exactly as with any other IIO device

**No changes to ros2_iio_medical** — the IIO abstraction hides M4 completely.

### Yocto Integration

```bash
# Add to your BSP layer:
IMAGE_INSTALL:append = " \
    m4-firmware-ads1299 \
    linux-remoteproc \
"

# meta-st-stm32mp provides:
# - remoteproc kernel support
# - OpenAMP userspace tools
# - M4 firmware loading at boot (systemd service)
```

---

## Comparison for Your Use Case

| Criterion | STM32MP157 | i.MX8M Plus | AM62x |
|---|---|---|---|
| Biosignal timing accuracy | ★★★★★ M4 exact | ★★★★ PREEMPT_RT | ★★★★ PREEMPT_RT |
| ROS 2 performance | ★★★ (A7 650 MHz) | ★★★★★ (A53 quad) | ★★★★ |
| ML inference on biosignals | ★★ (no NPU) | ★★★★★ (NPU 2T) | ★★ |
| Your existing U-Boot knowledge | ★★★ | ★★★★★ i.MX family | ★★★ |
| Cost | ★★★★ | ★★★ | ★★★★★ |
| Medical market adoption | ★★★★★ | ★★★★★ | ★★★ |

**Recommendation:**
- For EEG / high-speed ECG where signal integrity is critical → **STM32MP157**
- For surgical robotics with AI + ROS 2 → **NXP i.MX8M Plus**
- For patient monitoring with cost constraints → **TI AM62x**

---

## Migration from Orange Pi Zero 2W

ros2_iio_medical requires no code changes when migrating boards.
The IIO sysfs interface is board-agnostic.

Steps:
1. Build Yocto BSP for target board (meta-st-stm32mp or meta-imx)
2. Add IIO driver for sensor (or M4 firmware + rpmsg_iio for STM32)
3. Deploy ros2_iio_medical package — same launch files, same parameters
4. Verify with `ros2 topic echo /biosignal/eeg`

See also: [REALTIME.md](REALTIME.md) for PREEMPT_RT setup on i.MX8M Plus / AM62x.
