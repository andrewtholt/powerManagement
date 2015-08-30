
drop table if exists ModBus_Tag;
drop table if exists ModBus_Template;
drop table if exists ModBus;

create table ModBus_Tag (
    idx integer not null primary key,
    name varchar(32) not null,
    address integer not null,
    length integer not null default 1 CHECK(length >0 and length <= 4),
    type integer not null,
    value integer CHECK(value >= 0 AND value <= 65535), 
    reg_swap boolean,
    enabled boolean,
    timestamp datetime,
    ttl integer
);

create table ModBus_Template (
    idx integer not null primary key,
    make varchar not null,
    model varchar not null,

    address integer not null,
    length integer not null default 1 CHECK(length >0 and length <= 4),
    type integer not null,
    reg_swap boolean default 0,
    enabled boolean default 0
);

create table ModBus (
    idx integer not null primary key,
    name varchar(32) not null,
    make varchar not null,
    model varchar not null,

    type varchar not null check(type = "RTU" or type = "TCP"),
    gateway varcher check( (type = "TCP" and gateway = NULL) or (type = "RTU" and gateway <> NULL)),
    --
    -- Serial settings
    --
    tty varchar check ( type = "TCP" and tty = NULL),
    baud integer check ( 
        (type = "TCP" and baud = NULL) or 
        ( type = "RTU" and baud in (9600,19200,38400,57600,115200))),

    length integer check ( (type = "TCP" and length = NULL) or (
        type = "RTU" and 
        (length in( 7,8 )))),

    parity varchar check ( (type = "TCP" and length = NULL) or (
            type = "RTU" and 
            (parity in( "odd","even" )))),

    stop integer check (  (type = "TCP" and stop = NULL) or 
        ( type = "RTU" and (stop in (1,2 )))),
    --
    -- Network settings
    --
    IP varchar check  ( type = "RTU" and IP = NULL ),
    netmask varchar check  ( type = "RTU" and IP = NULL ),
    port integer default 501 check ( ( type = "RTU" and port = NULL ) or (
            type = "TCP" and (port >= 0 and port <= 65535)
    )),

    enabled boolean default 0
);



