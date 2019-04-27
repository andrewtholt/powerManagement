
-- delete from io_point;
-- delete from mqtt;

replace into mqtt (topic ) values ('/test/start');
replace into io_point (name,direction ) values ('START','IN');

replace into mqtt (topic ) values ('/test/stop');
replace into io_point (name,direction ) values ('STOP','IN');

replace into mqtt (topic ) values ('/test/motor');
replace into io_point (name,direction ) values ('MOTOR','OUT');

update io_point,mqtt set io_point.io_idx=mqtt.idx where io_point.name = mqtt.name;

