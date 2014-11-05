/* 
	this method tries to receive all the data given (*num_bytes)
	upon return, *num_bytes will contain the actual number of bytes sent
	if the method failed, a positive value will be returned, on success 0 will be returned.
	note that method might fail if connection on the other end was closed, in this case
	*connection_closed will contain 1 (otherwise, 0)
*/

int recv_all(int sockfd, const void* buffer, int* num_bytes, int* connection_closed);

/* 
	method tries to send num_bytes from buffer, via sockfd
   returns 0 on success, or 1 if could not send all the data
   upon return, *num_bytes will hold the actual number of bytes that were sent
   if the other end was closed (EPIPE error), *connection_closed will contain 1 (otherwise 0)
   NOTE: send is called with MSG_NOSIGNAL, meaning, no signal will be recieved by the process
   use *connection_closed to determine that the connection was closed by other end 
   
   
*/
int send_all(int sockfd, const void * buffer, int* num_bytes, int* connection_closed);