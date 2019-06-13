use automation;

delete from io_point;
delete from mqtt;


replace into mqtt (name, topic ) values ('START','/test/start');
replace into io_point (name,direction ) values ('START','IN');

replace into mqtt (name, topic ) values ('STOP','/test/stop');
replace into io_point (name,direction ) values ('STOP','IN');

replace into mqtt (name, topic ) values ('MOTOR','/test/motor');
replace into io_point (name,direction ) values ('MOTOR','OUT');

update io_point,mqtt set io_point.io_idx=mqtt.idx where io_point.name = mqtt.name;

replace into internal (name, data_type ) values ('TEST_LOCAL','STRING');
replace into io_point (name,direction,io_type ) values ('TEST_LOCAL','INTERNAL','INTERNAL');
update io_point,internal set io_point.io_idx=internal.idx where io_point.name = internal.name;

replace into mqtt(name, topic, data_type ) values ('SUNRISE','/test/SUNRISE','STRING');
replace into io_point (name,direction,io_type ) values ('SUNRISE','IN','MQTT');
update io_point,mqtt set io_point.io_idx=mqtt.idx where io_point.name = mqtt.name;

