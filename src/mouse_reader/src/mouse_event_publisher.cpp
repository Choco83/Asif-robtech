// Copyright (c) 2015-2016, The University of Texas at Austin
// All rights reserved.
// 
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
// 
//     * Redistributions of source code must retain the above copyright
//       notice, this list of conditions and the following disclaimer.
// 
//     * Redistributions in binary form must reproduce the above copyright
//       notice, this list of conditions and the following disclaimer in the
//       documentation and/or other materials provided with the distribution.
// 
//     * Neither the name of the copyright holder nor the names of its
//       contributors may be used to endorse or promote products derived from
//       this software without specific prior written permission.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER BE LIABLE FOR ANY
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

/** @file keyboard_event_publisher.cpp
 * 
 *  'rosrun keyboard_reader keyboard_event_publisher'
 *  
 *  OR
 * 
 *  'rosrun keyboard_reader keyboard_event_publisher _path:=[user_specified_keyboard_path]'
 * 
 *  @author karl.kruusamae(at)utexas.edu
 */

#include "ros/ros.h"
#include "mouse_reader/mouse_reader.h"
#include "mouse_reader/Key.h"

/** Main function and a ROS publisher */
int main(int argc, char *argv[]) {
  
  // ROS init
  ros::init(argc, argv, "mouse_event_publisher");
  // Use async spinner
  ros::AsyncSpinner spinner(1);
  spinner.start();

  // ROS node handle
  ros::NodeHandle nh;
  // Private node handle for optional ROS parameter _path
  ros::NodeHandle pnh("~");

  // Getting user-specified path from ROS parameter server
  std::string mouse_path;
  pnh.param<std::string>("path", mouse_path, "");
  
  // Notify user if no mouse was specified
  if (mouse_path.empty())
  {
    ROS_INFO("No mouse specified, let's find one.");
  }

  // Create a mouse object
  Mouse mouse(mouse_path);

  // If failed to open any keyboard input event, print info and exit
  if(!mouse.isReadable())
  {
    ROS_INFO("Unable to locate mouse.");
    ROS_INFO("Try: %s [device]\n", argv[0]);
    return 1;
  }

  // Creates publisher that advertises Key messages on rostopic /mouse
  ros::Publisher pub_mouse = nh.advertise<mouse_reader::Key>("mouse", 100);
  
  // Message for publishing key press events
  mouse_reader::Key key_event;

  // Vector containing event data
  std::vector <uint16_t> event;
  
  while(ros::ok())
  {
    event = mouse.getKeyEvent();				// get key event
//     ROS_INFO("Ready to publish: %d", event[0]);
    
    // Compose a publishable message
    key_event.key_code = event[0];				// event code
    key_event.key_name = mouse.getKeyName(event[0]);	// string corresponding to event code
    key_event.key_pressed = (bool)event[1];			// true when key is pressed, false otherwise
    if (event[0] > 0) pub_mouse.publish(key_event);		// publish a Key msg only if event code is greater than zero
    
  } // end while
  
  // Close keyboard event file
  mouse.closeMouse();

  return 0;
} //end main
