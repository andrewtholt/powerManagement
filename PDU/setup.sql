drop table if exists hosts;
drop table if exists outlets;
drop table if exists ups;

create table hosts 
    ( idx integer primary key autoincrement,
      name varchar(32) not null unique,
      ip varchar(32),
      mac varchar(32),
      --
      -- type is used by the pdu processing,
      -- Currently apc and cyc are used.
      -- Could be used to hold model info.
      --
      type varchar(8) default 'UNKNOWN',
      -- snmp port
      port integer default 161,
      rw_community varchar(32) default 'private',
      ro_community varchar(32) default 'public',
      on_value integer,
      off_value integer,
      reboot_value integer,
      status varchar(8) default 'UNKNOWN',
      ping_count integer default 3,
      ping_counter integer default -1,
      touched date default CURRENT_TIMESTAMP
      );

create table outlets
    ( idx integer primary key autoincrement,
      hostidx int not null,
      name varchar(32) unique,
      delay integer default 10,
      oid varchar(128),
      number int,
      state varchar(8) default 'UNKNOWN',
      requested_state varchar(8) default 'NA',
      initial_state varchar(8) default 'OFF',
      -- start state
      --
      -- Valid values: RESTORE, ON, OFF
      --
      start_state varchar(8) default 'RESTORE',
      locked varchar(4) default 'NO',
      shutdown_cmd varchar(64) default 'NONE',
      pf_action varchar(64) default 'NONE',
      pf_state varchar(8) default 'UNKNOWN',
      touched date default CURRENT_TIMESTAMP
    );

create table ups
    (   idx integer primary key autoincrement,
        name varchar(32) not null unique,
        charge integer default -1,
        threshold integer integer default 99,
        --
        -- state 
        --   UNKNOWN
        --   ONBATT
        --   ONLINE
        port integer default 161,
        state varchar(8) not null default 'UNKNOWN',
        delay integer default 60,
        touched date default CURRENT_TIMESTAMP
    );

