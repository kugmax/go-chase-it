#include "ros/ros.h"
#include "ball_chaser/DriveToTarget.h"
#include <sensor_msgs/Image.h>

// Define a global client that can request services
ros::ServiceClient client;

// This function calls the command_robot service to drive the robot in the specified direction
void drive_robot(float lin_x, float ang_z)
{
    // : Request a service and pass the velocities to it to drive the robot
    ROS_INFO_STREAM("Drive to a target");

    ball_chaser::DriveToTarget srv;
    srv.request.linear_x = lin_x;
    srv.request.angular_z = ang_z;

    if (!client.call(srv))
        ROS_ERROR("Failed to call service drive to target");
}

// This callback function continuously executes and reads the image data
void process_image_callback(const sensor_msgs::Image img)
{

    int white_pixel = 255;

    // : Loop through each pixel in the image and check if there's a bright white one
    // Then, identify if this pixel falls in the left, mid, or right side of the image
    // Depending on the white ball position, call the drive_bot function and pass velocities to it
    // Request a stop when there's no white ball seen by the camera

    ROS_INFO_STREAM("process_image_c 1" );
    int end_left_zone = img.width / 3;
    ROS_INFO_STREAM("process_image_c 2" );
    int start_right_zone = end_left_zone * 2;

    ROS_INFO_STREAM("process_image_c 3" );

    ROS_INFO_STREAM("img.width " +std::to_string(img.width) );
    ROS_INFO_STREAM("img.height " +std::to_string(img.height) );
    ROS_INFO_STREAM("end_left_zone " + std::to_string(end_left_zone) );
    ROS_INFO_STREAM("start_right_zone " + std::to_string(start_right_zone));

    float speed = 0.5; 

    for (int i=1; i<img.height; i++ )
    {
	for(int j=1; j<img.width; j++)
	{
	   int pixel_pos = i * j - 1; 
	  //TODO: here bug need pixel_pos devide on height ??
	   if (img.data[pixel_pos] == white_pixel )
	   {
		if (pixel_pos < end_left_zone)
		{
		   drive_robot(0.0, speed);
		} 
		else if (pixel_pos > start_right_zone) 
		{
		   drive_robot(0.0, -speed);
		} 
		else 
		{
		   drive_robot(speed, 0.0);
		}
		

		return;
	   }
	}
    }

    drive_robot(0.0, 0.0);

}

int main(int argc, char** argv)
{
    // Initialize the process_image node and create a handle to it
    ros::init(argc, argv, "process_image");
    ros::NodeHandle n;

    // Define a client service capable of requesting services from command_robot
    client = n.serviceClient<ball_chaser::DriveToTarget>("/ball_chaser/command_robot");

    // Subscribe to /camera/rgb/image_raw topic to read the image data inside the process_image_callback function
    ros::Subscriber sub1 = n.subscribe("/camera/rgb/image_raw", 10, process_image_callback);

    // Handle ROS communication events
    ros::spin();

    return 0;
}
