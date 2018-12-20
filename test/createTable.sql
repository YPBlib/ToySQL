create table tb1
(
    c1 int unique,
    c2 int,
    c3 float,
    c4 char,
    c5 char(1),
    c6 char(127),
    c7 char(255),
    primary key(c7)
);

create table tb2
(
    c1 int unique key,
    c2 int,
    c3 float,
    c4 char,
    c5 char(1),
    c6 char(127),
    c7 char(255),
    primary key(c7)
);

create table tb3
(
    c1 int unique key,
    c2 int ,
    c3 float,
    c4 char,
    c5 char(1),
    c6 char(127),
    c7 char(255),
    key(c7)
);

create table tb4
(
    c1 int unique key not null,
    c2 int,
    c3 float,
    c4 char,
    c5 char(1),
    c6 char(127),
    c7 char(255) key
);



create table tb5
(
    c1 int unique ,
    c2 int,
    c3 float,
    c4 char,
    c5 char(1),
    c6 char(127),
    c7 char(255) primary key
);