For each new user, create a new config file and modify the user field as:
user=<new_username>;

For a default_user role only the user is necessary in the config file.

Then, go to the database and create a table with their username and 
grant permissions upon the table to the user. To do it, create and 
execute the Sql_create_<new_username>.sql query in the db 
(copy an existing one and replace <new_username> w/ real user name).

Used query:
CREATE TABLE `<new_username>` (
  `RowName` int(11) NOT NULL AUTO_INCREMENT,
  `MaterialName` varchar(50) NOT NULL,
  `UMaterialMatch` varchar(50) DEFAULT NULL,
  `FatherName` varchar(50) DEFAULT NULL,
  `TextureNames` varchar(500) DEFAULT NULL,
  `ScalarParamValues` varchar(500) DEFAULT NULL,
  `VectorParamValues` varchar(500) DEFAULT NULL,
  PRIMARY KEY (`RowName`),
  UNIQUE KEY `MaterialName_UNIQUE` (`MaterialName`)
) ENGINE=InnoDB AUTO_INCREMENT=21 DEFAULT CHARSET=latin1;

GRANT SELECT, UPDATE, DELETE, INSERT ON <new_username> to `default_user`@`%`; 