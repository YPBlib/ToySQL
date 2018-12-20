delete from tb1;
delete from tb2 where 0;
delete from tb2 where 1;
delete from tb3 where exists (select * from tb5);