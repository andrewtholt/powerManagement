
drop table if exists io_point;
drop table if exists io_type;
drop table if exists mqtt;
drop table if exists modbus;
drop table if exists snmp;

create table if not exists io_type (
    ioType varchar(32) not null unique
);
--
-- Note these will become the names of tables.
--
insert into io_type (ioType) values ( 'INTERNAL');
insert into io_type (ioType) values ( 'MQTT');
insert into io_type (ioType) values ( 'MODBUS');
insert into io_type (ioType) values ( 'SNMP');

create table if not exists io_point ( name varchar(32) not null unique,
	direction enum('IN','OUT','DISABLED','INTERNAL') not null default 'DISABLED',
    ioType varchar(8) not null ,
    io_idx int
);

create table if not exists  mqtt ( 
    name varchar(32) not null unique,
    topic varchar(128) not null unique,
    on_state varchar(32) not null default 'TRUE',
    off_state varchar(32) not null default 'FALSE'
);

create table if not exists  snmp ( 
    name varchar(32) not null unique,
    oid varchar(255) not null unique,
    ipAddress varchar(16) not null,
    port int not null,
    on_state varchar(32) not null default 'TRUE',
    off_state varchar(32) not null default 'FALSE'
);

create table if not exists modbus ( 
    name varchar(32) not null unique,
    address int not null unique,
    functionCode int not null,
    ipAddress varchar(16) not null,
    port int not null,
    on_state varchar(32) not null default 'TRUE',
    off_state varchar(32) not null default 'FALSE'
);

begin  ;
insert into  io_point (name, direction, ioType ) values
    ("test", "IN", "MQTT") ;

insert into mqtt( name, topic ) values ('test', '/home/fred');
commit ;

-- select io_point.name, mqtt.topic, io_point.direction from io_point, mqtt  where io_point.name = mqtt.name;

