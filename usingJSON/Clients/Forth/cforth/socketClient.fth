\ test-socket.fth
\ Example code for using client sockets
\ Connects to an SSH server (port 22) on localhost (127.0.0.1)
\ and reads/displays the identification message

1 constant SOCK_STREAM  \ from /usr/include/*/bits/socket_type.h
2 constant PF_INET      \ from /usr/include/*/bits/socket.h
0 constant IPPROTO_IP   \ from /usr/include/netinet/in.h

#16 constant /sockaddr
#255 constant /general-buffer

/sockaddr buffer: sockaddr  \ w.pf bew.port ip[4] padding[8]

/general-buffer buffer: output-buffer
/general-buffer buffer: input-buffer

-1 value socket-fd


: open-socket  ( -- )
   IPPROTO_IP SOCK_STREAM PF_INET socket dup ?posix-err to socket-fd
;
: close-socket  ( -- )
   socket-fd 0>=  if
      socket-fd h-close-handle
      -1 to socket-fd
   then
;

: connect-socket  ( 'ip port -- )
   sockaddr /sockaddr '0' fill   ( 'ip port )
   PF_INET sockaddr w!           ( 'ip port )
   sockaddr wa1+ be-w!           ( 'ip )
   sockaddr 2 wa+  4 move        ( )
   /sockaddr sockaddr socket-fd connect  ?posix-err
;

create localhost #127 c, 0 c, 0 c, 1 c,
create rpi3 #192 c, #168 c, #10 c, #124 c,

#22 constant ssh-port
#9191 constant server-port

: probe-ssh  ( -- )
   open-socket
\   localhost ssh-port connect-socket
   rpi3 ssh-port connect-socket
   pad $100 socket-fd h-read-file  ( actual )
   dup ?posix-err                  ( actual )
   pad swap type                   ( )
   close-socket                    ( )
;

: add-cr ( addr len -- addr len+1 )
    output-buffer /general-buffer erase
    tuck tuck

    output-buffer swap move
    output-buffer + $0a swap c!
    1+
    output-buffer swap
;

: cmd-get ( addr len -- )
    socket-fd 0> if
        s" GET %s\n" sprintf socket-fd h-write-file drop
        input-buffer /general-buffer erase
 
        input-buffer /general-buffer socket-fd h-read-file  ( actual )
        dup ?posix-err                  ( actual )
        drop
    then
;

: cmd-set ( value len name len -- )
    socket-fd 0> if
        s" SET %s %s\n" sprintf socket-fd h-write-file drop
        input-buffer /general-buffer erase
 
        input-buffer /general-buffer socket-fd h-read-file  ( actual )
        dup ?posix-err                  ( actual )
        drop
    then
;

: cmd-close
    socket-fd 0> if
        s" CLOSE" s" %s\n" sprintf socket-fd h-write-file drop

        input-buffer /general-buffer socket-fd h-read-file  ( actual )
        dup ?posix-err                  ( actual )
        drop
        close-socket                    ( )
    then
;

: to-boolean ( -- t|f )
    input-buffer 2 s" ON" compare 0= if
        true
    else
        false
    then
;

: my-server
    open-socket
 
    rpi3 server-port connect-socket

    s" FANS" cmd-get
 
    to-boolean if
        ." It's ON" cr 
    else 
        ." It's OFF" cr 
    then
\    close-socket                    ( )
;


\  Usage example:
\ 
\ $ cd build/bluez64
\ -- Put the file test-socket.fth in this directory --
\ $ make
\ <compilation messages elided>
\ $ ./forth test-socket.fth -
\ ok probe-ssh
\ SSH-2.0-OpenSSH_6.6.1p1 Ubuntu-2ubuntu2.7
\ 

