select * from tb1;
select c2 from tb2;
select tb1.c4,tb2.c4 from tb1 ,tb2;
select * from tb3 join tb4  ;
select * from tb3 join tb4 as T ;
select * from tb3 left outer join tb4 on 1;
select * from tb2 natural right join tb3  left outer join tb4 on 1;

select * from tb1 where 'b'>'a';
select * from tb1 where 'b'<'a';
select c7 from tb1 where c2 = 3;
select c7 from tb1 where c2 <> 3;
select c7 from tb1 where c2 < 3;
select c7 from tb1 where c2 > 3;
select c7 from tb1 where c2 >= 3;
select c7 from tb1 where c2 <= 3;

select T.c7 from tb1 as T;
select T.c7 as C_seven from tb1 as T;
select c7 from tb5 where c2 in (1,2,3,4,5,6,7,8,9,10);
select c7 from tb5 where exists (select * from tb1,tb2 where tb1.c1=tb2.c1);
