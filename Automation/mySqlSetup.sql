
drop table if exists io_point;

create table io_point ( name varchar(32) not null unique, topic varchar(128) default 'NONE', direction enum('IN','OUT','DISABLED','INTERNAL') not null default 'DISABLED', state varchar(8) not null default 'UNKNOWN', on_state  varchar(6) default 'ON', off_state varchar(6) default 'OFF');
