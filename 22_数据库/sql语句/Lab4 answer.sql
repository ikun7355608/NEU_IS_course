use database2020;
#1
select sid,sname,score,score*sssize/1000 NormalScore
from student
where abs(score-score*sssize/1000)>20;/*no subquery*/

select sid,sname,score,NormalScore
from (select sid,sname,score,score*sssize/1000 NormalScore from student) s
where abs(score-NormalScore)>20;

#2 
select uname,max(score)
from student join apply using(sid)
group by uname;/*only max score*/

select uname,sname
from student s1 join apply a1 using (sid)
where score=(select max(score)
			 from student s2,apply a2
			 where s2.sid=a2.sid and a2.uname=a1.uname);


select uname, (select sname
               from student s1 join apply a1 using(sid)
               where u.uname=a1.uname and score>=all (select score
                                 from student s2 join apply a2 using(sid)
								 where u.uname=a2.uname)) as MaxName
from university u;



