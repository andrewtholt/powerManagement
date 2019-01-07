drop table if exists iopoints;
drop table if exists ioMQTT;
drop trigger if exists toMQTT;

create table iopoints (
		idx integer primary key autoincrement,
		short_name varchar(32) not null unique,
		value varchar(32) default 'OFF',
		oldvalue varchar(32) default 'OFF',
		class varchar(32) check( "class" in ('MQTT','HA_REST','LOCAL')),
		direction varchar(32) default 'LOCAL'
		);

create table ioMQTT (
		idx integer primary key autoincrement,
		var_idx integer, 
		topic varchar(64) default 'LOCAL'
		);

create trigger toMQTT after insert on iopoints when new.class='MQTT'
begin
insert into ioMQTT (var_idx) values (new.idx) ;
end;

insert into iopoints (short_name, class) values ( 'TEST','MQTT' );
