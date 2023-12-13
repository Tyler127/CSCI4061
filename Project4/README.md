# Project Group Number: 3

## This Program Works on CSELabs Machines
- Professor Chandra told us to document this for the grader.

## Group Members: 
- Gianni Guadagno - guada028 
- Jackson Reichow - reich656 
- Tyler Larson - lars6653

## Testing Machine: 
- login03.cselabs.umn.edu

## Changes to Makefile or Existing Files: 
- We created a common.h like lab 12 , that client and the server both have access to make the connection work easier.
- In the current iteration we changed the kill server command from pkill -f ./server to killall -9 server.

## Individual Contributions:
- All: Bug Fix, implemented threads.
- Jackson: Create the client handler clients and help with the implementation of the connection. Fixed directory traversal. Recieve_file in Client.c
- Tyler: Applied the image rotation and bug fixed. Created the temp file for data output. Worked on sending initial packet with complete data.
- Gianni: Complete the implementation of the sockets, so data packets and seperate image data are sent. Freed memory. Completed the README and cleaned up documentation and error checking.

## Assumptions 
- None

## Program Design for package sending and processing:
- Client:
Create socket and connect to server (same as intermediate)
Read image details into a buffer (size, data)
Create a packet with te size and flag based on rotation degree
Serialize the pack to prep for sending
Send the serialzed packet and image data to server through socket.

- Server:
Establish connection with client (same as  intermediate)
Create a client handling thread with pthread_create()
Recieve the packet through the socket
Deserialiize the packet (store the operation, flag and size in the server)
If flag is IMG_OP_EXIT:
     safely close socket connection
    free the packet
    pthread_exit()
Recieve the image data through the socket
Write the recieved image data to a new file
Rotate the image (Copy and paste from PA3)
Create a temp file for output image data
Create a success acknowledgement packet
Send acknowledgement packet to client
Send rotated image to client through socket.

## Difference From Intermediate: 
- We used a temp file/folder for output image data to help with bug fixes, as recommended.
- Sent a seperate packet with the operation, size and flag. Then the image data seperately (not mentioned in intermediate) 

## AI Usage:
- Used to help with dubugging certain errors, helped find a few syntax errors.

