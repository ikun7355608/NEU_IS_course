create database labs;
use labs;

create table Student (
  sID INT ,
  sName VARCHAR(45),
  score INT ,
  SSSize INT ,
  primary key (sID)
);

create table University (
  uName VARCHAR(45) ,
  uCity VARCHAR(45),
  enrollment INT ,
  primary key (uName)
);

create table Apply (
  sID INT ,
  uName VARCHAR(45) ,
  degree VARCHAR(45) ,
  decision CHAR ,
  primary key (sID,uName,degree),
  foreign key (sID) references Student(sID),
  foreign key (uName) references University(uName)
);

insert into University (uName, uCity, enrollment) values ('DCU', 'Dublin', 8000);
insert into University (uName, uCity, enrollment) values ('UCC', 'Cork', 10000);
insert into University (uName, uCity, enrollment) values ('UCD', 'Dublin', 20000);
insert into University (uName, uCity, enrollment) values ('Imperial', 'London', 8000);

insert into Student (sID, sName, score, SSSize) values (123, 'John', 75, 500);
insert into Student (sID, sName, score, SSSize) values (234, 'Helen', 85, 600);
insert into Student (sID, sName, score, SSSize) values (345, 'Ann', 70, 1000);
insert into Student (sID, sName, score, SSSize) values (456, 'Bill', 90, 300);
insert into Student (sID, sName, score, SSSize) values (567, 'Paul', 55, 1000);
insert into Student (sID, sName, score, SSSize) values (678, 'Mary', 85, 500);
insert into Student (sID, sName, score, SSSize) values (789, 'Ann', 95, 700);
insert into Student (sID, sName, score, SSSize) values (321, 'Michael', 90, 600);
insert into Student (sID, sName, score, SSSize) values (432, 'Kate', 70, 800);
insert into Student (sID, sName, score, SSSize) values (543, 'Peter', 70, 900);
insert into Student (sID, sName, score, SSSize) values (654, 'James', 65, 400);
insert into Student (sID, sName, score, SSSize) values (765, 'Jane', 80, 500);

insert into Apply (sID, uName, degree, decision) values (123, 'DCU', 'CS', 'Y');
insert into Apply (sID, uName, degree, decision) values (123, 'UCC', 'EE', 'Y');
insert into Apply (sID, uName, degree, decision) values (123, 'DCU', 'EE', 'N');
insert into Apply (sID, uName, degree, decision) values (123, 'UCD', 'CS', 'Y');
insert into Apply (sID, uName, degree, decision) values (234, 'Imperial', 'Chemistry', 'N');
insert into Apply (sID, uName, degree, decision) values (345, 'UCC', 'Economics', 'N');
insert into Apply (sID, uName, degree, decision) values (345, 'UCD', 'Economics', 'Y');
insert into Apply (sID, uName, degree, decision) values (345, 'DCU', 'Business', 'N');
insert into Apply (sID, uName, degree, decision) values (345, 'DCU', 'Economics', 'Y');
insert into Apply (sID, uName, degree, decision) values (567, 'DCU', 'CS', 'N');
insert into Apply (sID, uName, degree, decision) values (567, 'UCC', 'CS', 'N');
insert into Apply (sID, uName, degree, decision) values (678, 'UCD', 'EE', 'Y');
insert into Apply (sID, uName, degree, decision) values (678, 'DCU', 'CS', 'Y');
insert into Apply (sID, uName, degree, decision) values (789, 'UCC', 'Physics', 'Y');
insert into Apply (sID, uName, degree, decision) values (321, 'DCU', 'CS', 'Y');
insert into Apply (sID, uName, degree, decision) values (321, 'Imperial', 'CS', 'Y');
insert into Apply (sID, uName, degree, decision) values (432, 'UCD', 'Chemistry', 'Y');
insert into Apply (sID, uName, degree, decision) values (432, 'UCC', 'Chemistry', 'N');
insert into Apply (sID, uName, degree, decision) values (543, 'DCU', 'CS', 'N');
insert into Apply (sID, uName, degree, decision) values (543, 'UCD', 'EE', 'Y');
insert into Apply (sID, uName, degree, decision) values (543, 'Imperial', 'Physics', 'Y');
insert into Apply (sID, uName, degree) values (765, 'DCU', 'CS');
