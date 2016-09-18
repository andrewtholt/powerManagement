-- 
-- If this returns 0 the Main Feed can safely be switched off
--
select count(*) from hosts,outlets
             where hosts.idx = outlets.hostidx and hosts.name='acs4' and
            outlets.state='ON';
