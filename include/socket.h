#ifndef ____SOCKET_H__
#define ____SOCKET_H__


#define SOCKET_NEW      0x00000000
#define SOCK_CONNECTING	0x00000001
#define SOCK_UPLINK		0x00000002
#define SOCK_LISTEN		0x00000004
#define SOCK_CTRL		0x00000008
#define SOCK_WRITE		0x00000010
#define SOCK_READ		0x00000020
#define SOCK_NOREAD		0x00000040
#define SOCK_DEAD		0x00000080


#define SOCK_CLUSTER	0x00000100 //Reserve for when we do clustering


#define socket_is_connecting(s) ((s)->flags & SOCK_CONNECTING)
#define socket_is_read(s)  ((s)->flags & SOCK_READ)
#define socket_is_write(s) ((s)->flags & SOCK_WRITE)
#define socket_is_listen(s) ((s)->flags & SOCK_LISTEN)
#define socket_is_dead(s)  ((s)->flags & SOCK_DEAD)

#define socket_is_fdset(s, set) FD_ISSET ((s)->sd, &(set))
#define socket_fdset(s, set) FD_SET ((s)->sd, &(set))

int max_sockets;


/****************************************************/
/**
 * MessageBuffer - Since we cant always guarantee
 * we get all our info in a single packet, lets 
 * buffer
 */
 

typedef struct
{
	char * message;
	size_t length;
	int    completed;
} MessageBuffer;


/****************************************************/
/**
 * Socket
 */

typedef struct socket_
{
	time_t  connected_at;
	
	int	sd;
	int timeout;
	int flags;
 
	unsigned int buffer_len;
	char	buffer[MAXLEN + 1];	

	char	name[512];

	struct  sockaddr_in	*sa;   

	dlink_list	msg_buffer;

	//Callbacks
	void	(*read_callback)  (struct socket_ *, int);
	void	(*write_callback) (void *);
    void    (*error_callback) (struct socket_ *);

} Socket;


E Socket * socket_new       ();
E int      socket_listen    (Socket *);
E void     socket_dead      (Socket *);
E void     socket_free      (Socket *);
E void     socket_remove    (Socket *);
E int      socket_write     (Socket *, char *, ...);
E void socket_empty_callback(void);

#define socket_read_callback(s, a) (s)->read_callback ? (s)->read_callback((s), a) : socket_empty_callback()



/****************************************************/
/**
 * Socket list functions
 */


#define socket_find socket_find_sd

E int      socket_delfrom_list (Socket *);
E int      socket_addto_list   (Socket *);
E Socket * socket_find_by_sd   (int);
E Socket * socket_find_by_name (char * name);
E void     socket_purge_all    (void);
E void     socket_purge_dead   (void);

dlink_list sockets;

#endif