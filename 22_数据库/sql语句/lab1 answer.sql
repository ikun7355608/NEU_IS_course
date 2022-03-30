use database2020;
-- 1 All the attributes of all the students.
select * from student;

#2 The ID and name of all the students that achieved a score of 75 or more.
select sID,sName
from student
where score>=75;

/*3 The name of each student and the degrees they applied for.
Ensure that there are no duplicate relations.*/
select distinct sname, degree
from student,apply
where student.sID=apply.sID;

/*4 The name, score and decision of the students from schools with less
than or equal to 600 student that applied to take computer science
(CS) from DCU.*/
select distinct sname,score, decision
from student,apply
where student.sID=apply.sID and SSSize<=600 and apply.uName='DCU' and degree='CS';

/*5 The names of the universities with 10,000 or more students that
have students applying for computer science. (Why can you not
query the names of universities with 10,000 or more students
that have students studying for computer science?)*/
select apply.uname
from university,apply
where university.uname=apply.uname and enrollment>=10000 and degree='cs';

/*6 The ID, name, score, university and enrollment of all the students
that have applied for a degree.*/
select distinct student.sid,sname,score,apply.uname, enrollment
from student,apply,university
where student.sid=apply.sid and university.uname=apply.uName;

/*7. The ID, name, score, university and enrollment of all the students
that have applied for a degree ordered by decreasing score.*/
select student.sid,sname,score,apply.uname, enrollment
from student,apply,university
where student.sid=apply.sid and university.uname=apply.uName
order by score desc;

/*8The ID, name, score, university and enrollment of all the students
that have applied for a degree ordered by decreasing score and
increasing enrollment size.*/
select student.sid,sname,score,apply.uname, enrollment
from student,apply,university
where student.sid=apply.sid and university.uname=apply.uName
order by score desc, enrollment;

/*9. The ID, name, score, school size and the score adjusted by the
(school size)/1000. Rename the adjusted score as “normalized
score”.*/
select student.sid,sname,score*SSSize/1000 as Normalized_Score,SSSize
from student;

/*10. All pairs of students that have the same score. Ensure that a
student is not paired with the mself and that each pair is
mentioned once.*/
select s1.*, s2.*
from student s1,student s2
where s1.score=s2.score and s1.sid<s2.sid;

/*A list of IDs for all students that have applied for computer
science (CS) and electronic engineering (EE). (MySQL does not
support the intersect operator so you need to write a query with
the same behavior. Remove duplicates. Why did the duplicates
occur?)*/
select distinct a1.sid
from apply a1,apply a2
where a1.sid=a2.sid and a1.degree='cs' and a2.degree='ee';




