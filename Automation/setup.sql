
drop table if exists io_point;

create table io_point (
    name varchar(32) not null unique,
    topic varchar(128) default 'NONE',
    direction varchar(8) not null 
        check(direction='IN' or direction = 'OUT' or direction = 'DISABLED') default 'DISABLED',

    state varchar(8) not null default 'UNKNOWN',

    on_state  varchar(4) default 'ON',
    off_state varchar(4) default 'OFF'

--    state varchar(8) not null 
--        check(state = 'TRUE' or state = 'FALSE' or state = 'UNKNOWN') default 'UNKNOWN'
);
