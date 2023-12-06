# Project Group Number: 3

## Group Members: 
- Gianni Guadagno - guada028 
- Jackson Reichow - reich656 
- Tyler Larson - lars6653

## Testing Machine: 
- login03.cselabs.umn.edu

## Changes to Makefile or Existing Files: 
- We created a common.h like lab 12 , that client and the server both have access to make the connection work easier.

## Contribution Plan:
- All: README, Bug Fix.
- Jackson: Create the client handler clients and help with the implementation of the connection.
- Tyler: Apply the image rotation and bug fix.
- Gianni: Complete the implementation of the sockets, so data packets are sent.

## Implementation plan for constructing the client handling thread:
- When a connection is accepted (intermediate) we createa a client handling thread with pthread_create().
- The thread will recieve the image data, (size, data and rotation angle)
- Similar to out P3, we will run the requested rotation with IMG_OP_NAK to handle our errors.
- Once the process is complete we send the image back to the client through the socket.
- If the server recieves IMG_OP_EXIT, it closes the connection and frees memory.

## Implementation plan for sending the package according to the protocol:
- First, we traverse the directory and addi packets to the request queue
- We establish a connection per client request and set up a client.  
- If the packet contents is not sent properly (data lost from connection), or a corrupt packet is sent, IMG_OP_NAK is run, and the connection is closed. 
- After image rotation is applied, The server sends the rotated image back in its packet and waits for a IMG_OP_EXIT and safetly closes the connection.

## References

Lines checks to see if the file is regular, taken from Stack Overflow: https://stackoverflow.com/questions/40163270/what-is-s-isreg-and-what-does-it-do#:~:text=S_ISREG()%20is%20a%20macro,stat)%20is%20a%20regular%20file.
