use database2020;
#1
select avg(score)
from student;

#2 
select min(score)
from student join apply using(sid)
where degree='cs';

#3                   
select avg(score)
from (select distinct sid,score
      from student join apply using(sid)
       where degree='cs') CS_Student;

#4
select count(distinct sid) count_DCU
from student join apply using(sid)
where uname='DCU';



#5
select cs.s1-NoCS.s2
from (select avg(score) s1
      from (select distinct sid,score
	        from student join apply using(sid)
	        where degree='cs') as csin )as cs,
	  (select avg(score) s2
	   from (select distinct sid, score
	   from student join apply using(sid)
	   where degree<>'cs') as NoCSin)as NoCS ;
       
#5
select cs.s1-el.s2 as differenceScore
from (select avg(score) as s1
from student 
where sID in (select sID from apply where degree = 'CS')) as cs,
(select avg(score) as s2
from student 
where sID in (select sID from apply where degree <> 'CS')) as el;


#6
select uname,count(distinct sid)
from university join apply using(uname)
group by uname;


#7
select uname, degree,max(score),min(score)
from apply join student using(sid)
group by uname,degree;

#8
select sid,sname,count(uname)
from student join apply using(sid)
group by sid;



#9 
(select sid,sname,count(uname)
from student join apply using(sid)
group by sid)
union
(select sid,sname,0
from student 
where sid not in(select sid from apply));


#10
select uname, count(*)
from apply
group by uname
having count(*)>4;

#11
select uname,count(distinct sid)
from university join apply using(uname)
group by uname
having count(distinct sid)>4;


#12
select degree
from (select degree,max(score) as maxscore
      from apply join student using(sid)
	  group by degree) descore
where descore.maxscore<(select avg(score) from student);