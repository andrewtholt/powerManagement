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
replace into io_point (name,direction,io_type,state ) values ('SUNRISE','IN','MQTT','08:00');
update io_point,mqtt set io_point.io_idx=mqtt.idx where io_point.name = mqtt.name;


replace into mqtt(name, topic, data_type ) values ('SUNSET','/test/SUNSET','STRING');
replace into io_point (name,direction,io_type,state ) values ('SUNSET','IN','MQTT','21:00');
update io_point,mqtt set io_point.io_idx=mqtt.idx where io_point.name = mqtt.name;

replace into mqtt (name,topic) values ('PORCH_LIGHTS','/home/outside/PorchLight_1/cmnd/power');
replace into io_point (name,direction,io_type) values ('PORCH_LIGHTS','OUT','MQTT');
update io_point,mqtt set io_point.io_idx=mqtt.idx where io_point.name = mqtt.name;

replace into mqtt (name,topic) values ('BACK_FLOODLIGHTS','/home/outside/BackFloodlight/cmnd/power');
replace into io_point (name,direction,io_type) values ('BACK_FLOODLIGHTS','OUT','MQTT');
update io_point,mqtt set io_point.io_idx=mqtt.idx where io_point.name = mqtt.name;

-- 
-- SNMP table
--
replace into snmp (name,oid,ipAddress) values ('FANS','.1.3.6.1.4.1.318.1.1.4.4.2.1.3.8','192.168.10.51');
replace into io_point (name,direction,io_type ) values ('FANS','OUT','SNMP');
update io_point,snmp set io_point.io_idx=snmp.idx where io_point.name = snmp.name;



