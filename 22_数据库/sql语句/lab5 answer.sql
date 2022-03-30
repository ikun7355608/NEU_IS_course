select distinct sname,degree
from student join apply on student.sid=apply.sid;

select distinct sname,score,decision
from student join apply on student.sid=apply.sid
where sssize<600 and degree='cs' and uname='dcu'; 

select distinct s.sid,sname,score,u.uname,enrollment
from student s join apply a on (s.sid=a.sid) join university u on (u.uname=a.uname)
where degree is not null;

select sname,degree
from student natural join apply ;

select *
from student natural join apply ;

select *
from student join apply using(sid)
where sssize<=600 and degree='cs' and uname='dcu';

select *
from student s1 join student s2 using(score)
where s1.sid<s2.sid;

select sid,sname,uname,degree
from student s left join apply a using(sid);



