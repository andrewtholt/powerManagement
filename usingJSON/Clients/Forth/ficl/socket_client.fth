-1 value sid
-1 value out-buffer
-1 value in-buffer

0 value init-run

\ 0 value count

2048 constant O_NONBLOCK \ 04000 in ocatl

0 constant F_DUPFD ( Duplicate file descriptor.  )
1 constant F_GETFD ( Get file descriptor flags.  )
2 constant F_SETFD ( Set file descriptor flags.  )
3 constant F_GETFL ( Get file status flags.  )
4 constant F_SETFL ( Set file status flags.  )

255 constant /buffer

0 constant OFF
-1 constant ON

: init ( addr len port )
    init-run 0= if
        socket-connect abort" connect" to sid

        /buffer allocate abort" allocate" to in-buffer
        /buffer allocate abort" allocate" to out-buffer
    
        in-buffer  /buffer erase
        out-buffer /buffer erase
    
        sid F_GETFL 0 fcntl  \ flags
        O_NONBLOCK or
        >r sid F_SETFL r> fcntl abort" fcntl"

    else
        2drop drop
    then
;

: flush
    begin
        in-buffer 1+ /buffer sid socket-recv 
        0 <
    until
    in-buffer /buffer erase
;
\ 
\ Takes a command as a addr count and sends it,
\ The response is a counted string in the in-buffer.
\ 
\ stack addr len 
: cmd { | size }
    out-buffer /buffer erase
    in-buffer /buffer erase

    dup -rot

    out-buffer swap move
    dup out-buffer + 0x0a swap c!
    1+ out-buffer swap ( addr len )

    sid socket-send abort" socket-send" drop

    begin
        in-buffer 1+ /buffer sid socket-recv 
        dup to size
        0 >
    until

    size 1- in-buffer c!
;

: boolCmd 
    cmd in-buffer count evaluate
;

: main
    s" localhost" 9191 init
\    s" 192.168.10.124" 9191 init

    s" GET SUNRISE" cmd in-buffer count type
;

\ main

: logicToText
    dup
    0= if
        ." FALSE"
    else
        ." TRUE"
    then

    cr
;

: t1
    s" localhost" 9191 init
    begin
\         flush
        s" GET START" boolCmd 
        ." START     :" logicToText
        s" GET FANS" boolCmd
        ." FANS      :" logicToText
        or 
        ." OR        :" logicToText
    
        s" GET STOP" boolCmd
        ." STOP      :" logicToText
        invert and
        ." INVERT AND:" logicToText
    
        if
            s" SET FANS ON" 
            boolCmd drop
            ." FANS ON" cr
        else
            s" SET FANS OFF" 
            boolCmd drop
            ." FANS OFF" cr
        then

        ." ======" cr
    
        100 ms
    again
;

