create table City (
  ID int,
  Name char(35) unique,
  CountryCode char(3),
  District char(20),
  Population int,
  primary key(ID)
);

create table CountryLanguage (
  CountryCode char(3),
  Language char(30),
  IsOfficial char(1),
  Percentage float,
  primary key(CountryCode)
);
insert into City values (1,'Kabul','AFG','Kabol',1780000);
insert into City values (2,'Qandahar','AFG','Qandahar',237500);
insert into City values (3,'Herat','AFG','Herat',186800);
insert into City values (4,'Mazar-e-Sharif','AFG','Balkh',127800);
insert into City values (5,'Amsterdam','NLD','Noord-Holland',731200);
insert into City values (6,'Rotterdam','NLD','Zuid-Holland',593321);
insert into City values (7,'Haag','NLD','Zuid-Holland',440900);
insert into City values (8,'Utrecht','NLD','Utrecht',234323);
insert into City values (9,'Eindhoven','NLD','Noord-Brabant',201843);
select * from City;
select Name, Population from City where ID=3;
select ID, District from City where Name='Herat';
select * from City where ID>50 and Population>200000;

select Language from CountryLanguage;
select * from CountryLanguage where Percentage>=70.0;
delete from City where ID=3;
delete from City where ID>50 and Population>200000;