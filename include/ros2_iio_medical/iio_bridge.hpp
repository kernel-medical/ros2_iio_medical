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

#ifndef ROS2_IIO_MEDICAL__IIO_BRIDGE_HPP_
#define ROS2_IIO_MEDICAL__IIO_BRIDGE_HPP_

#include <rclcpp/rclcpp.hpp>
#include <std_msgs/msg/float64_multi_array.hpp>
#include <std_msgs/msg/header.hpp>

#include <string>
#include <vector>
#include <fstream>

namespace ros2_iio_medical
{

class IIOBridgeNode : public rclcpp::Node
{
public:
  explicit IIOBridgeNode(const rclcpp::NodeOptions & options = rclcpp::NodeOptions());

private:
  void timer_callback();
  void read_scale();
  bool read_channel(int channel, double & value);

  std::string device_path_;
  std::string topic_name_;
  int num_channels_;
  double sample_rate_hz_;
  double scale_;

  rclcpp::Publisher<std_msgs::msg::Float64MultiArray>::SharedPtr publisher_;
  rclcpp::TimerBase::SharedPtr timer_;
};

}  // namespace ros2_iio_medical

#endif  // ROS2_IIO_MEDICAL__IIO_BRIDGE_HPP_
