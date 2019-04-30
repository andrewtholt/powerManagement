drop database if exists automation;
create database automation ;
use  automation;

-- SELECT User,Host FROM mysql.user where User='automation';
-- create user 'automation'@'%' identified by 'automation';
-- grant all on `automation`.* TO 'automation'@'%' identified by 'automation';


drop table if exists io_point;
drop table if exists io_type;
drop table if exists mqtt;
drop table if exists modbus;
drop table if exists snmp;
drop table if exists internal;

drop view if exists mqttQuery ;
-- 
CREATE TABLE IF NOT EXISTS io_type (
    ioType VARCHAR(32) NOT NULL UNIQUE
);

--
-- Note these will become the names of tables.
--
insert into io_type (ioType) values ( 'INTERNAL');
insert into io_type (ioType) values ( 'MQTT');
insert into io_type (ioType) values ( 'MODBUS');
insert into io_type (ioType) values ( 'SNMP');

CREATE TABLE IF NOT EXISTS io_point (
    name VARCHAR(32) NOT NULL UNIQUE,
    direction ENUM('IN', 'OUT', 'DISABLED', 'INTERNAL') NOT NULL DEFAULT 'DISABLED',
    io_idx INT
);


CREATE TABLE IF NOT EXISTS internal (
    idx INT NOT NULL AUTO_INCREMENT,
    name VARCHAR(32) NOT NULL UNIQUE,
    on_state VARCHAR(32) NOT NULL DEFAULT 'ON',
    off_state VARCHAR(32) NOT NULL DEFAULT 'OFF',
    state VARCHAR(32) NOT NULL DEFAULT 'OFF',
    PRIMARY KEY (idx)
    );

CREATE TABLE IF NOT EXISTS mqtt (
    idx INT NOT NULL AUTO_INCREMENT,
    name VARCHAR(32) NOT NULL UNIQUE,
    topic VARCHAR(32) NOT NULL UNIQUE,
    on_state VARCHAR(32) NOT NULL DEFAULT 'ON',
    off_state VARCHAR(32) NOT NULL DEFAULT 'OFF',
    state VARCHAR(32) NOT NULL DEFAULT 'OFF',
    PRIMARY KEY (idx)
);

create table if not exists  snmp ( 
    idx INT NOT NULL AUTO_INCREMENT,
    name varchar(32) not null unique,
    oid varchar(255) not null unique,
    ipAddress varchar(16) not null,
    port int not null,
    on_state varchar(32) not null default 'ON',
    off_state varchar(32) not null default 'OFF',
    state VARCHAR(32) NOT NULL DEFAULT 'OFF',
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
    state VARCHAR(32) NOT NULL DEFAULT 'OFF',
    PRIMARY KEY (idx)
);

CREATE VIEW mqttQuery AS
    SELECT 
        io_point.name, mqtt.topic,mqtt.state, io_point.direction
    FROM
        io_point,mqtt
	WHERE io_point.name = mqtt.name;


