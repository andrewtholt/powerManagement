
drop database if exists automation;
create database automation ;
use  automation;

-- SELECT User,Host FROM mysql.user where User='automation';
-- 
-- create user if not exists 'automation'@'localhost' identified by 'automation';
-- 
-- grant all on `automation`.* TO 'automation'@'localhost' identified by 'automation';

drop table if exists io_point;
drop table if exists io_type;
drop table if exists mqtt;
drop table if exists modbus;
drop table if exists snmp;
drop table if exists internal;

drop view if exists mqttQuery ;
drop view if exists internalQuery ;
drop view if exists snmpQuery ;


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

create table if not exists io_point ( 
    name varchar(32) not null unique,
	direction enum('IN','OUT','DISABLED','INTERNAL') not null default 'DISABLED',
    ioType varchar(8) not null ,
    io_idx int,
    state BOOLEAN not null default 0,
    old_state BOOLEAN not null default 0,
    logtime TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE     CURRENT_TIMESTAMP
);

CREATE TABLE IF NOT EXISTS internal (
    idx INT NOT NULL AUTO_INCREMENT,
    name VARCHAR(32) NOT NULL UNIQUE,
    on_state VARCHAR(32) NOT NULL DEFAULT 'ON',
    off_state VARCHAR(32) NOT NULL DEFAULT 'OFF',
    data_type ENUM('BOOL', 'STRING') NOT NULL DEFAULT 'BOOL',
    PRIMARY KEY (idx)
    );

CREATE VIEW internalQuery AS
    SELECT
        io_point.name, io_point.state,io_point.old_state, io_point.direction, internal.data_type, io_point.logtime
    FROM
        io_point,internal
    WHERE io_point.name = internal.name;

create table if not exists mqtt ( 
    name varchar(32) not null unique,
    command_topic      varchar(128) not null unique,
    state_topic        varchar(128) not null unique,
    availability_topic char(128) not null unique,

    data_type ENUM('BOOL', 'STRING') NOT NULL DEFAULT 'BOOL',
    payload_on  varchar(32) not null default 'ON',
    payload_off varchar(32) not null default 'OFF'
);

CREATE VIEW mqttQuery AS
    SELECT
        io_point.name, mqtt.command_topic, mqtt.state_topic,
        io_point.state,io_point.old_state,io_point.direction,mqtt.data_type,io_point.logtime
    FROM
        io_point,mqtt
     WHERE io_point.name = mqtt.name; 

create table if not exists snmp ( 
    name varchar(32) not null unique,
    oid varchar(255) not null unique,
    ipAddress varchar(16) not null,
    port int not null,
    on_state varchar(32) not null default 'TRUE',
    off_state varchar(32) not null default 'FALSE',
    ro_community varchar(32) not null default 'public',
    rw_community varchar(32) not null default 'private',
    --
    --  Default values for APC MasterSwitch
    --
    on_value  int not null default 1,
    off_value int not null default 2,
    data_type ENUM('BOOL', 'STRING') NOT NULL DEFAULT 'BOOL',

    value int null default 3
);

CREATE VIEW snmpQuery AS
    SELECT
        io_point.name, snmp.ipAddress, snmp.port,
        snmp.oid, snmp.value, snmp.on_value, snmp.off_value,
        snmp.ro_community, snmp.rw_community,
        io_point.state,io_point.old_state,io_point.direction,snmp.data_type,io_point.logtime
    FROM
        io_point,snmp
    WHERE io_point.name = snmp.name;



create table if not exists modbus ( 
    name varchar(32) not null unique,
    address int not null unique,
    functionCode int not null,
    ipAddress varchar(16) not null,
    port int not null,
    on_state varchar(32) not null default 'TRUE',
    off_state varchar(32) not null default 'FALSE'
);


-- begin  ;
-- insert into  io_point (name, direction, ioType ) values
--     ("test", "IN", "MQTT") ;
-- 
-- insert into mqtt( name, topic ) values ('test', '/home/fred');
-- commit ;

-- select io_point.name, mqtt.topic, io_point.direction from io_point, mqtt  where io_point.name = mqtt.name;

