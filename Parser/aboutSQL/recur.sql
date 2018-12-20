delimiter //
CREATE PROCEDURE `Fibonacci`(IN POS INT, IN RANG INT, IN LIMTED INT)
BEGIN
       select FORMAT(Fibonacci,0) AS Fibonacci from 
       (
       select @f0 Fibonacci, @fn:=@f1+@f0, @f0:=@f1, @f1:=@fn 
       from    (select @f0:=0, @f1:=1, @fn:=1) x, 
       information_schema.STATISTICS p limit LIMTED
       ) y LIMIT POS, RANG;
END//
delimiter ;





DROP PROCEDURE  IF EXISTS mysp_1;
DELIMITER $$

CREATE PROCEDURE `mysp_1`(
   IN leaf INT,
   OUT result INT
)
BEGIN
    DECLARE i1 INT DEFAULT 1 ;
    DECLARE i2 INT DEFAULT 1;
    DECLARE i3 INT DEFAULT 1;

    select c2 from t1 where c1=leaf into i1;
    if i1 is null then set result = 1;
    else call mysp_1(i1,i3);
    set result = i1*i3;
    end if;

END$$

delimiter ;







