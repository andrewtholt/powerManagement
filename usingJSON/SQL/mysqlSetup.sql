drop database if exists automation;
create database automation ;
use  automation;

-- SELECT User,Host FROM mysql.user where User='automation';
-- create user 'automation'@'%' identified by 'automation';
-- grant all on `automation`.* TO 'automation'@'%' identified by 'automation';


drop table if exists io_point;
-- drop table if exists io_type;
drop table if exists mqtt;
drop table if exists modbus;
drop table if exists snmp;
drop table if exists internal;

drop view if exists mqttQuery ;
drop view if exists internalQuery ;
drop view if exists snmpQuery ;
-- 
-- CREATE TABLE IF NOT EXISTS io_type (
--    ioType VARCHAR(32) NOT NULL UNIQUE
-- );

--
-- Note these will become the names of tables.
--
-- insert into io_type (ioType) values ( 'INTERNAL');
-- insert into io_type (ioType) values ( 'MQTT');
-- insert into io_type (ioType) values ( 'MODBUS');
-- insert into io_type (ioType) values ( 'SNMP');

CREATE TABLE IF NOT EXISTS io_point (
    name VARCHAR(32) NOT NULL UNIQUE,
    direction ENUM('IN', 'OUT', 'DISABLED', 'INTERNAL') NOT NULL DEFAULT 'DISABLED',
    io_type ENUM('INTERNAL', 'MQTT','SNMP') NOT NULL DEFAULT 'MQTT',
    notify VARCHAR(32) NOT NULL DEFAULT 'NO',
    state VARCHAR(32) NOT NULL DEFAULT 'OFF',
    old_state VARCHAR(32) NOT NULL DEFAULT 'UNKNOWN',
    logtime TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
    enabled varchar(8) NOT NULL DEFAULT 'YES',
    io_idx INT
);


CREATE TABLE IF NOT EXISTS internal (
    idx INT NOT NULL AUTO_INCREMENT,
    name VARCHAR(32) NOT NULL UNIQUE,
    on_state VARCHAR(32) NOT NULL DEFAULT 'ON',
    off_state VARCHAR(32) NOT NULL DEFAULT 'OFF',
    data_type ENUM('BOOL', 'STRING') NOT NULL DEFAULT 'BOOL',
    PRIMARY KEY (idx)
    );

CREATE TABLE IF NOT EXISTS mqtt (
    idx INT NOT NULL AUTO_INCREMENT,
    name VARCHAR(32) NOT NULL UNIQUE,
    topic VARCHAR(64) NOT NULL UNIQUE,
    on_state VARCHAR(32) NOT NULL DEFAULT 'ON',
    off_state VARCHAR(32) NOT NULL DEFAULT 'OFF',
    data_type ENUM('BOOL', 'STRING') NOT NULL DEFAULT 'BOOL',
    PRIMARY KEY (idx)
);

create table if not exists snmp ( 
    idx INT NOT NULL AUTO_INCREMENT,
    name varchar(32) not null unique,
    oid varchar(128) not null unique,
    ipAddress varchar(16) not null,
    port int not null default 161,
    ro_community varchar(32) not null default 'public',
    rw_community varchar(32) not null default 'private',
    -- 
    --  Default values for APC MasterSwitch
    -- 
    on_value  int not null default 1,
    off_value int not null default 2,
    -- 
    -- Hopefully invalid 
    -- 
    value int null default 3,
    -- 
    -- 
    -- 
    on_state varchar(32) not null default 'ON',
    off_state varchar(32) not null default 'OFF',
    data_type ENUM('BOOL', 'STRING') NOT NULL DEFAULT 'BOOL',
    PRIMARY KEY (idx)
);

CREATE TABLE IF NOT EXISTS modbus (
    idx INT NOT NULL AUTO_INCREMENT,
    name VARCHAR(32) NOT NULL UNIQUE,
    address INT NOT NULL UNIQUE,
    functionCode INT NOT NULL,
    ipAddress VARCHAR(16) NOT NULL,
    port INT NOT NULL,
    on_state VARCHAR(32) NOT NULL DEFAULT 'ON',
    off_state VARCHAR(32) NOT NULL DEFAULT 'OFF',
    data_type ENUM('BOOL', 'STRING') NOT NULL DEFAULT 'BOOL',
    PRIMARY KEY (idx)
);

CREATE VIEW mqttQuery AS
    SELECT 
        io_point.name,io_point.io_type, mqtt.topic,io_point.state,io_point.old_state,io_point.direction,mqtt.data_type,io_point.logtime, io_point.enabled
    FROM
        io_point,mqtt
	WHERE io_point.name = mqtt.name;

CREATE VIEW internalQuery AS
    SELECT 
        io_point.name,io_point.io_type, io_point.state,io_point.old_state, io_point.direction, internal.data_type, io_point.logtime, io_point.enabled
    FROM
        io_point,internal
	WHERE io_point.name = internal.name;

CREATE VIEW snmpQuery AS
    SELECT 
        io_point.name, io_point.io_type,snmp.ipAddress, snmp.port,
        snmp.oid, value, on_value, off_value, 
        snmp.ro_community, snmp.rw_community,
        io_point.state,io_point.old_state,io_point.direction,snmp.data_type,io_point.logtime,
        io_point.enabled
    FROM
        io_point,snmp
	WHERE io_point.name = snmp.name;

