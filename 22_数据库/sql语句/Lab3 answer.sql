use database2020;
# Lab 3
#1 
select distinct sid,sname
from student
where sid in (select sid
              from apply
			  where degree='cs');
              
# 2
select distinct student.sid,sname
from student,apply
where student.sid=apply.sid and degree='cs' ;

#3
select distinct sname
from student,apply
where student.sid=apply.sid and degree='cs' ;

#4 
select distinct sid,sname
from student
where sid in (select sid from apply
			  where degree='cs') and sid not in (select sid from apply
                                                 where degree='ee');
#5
select uname
from university u1
where exists (select * from university u2
			  where ucity=u1.ucity and uname<>u1.uname);			
              
#6
select * from student s1
where not exists (select * from student s2
                  where s2.score>s1.score);
#7
select *
from student
where score>=all(select score from student);
#8
select sid,sname
from student
where sssize>some (select sssize from student);
