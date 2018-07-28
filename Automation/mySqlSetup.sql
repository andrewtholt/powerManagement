
drop database if exists automation;
create database automation ;
use  automation;

SELECT User,Host FROM mysql.user where User='automation';

create user 'automation'@'localhost' identified by 'automation';
grant all on `automation`.* TO 'automation'@'localhost' identified by 'automation';

drop table if exists io_point;

create table io_point ( name varchar(32) not null unique,
    topic varchar(128) default 'NONE',
    direction enum('IN','OUT','DISABLED','INTERNAL') not null default 'DISABLED',
    state varchar(8) not null default 'OFF',
    on_state  varchar(6) default 'ON',
    off_state varchar(6) default 'OFF'

    ioType enum('MQTT','MODBUS','SNMP') not null default 'MQTT');
);
