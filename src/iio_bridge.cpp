// Copyright 2026 Md Shofiqul Islam
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

// ROS2 IIO Medical Sensor Bridge
// Reads Linux IIO biosignal devices (ADS1299, MAX86150, ti-ads1298)
// and publishes samples as ROS2 topics for surgical/medical robotics.
//
// Two acquisition modes:
//   1. Polling via sysfs (this file) — simple, low rate, good for prototyping
//   2. Triggered buffer via /dev/iio:deviceX — high speed, production use
//
// Sysfs path layout expected:
//   /sys/bus/iio/devices/iio:deviceN/in_voltageX_raw   — raw ADC count
//   /sys/bus/iio/devices/iio:deviceN/in_voltage_scale  — LSB to mV factor

#include "ros2_iio_medical/iio_bridge.hpp"

#include <chrono>
#include <string>
#include <fstream>
#include <sstream>

using namespace std::chrono_literals;

namespace ros2_iio_medical
{

IIOBridgeNode::IIOBridgeNode(const rclcpp::NodeOptions & options)
: Node("iio_bridge", options), scale_(1.0)
{
  // ── Parameters ────────────────────────────────────────────────────────────
  device_path_ = this->declare_parameter<std::string>(
    "device_path", "/sys/bus/iio/devices/iio:device0");

  num_channels_ = this->declare_parameter<int>("num_channels", 8);

  sample_rate_hz_ = this->declare_parameter<double>("sample_rate_hz", 250.0);

  topic_name_ = this->declare_parameter<std::string>(
    "topic_name", "/biosignal/ecg");

  // ── Publisher ──────────────────────────────────────────────────────────────
  publisher_ = this->create_publisher<std_msgs::msg::Float64MultiArray>(
    topic_name_, 10);

  // ── Timer (polling loop) ───────────────────────────────────────────────────
  auto period_ms = std::chrono::milliseconds(
    static_cast<int>(1000.0 / sample_rate_hz_));

  timer_ = this->create_wall_timer(
    period_ms, std::bind(&IIOBridgeNode::timer_callback, this));

  read_scale();

  RCLCPP_INFO(
    this->get_logger(),
    "IIO bridge started | device: %s | channels: %d | rate: %.1f Hz | topic: %s",
    device_path_.c_str(), num_channels_, sample_rate_hz_, topic_name_.c_str());
}

void IIOBridgeNode::read_scale()
{
  // Try per-channel scale first, then fall back to generic voltage scale
  std::string path = device_path_ + "/in_voltage_scale";
  std::ifstream f(path);
  if (f.is_open()) {
    f >> scale_;
    RCLCPP_INFO(this->get_logger(), "IIO scale: %.6f mV/LSB", scale_);
    return;
  }
  RCLCPP_WARN(this->get_logger(), "Scale not found at %s — using 1.0", path.c_str());
}

bool IIOBridgeNode::read_channel(int channel, double & value)
{
  std::string path = device_path_ + "/in_voltage" +
    std::to_string(channel) + "_raw";

  std::ifstream f(path);
  if (!f.is_open()) {
    return false;
  }

  int raw = 0;
  f >> raw;
  value = static_cast<double>(raw) * scale_;  // convert to mV
  return true;
}

void IIOBridgeNode::timer_callback()
{
  auto msg = std_msgs::msg::Float64MultiArray();

  // Layout: 1D array, each element = one channel in mV
  msg.layout.dim.resize(1);
  msg.layout.dim[0].label  = "channels";
  msg.layout.dim[0].size   = static_cast<uint32_t>(num_channels_);
  msg.layout.dim[0].stride = static_cast<uint32_t>(num_channels_);
  msg.data.resize(static_cast<size_t>(num_channels_));

  for (int i = 0; i < num_channels_; ++i) {
    if (!read_channel(i, msg.data[static_cast<size_t>(i)])) {
      RCLCPP_ERROR_THROTTLE(
        this->get_logger(), *this->get_clock(), 5000,
        "Failed to read channel %d from %s", i, device_path_.c_str());
      return;
    }
  }

  publisher_->publish(msg);
}

}  // namespace ros2_iio_medical

// ── Main ──────────────────────────────────────────────────────────────────────
int main(int argc, char * argv[])
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<ros2_iio_medical::IIOBridgeNode>());
  rclcpp::shutdown();
  return 0;
}
