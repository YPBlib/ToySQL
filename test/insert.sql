insert into tb1 values(2,3,3.2,"r","w","abcd","qwe");
insert into tb2 value(2,3,3.2,"r","w","abcd","qwe");
insert into tb3 value(2+6*9,3,-.2e30,"r","w","abcd","qwe");
insert into tb4 value(2,3,3.2,"r","w","ab\"'cd","qwe");
insert into tb4 value(4,3,3.2,"r","w","ab\"'cd","qw""e");
insert into tb5 value(2,3,3.2,"r","w","ab\"'cd","qw" "e");
insert into tb1 value(1,3,3.2,"r","w","ab\"\n\r\b\v\t'cd"," qwe ");